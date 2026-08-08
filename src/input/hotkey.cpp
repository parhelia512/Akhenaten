#include "hotkey.h"

#include "core/app.h"
#include "editor/editor.h"
#include "graphics/video.h"
#include "graphics/window.h"
#include "game/game_events.h"
#include "input/scroll.h"
#include "config/hotkeys.h"
#include "window/popup_dialog.h"
#include "game/game.h"

#include <algorithm>
#include <array>
#include <cstring>

struct hotkey_definition {
    int* action = nullptr;
    int value;
    int key;
    int modifiers;
    int repeatable;
    std::function<void()> callback;
};

struct arrow_definition {
    void (*action)(int is_down);
    int key;
};

struct hotkey_data_t {
    hotkeys hotkey_state;

    svector<hotkey_definition, 128> definitions;
    svector<arrow_definition, 16> arrows;
};

hotkey_data_t g_hotkey_data;

static bool hotkey_action_is_repeatable(e_hotkey_action action) {
    switch (action) {
    case HOTKEY_INCREASE_GAME_SPEED:
    case HOTKEY_DECREASE_GAME_SPEED:
    case HOTKEY_DEBUG_1_UP:
    case HOTKEY_DEBUG_1_DOWN:
    case HOTKEY_DEBUG_RENDER_UP:
    case HOTKEY_DEBUG_RENDER_DOWN:
        return true;
    default:
        return false;
    }
}

static void add_definition(const hotkey_mapping& mapping, bool alt) {
    auto& data = g_hotkey_data;

    e_key key = alt ? mapping.alt.key : mapping.state.key;
    e_key_mode modifiers = alt ? mapping.alt.modifiers : mapping.state.modifiers;

    if (key == KEY_NONE) {
        return;
    }

    hotkey_definition *def = &data.definitions.emplace_back();
    def->key = key;
    def->modifiers = modifiers;
    def->value = 1;
    def->repeatable = hotkey_action_is_repeatable(mapping.action) ? 1 : 0;
    def->callback = [action = mapping.action] {
        events::emit(event_hotkey_fired{ (int)action });
    };
}

static void add_arrow(const hotkey_mapping& mapping, bool alt) {
    auto& data = g_hotkey_data;
    e_key key = alt ? mapping.alt.key : mapping.state.key;

    if (key == KEY_NONE) {
        return;
    }

    arrow_definition* arrow = &data.arrows.emplace_back();
    arrow->key = key;

    switch (mapping.action) {
    case HOTKEY_ARROW_UP:
        arrow->action = scroll_arrow_up;
        break;
    case HOTKEY_ARROW_DOWN:
        arrow->action = scroll_arrow_down;
        break;
    case HOTKEY_ARROW_LEFT:
        arrow->action = scroll_arrow_left;
        break;
    case HOTKEY_ARROW_RIGHT:
        arrow->action = scroll_arrow_right;
        break;
    default:
        arrow->action = 0;
        break;
    }
}

void hotkeys::install(const xspan<hotkey_mapping> &mappings) {
    auto& data = g_hotkey_data;

    data.definitions.clear();
    data.arrows.clear();

    // Fixed keys: Escape and Enter
    hotkey_definition &escape_def = data.definitions.emplace_back();

    escape_def.action = &data.hotkey_state.enter_pressed;
    escape_def.key = KEY_ENTER;
    escape_def.modifiers = 0;
    escape_def.repeatable = 0;
    escape_def.value = 1;

    hotkey_definition &enter_def = data.definitions.emplace_back();
    enter_def.action = &data.hotkey_state.escape_pressed;
    enter_def.key = KEY_ESCAPE;
    enter_def.modifiers = 0;
    enter_def.repeatable = 0;
    enter_def.value = 1;

    std::array<e_hotkey_action, 4> arrow_actions = {
        HOTKEY_ARROW_UP,
        HOTKEY_ARROW_DOWN,
        HOTKEY_ARROW_LEFT,
        HOTKEY_ARROW_RIGHT
    };

    for (int i = 0; i < mappings.size(); i++) {
        int action = mappings[i].action;
        const bool is_arrow_action = std::find(arrow_actions.begin(), arrow_actions.end(), action) != arrow_actions.end();
        if (is_arrow_action) {
            add_arrow(mappings[i], false);
            add_arrow(mappings[i], true);
        } else {
            add_definition(mappings[i], false);
            add_definition(mappings[i], true);
        }
    }
}

const hotkeys* hotkey_state(void) {
    return &g_hotkey_data.hotkey_state;
}

void hotkey_reset_state(void) {
    auto& data = g_hotkey_data;
    memset(&data.hotkey_state, 0, sizeof(data.hotkey_state));
}

void hotkey_key_pressed(int key, int modifiers, int repeat) {
    auto& data = g_hotkey_data;
    if (g_window_manager.window_is("window_hotkey_editor")) {
        hotkey_editor_notify_key_pressed(key, modifiers);
        return;
    }

    if (key == KEY_NONE) {
        return;
    }

    for (auto &arrow: data.arrows) {
        if (arrow.key == key)
            arrow.action(1);
    }

    for (auto &def: data.definitions) {
        if (def.key == key && def.modifiers == modifiers && (!repeat || def.repeatable)) {
            if (def.action) {
                *(def.action) = def.value;
            }

            if (def.callback) {
                def.callback();
            }
        }
    }
}

void hotkey_key_released(int key, int modifiers) {
    auto& data = g_hotkey_data;
    if (g_window_manager.window_is("window_hotkey_editor")) {
        hotkey_editor_notify_key_released(key, modifiers);
        return;
    }

    if (key == KEY_NONE) {
        return;
    }

    for (auto &arrow : data.arrows) {
        if (arrow.key == key)
            arrow.action(0);
    }
}

void hotkey_handle_escape(void) {
    video_stop();
    // Editor: no Ironwill checkpoint (city save would clobber the slot).
    if (editor_is_active()) {
        popup_dialog::show_yesno("#popup_dialog_quit", [](bool accepted) {
            if (accepted) {
                game_exit_editor();
            }
        });
        return;
    }
    // City: same Exit-to-menu flow as File→Exit (Ironwill checkpoint in JS).
    events::emit(event_exit_to_menu_requested{ 0 });
}
