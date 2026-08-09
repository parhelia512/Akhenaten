#include "window_empire.h"

#include "city/military.h"
#include "city/city.h"
#include "game/game_events.h"
#include "city/city_warnings.h"
#include "city/constants.h"
#include "empire/empire.h"
#include "empire/empire_city.h"
#include "empire/empire_map.h"
#include "empire/empire_object.h"
#include "empire/empire_traders.h"
#include "empire/trade_route.h"
#include "empire/type.h"
#include "scenario/distant_battle.h"
#include "graphics/image.h"
#include "graphics/graphics.h"
#include "graphics/elements/generic_button.h"
#include "graphics/elements/image_button.h"
#include "graphics/elements/lang_text.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "input/input.h"
#include "input/mouse.h"
#include "input/scroll.h"
#include "scenario/empire.h"
#include "scenario/scenario_invasion.h"
#include "scenario/scenario.h"
#include "window/window_city.h"
#include "game/game_config.h"
#include "platform/renderer.h"
#include "game/game.h"
#include "core/profiler.h"
#include "js/js_game.h"
#include "graphics/elements/ui_js.h"
#include "dev/debug.h"
#include <cmath>

static const vec2i EMPIRE_MAP_SIZE_PX{1200, 1600};

static int consume_scroll_remainder(float& remainder) {
    const int step = (remainder >= 0.f)
      ? (int)std::floor(remainder)
      : (int)std::ceil(remainder);
    remainder -= step;
    return step;
}

using empire_object_tokens_t = token_holder<e_empire_object, EMPIRE_OBJECT_ORNAMENT, EMPIRE_OBJECT_COUNT>;
const empire_object_tokens_t ANK_CONFIG_ENUM(empire_object_tokens);

struct object_trade_info {
    rect r;
    e_resource res;
};

empire_window g_empire_window;

struct empire_window_draw {
    vec2i draw_offset;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw, draw_offset);

struct empire_window_draw_trade_route {
    vec2i draw_offset;
    int route_id = 0;
    int effect = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_trade_route, draw_offset, route_id, effect);

struct empire_window_draw_trader {
    vec2i draw_offset;
    int index;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_trader, draw_offset, index);

struct empire_window_draw_battle_icon {
    vec2i draw_offset;
    vec2i pos;
    int image_id = 0;
    int path = 0;
    int years = 0;
    int object_index = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_battle_icon, draw_offset, pos, image_id, path, years, object_index);

struct empire_window_draw_city {
    vec2i draw_offset;
    vec2i pos;
    int object_index = 0;
    int city_id = 0;
    int text_align = 0;
    int width = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_city, draw_offset, pos, object_index, city_id, text_align, width);

struct empire_window_draw_text {
    vec2i draw_offset;
    vec2i pos;
    xstring label;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_text, draw_offset, pos, label);

struct empire_window_draw_sprite {
    vec2i draw_offset;
    vec2i pos;
    int image_id = 0;
    int object_index = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_sprite, draw_offset, pos, image_id, object_index);

struct empire_window_init_event {
    vec2i pos;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_init_event, pos);

struct empire_window_draw_background_evt {
    int pad = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_draw_background_evt, pad);

struct empire_window_object_info_evt {
    int unused = 0;
};
ANK_REGISTER_STRUCT_WRITER(empire_window_object_info_evt, unused);

void empire_window::init() {
    int selected_object = g_empire_map.selected_object();
    g_empire_map.selected_city = selected_object ? g_empire.get_city_for_object(selected_object - 1) : 0;
    scroll_remainder_x = 0.f;
    scroll_remainder_y = 0.f;
    left_panning = false;
    left_pan_travel = 0;
    left_pan_last_pos = {0, 0};

    ui.begin_widget(pos);
    ui.event(empire_window_init_event{pos}, get_section(), "init");
    ui.end_widget();
}

void empire_window::archive_load(archive arch) {
    autoconfig_window::archive_load(arch);

    start_pos = arch.r_vec2i("start_pos");
    finish_pos = arch.r_vec2i("finish_pos");
    arch.r_desc("image", image);

    init();
}

void empire_window::draw_trade_route(const empire_city* city, int object_index, bool force) {
    if (!city) {
        return;
    }

    if (city->type != EMPIRE_CITY_EGYPTIAN_TRADING && city->type != EMPIRE_CITY_FOREIGN_TRADING
        && city->type != EMPIRE_CITY_PHARAOH_TRADING) {
        return;
    }

    e_empire_route_state state = ROUTE_CLOSED;
    if (city->is_open) {
        state = (g_empire_map.selected_object()
                  && g_empire_map.selected_city == g_empire.get_city_for_object(object_index))
                  ? ROUTE_OPEN_SELECTED
                  : ROUTE_OPEN;
    } else {
        state = (g_empire_map.selected_object()
                  && g_empire_map.selected_city == g_empire.get_city_for_object(object_index))
                  ? ROUTE_CLOSED_SELECTED
                  : ROUTE_CLOSED;
    }

    if ((state == ROUTE_OPEN_SELECTED || state == ROUTE_CLOSED_SELECTED) && !force) {
        deffer_city_route_id = city->lookup_id;
        return;
    }

    const map_route_object& obj = g_empire.get_route_object(city->route_id);
    if (!obj.in_use || state == ROUTE_CLOSED) {
        return;
    }

    ui::event(empire_window_draw_trade_route{draw_offset, city->route_id, (int)state}, get_section(), "draw_map",
      empire_object_tokens.name(EMPIRE_OBJECT_TRADE_ROUTE));
}

void empire_window::draw_object_info() {
    ui.event(empire_window_object_info_evt{}, get_section(), "draw_object_info");
}

void empire_window::determine_selected_object(const mouse* m) {
    if (!m->left.went_up || finished_scroll || is_outside_map(m->x, m->y)) {
        finished_scroll = 0;
        return;
    }

    const vec2i origin = map_base_origin();
    const float scale = map_scale();
    const vec2i map_pos{
        std::max(0, (int)std::lround((m->x - origin.x) / std::max(0.001f, scale))),
        std::max(0, (int)std::lround((m->y - origin.y) / std::max(0.001f, scale)))
    };

    g_empire_map.select_object(map_pos);
}

bool empire_window::is_outside_map(int x, int y) {
    const vec2i origin = map_clip_origin();
    const vec2i size = map_area_size_pixels();
    return x < origin.x || x >= origin.x + size.x || y < origin.y || y >= origin.y + size.y;
}

int empire_window::ui_handle_mouse(const mouse* m) {
    const hotkeys* h = hotkey_state();

    last_mouse_pos = {m->x, m->y};

    // Left-drag: 1:1 grab pan (map follows the cursor). Skip camera-scroll stack.
    if (!m->is_touch && m->left.went_down && !is_outside_map(m->x, m->y)) {
        left_panning = true;
        left_pan_travel = 0;
        left_pan_last_pos = {m->x, m->y};
    }

    if (!m->is_touch && left_panning && m->left.is_down) {
        const int dx = m->x - left_pan_last_pos.x;
        const int dy = m->y - left_pan_last_pos.y;
        if (dx || dy) {
            const float scale = std::max(0.001f, map_scale());
            // Negative: drag right → content follows right (scroll decreases).
            scroll_remainder_x += -dx / scale;
            scroll_remainder_y += -dy / scale;
            const vec2i map_delta{
                consume_scroll_remainder(scroll_remainder_x),
                consume_scroll_remainder(scroll_remainder_y)
            };
            if (map_delta.x || map_delta.y) {
                g_empire_map.scroll_map(map_delta);
            }
            left_pan_travel += std::abs(dx) + std::abs(dy);
            left_pan_last_pos = {m->x, m->y};
        }
    }

    if (!m->is_touch && m->left.went_up) {
        finished_scroll = left_pan_travel > g_scroll.config.drag_min_delta ? 1 : 0;
        left_panning = false;
        left_pan_travel = 0;
    }

    if (!left_panning) {
        vec2i position = g_scroll.get_delta(m, scroll_t::EMPIRE);
        if (position.x || position.y) {
            const float scale = map_scale();
            scroll_remainder_x += position.x / std::max(0.001f, scale);
            scroll_remainder_y += position.y / std::max(0.001f, scale);
            const vec2i map_delta{
                consume_scroll_remainder(scroll_remainder_x),
                consume_scroll_remainder(scroll_remainder_y)
            };
            if (map_delta.x || map_delta.y) {
                g_empire_map.scroll_map(map_delta);
            }
        }
    }

    if (!!game_features::gameopt_middle_mouse_camera_pan
        && m->middle.went_down && !is_outside_map(m->x, m->y)) {
        g_scroll.drag_start(scroll_t::drag_source::middle_mouse_pan);
    }

    if (m->is_touch) {
        const touch_t* t = get_earliest_touch();
        if (!is_outside_map(t->current_point.x, t->current_point.y)) {
            if (t->has_started) {
                is_scrolling = 1;
                g_scroll.drag_start(scroll_t::drag_source::touch);
            }
        }

        if (t->has_ended) {
            is_scrolling = 0;
            finished_scroll = !touch_was_click(t);
            g_scroll.drag_end();
        }
    }

    if (m->middle.went_up) {
        g_scroll.drag_end();
    }

    determine_selected_object(m);

    int selected_object = g_empire_map.selected_object();
    if (selected_object) {
        const empire_object* obj = g_empire.get_object(selected_object - 1);
        if (obj->type == EMPIRE_OBJECT_CITY) {
            g_empire_map.selected_city = g_empire.get_city_for_object(selected_object - 1);
        }

        if (input_go_back_requested(m, h)) {
            g_empire_map.clear_selected_object();
            return 0;
        }
    } else if (input_go_back_requested(m, h)) {
        g_scroll.drag_end();
        window_city_show();
        return 0;
    }

    ui.begin_widget({0, 0});
    ui.handle_mouse(m);
    ui.end_widget();

    return 0;
}

void empire_window::draw_empire_object(int object_index, const empire_object& obj) {
    // Routes are drawn via draw_trade_route(); pak Cleopatra IDs here are unmapped and cover cities.
    if (obj.type == EMPIRE_OBJECT_LAND_TRADE_ROUTE || obj.type == EMPIRE_OBJECT_SEA_TRADE_ROUTE
        || obj.type == EMPIRE_OBJECT_TRADER) {
        return;
    }

    vec2i pos;
    int image_id;
    if (scenario_empire_is_expanded()) {
        pos = obj.expanded.pos;
        image_id = obj.expanded.image_id;
    } else {
        pos = obj.pos;
        image_id = obj.image_id;
    }

    const e_empire_object ot = (e_empire_object)obj.type;
    if (ot == EMPIRE_OBJECT_CITY) {
        const int city_id = g_empire.get_city_for_object(object_index);
        const empire_city* city = g_empire.city(city_id);
        if (!city || !city->shows_as_trade_city_on_map()) {
            return;
        }
        ui.event(empire_window_draw_city{draw_offset, pos, object_index, city_id, obj.text_align, obj.width},
          get_section(), "draw_map", empire_object_tokens.name(ot));
        return;
    }

    if (ot == EMPIRE_OBJECT_TEXT) {
        const full_empire_object* full = g_empire.get_full_object(object_index);
        xstring label;
        if (full && !!full->text_key) {
            label = lang_xtext_from_key(full->text_key);
        } else if (full) {
            label = ui::str(196, full->city_name_id);
        }
        ui.event(empire_window_draw_text{draw_offset, pos, label}, get_section(), "draw_map",
          empire_object_tokens.name(ot));
        return;
    }

    if (ot == EMPIRE_OBJECT_BATTLE_ICON) {
        ui.event(empire_window_draw_battle_icon{draw_offset, pos, image_id, obj.invasion_path_id, obj.invasion_years,
                   object_index},
          get_section(), "draw_map", empire_object_tokens.name(ot));
        return;
    }

    if (ot == EMPIRE_OBJECT_ENEMY_ARMY) {
        if (g_distant_battle.battle.months_until_battle <= 0
            || g_distant_battle.enemy_months_traveled() != obj.distant_battle_travel_months) {
            return;
        }
    }

    if (ot == EMPIRE_OBJECT_KINGDOME_ARMY) {
        if (!g_distant_battle.kingdome_army_is_traveling()
            || city_military_distant_battle_kingdome_months_traveled() != obj.distant_battle_travel_months) {
            return;
        }
    }

    ui.event(empire_window_draw_sprite{draw_offset, pos, image_id, object_index}, get_section(), "draw_map",
      empire_object_tokens.name(ot));
}

void empire_window::draw_map() {
    const vec2i clip_origin = map_clip_origin();
    const vec2i pixel_view = map_area_size_pixels();
    const vec2i map_view = map_viewport_size();
    const float scale = map_scale();

    graphics_set_clip_rectangle(clip_origin, pixel_view);

    g_empire_map.set_viewport(map_view);

    draw_offset = map_draw_origin();
    ui::set_tooltip({});
    deffer_city_route_id = -1;

    painter ctx = game.painter();
    image_desc map_bg = g_empire.map_background.valid() ? g_empire.map_background : image;
    if (const image_t* map_img = image_get(map_bg.tid())) {
        sprite spr;
        spr.img = map_img;
        ctx.draw(spr, draw_offset, COLOR_MASK_NONE, scale, scale);
    }

    g_empire.foreach_object(
      [this](int object_index, const empire_object& obj) { draw_empire_object(object_index, obj); });

    scenario_invasion_foreach_warning([&](vec2i pos, int image_id) {
        if (const image_t* warning_img = image_get(image_id)) {
            sprite spr;
            spr.img = warning_img;
            ctx.draw(spr, map_to_screen(pos), COLOR_MASK_NONE, scale, scale);
        }
    });

    for (auto& trader : g_empire_traders.traders) {
        if (!trader.is_active) {
            continue;
        }
        ui.event(empire_window_draw_trader{draw_offset, trader.id}, get_section(), __func__,
          empire_object_tokens.name(EMPIRE_OBJECT_TRADER));
    }

    ui.event(empire_window_draw{draw_offset}, get_section(), __func__,
      empire_object_tokens.name(EMPIRE_OBJECT_DISTANT_BATTLE_ROUTE));

    if (deffer_city_route_id >= 0) {
        const empire_city* deferred_city = g_empire.city(deffer_city_route_id);
        if (deferred_city) {
            draw_trade_route(deferred_city, deferred_city->empire_object_id, true);
        }
    }

    ui.begin_widget(pos);
    ui.event(empire_window_draw{draw_offset}, get_section(), __func__);
    ui.end_widget();

    graphics_reset_clip_rectangle();
}

void empire_window::draw_paneling() {
    ui.event(empire_window_draw{pos}, get_section(), __func__);
}

void empire_window::ui_draw_foreground(UiFlags flags) {
    draw_map();

    const empire_city* city = nullptr;
    int selected_object = g_empire_map.selected_object();
    if (selected_object) {
        const empire_object* object = g_empire.get_object(selected_object - 1);
        if (object->type == EMPIRE_OBJECT_CITY) {
            g_empire_map.selected_city = g_empire.get_city_for_object(selected_object - 1);
            city = g_empire.city(g_empire_map.selected_city);
        }
    }

    ui["city_name"] = "";
    ui["button_help"].enabled = !!city;
    ui["button_close"].enabled = true;
    ui["button_advisor"].enabled = !!city;
    ui["city_name"] = city ? city->name_str : "";

    const bool may_open_trade = city && !city->is_open && city->can_trade();
    ui["button_open_trade"].enabled = may_open_trade;
    if (may_open_trade) {
        ui["button_open_trade"].text_var("%s %d %s", ui::str_from_key("#debens"), city->cost_to_open,
          ui::str(sell_res_group, 6 + city->is_sea_trade));
    }

    draw_paneling();

    ui.begin_widget({0, 0});
    ui.draw(flags);
    draw_object_info();
    ui.end_widget();
}

void window_empire_show() {
    static window_type window = {"window_empire",
      [](int flags) { g_empire_window.draw_background(flags); },
      [](int flags) { g_empire_window.ui_draw_foreground(flags); },
      [](const mouse* m, const hotkeys* h) { g_empire_window.ui_handle_mouse(m); }
    };

    g_empire_window.init();
    window_show(&window);
}

void window_empire_show_checked() {
    const e_availability avail = g_city.is_empire_available();

    const bool is_custom_map = (g_scenario.mode() != e_scenario_normal);
    if (avail == AVAILABLE || is_custom_map) {
        window_empire_show();
    } else {
        pcstr text = (avail == NOT_AVAILABLE ? "#not_available_in_this_assignment" : "#not_available_yet");
        events::emit(event_city_warning{text});
    }
}
ANK_FUNCTION(window_empire_show_checked)

void __empire_window_set_map_bounds(int min_x, int min_y, int max_x, int max_y) {
    g_empire_window.min_pos = {min_x, min_y};
    g_empire_window.max_pos = {max_x, max_y};
}
ANK_FUNCTION_4(__empire_window_set_map_bounds)

void __empire_window_draw_city_trade_route(int city_id, int object_index, int force) {
    g_empire_window.draw_trade_route(g_empire.city(city_id), object_index, force != 0);
}
ANK_FUNCTION_3(__empire_window_draw_city_trade_route)

int __empire_update_map_animation(int object_index, int image_id) {
    const empire_object* obj = g_empire.get_object(object_index);
    if (!obj || image_id <= 0) {
        return 0;
    }
    return g_empire.update_animation(object_index, *obj, image_id);
}
ANK_FUNCTION_2(__empire_update_map_animation)

int __empire_city_image_id(int city_type) {
    return empire_city_images.image_id((e_empire_city)city_type, scenario_empire_is_expanded());
}
ANK_FUNCTION_1(__empire_city_image_id)

vec2i empire_window::map_clip_origin() const {
    return min_pos + start_pos;
}

vec2i empire_window::map_area_size_pixels() const {
    return {
        std::max(1, max_pos.x - min_pos.x - finish_pos.x),
        std::max(1, max_pos.y - min_pos.y - finish_pos.y)
    };
}

float empire_window::map_scale() const {
    const vec2i size = map_area_size_pixels();
    const float scale_x = size.x / static_cast<float>(EMPIRE_MAP_SIZE_PX.x);
    const float scale_y = size.y / static_cast<float>(EMPIRE_MAP_SIZE_PX.y);
    return std::max(scale_x, scale_y);
}

vec2i empire_window::map_viewport_size() const {
    const vec2i pixel_size = map_area_size_pixels();
    const float scale = map_scale();
    return {
        std::min(EMPIRE_MAP_SIZE_PX.x, std::max(1, (int)std::lround(pixel_size.x / std::max(0.001f, scale)))),
        std::min(EMPIRE_MAP_SIZE_PX.y, std::max(1, (int)std::lround(pixel_size.y / std::max(0.001f, scale))))
    };
}

vec2i empire_window::map_draw_origin() const {
    const vec2i scroll = g_empire_map.get_scroll();
    const float scale = map_scale();
    return map_base_origin() - vec2i{
        (int)std::lround(scroll.x * scale),
        (int)std::lround(scroll.y * scale)
    };
}

vec2i empire_window::map_base_origin() const {
    const vec2i clip_origin = map_clip_origin();
    const vec2i clip_size = map_area_size_pixels();
    const float scale = map_scale();
    const vec2i scaled_map_size{
        std::max(1, (int)std::lround(EMPIRE_MAP_SIZE_PX.x * scale)),
        std::max(1, (int)std::lround(EMPIRE_MAP_SIZE_PX.y * scale))
    };
    return clip_origin + vec2i{
        std::max(0, (clip_size.x - scaled_map_size.x) / 2),
        std::max(0, (clip_size.y - scaled_map_size.y) / 2)
    };
}

vec2i empire_window::map_to_screen(vec2i map_pos) const {
    const float scale = map_scale();
    return map_draw_origin() + vec2i{
        (int)std::lround(map_pos.x * scale),
        (int)std::lround(map_pos.y * scale)
    };
}
