#include "window_terrain_info.h"

#include "city/object_info.h"
#include "sound/sound.h"
#include "window/building/common.h"
#include "window/message_dialog.h"
#include "js/js_struct.h"
#include "graphics/elements/ui_js.h"

#include <algorithm>

struct event_classify_terrain_info { int reserved = 0; };
ANK_REGISTER_STRUCT_WRITER(event_classify_terrain_info, reserved);

struct terrain_info_window_init {
    vec2i pos;
    int grid_offset;
    int terrain_type;
};
ANK_REGISTER_STRUCT_WRITER(terrain_info_window_init, pos, grid_offset, terrain_type);

terrain_info_window::terrain_info_window() {
    window_terrain_register_handler(this);
}

void terrain_info_window::archive_load(archive arch) {
    common_info_window::archive_load(arch);
    arch.r("related_terrain", related_terrain);
    arch.r("help_id", help_id);
}

bool terrain_info_window::check(object_info &c) {
    if (c.terrain_type == terrain_info_none) {
        ui::event(event_classify_terrain_info{});
        if (c.terrain_type == terrain_info_none) {
            c.terrain_type = terrain_info_empty;
        }
    }
    return std::find(related_terrain.begin(), related_terrain.end(), c.terrain_type) != related_terrain.end();
}

void terrain_info_window::window_info_background(object_info &c) {
    ui_scope_property holder;
    ui.format_all(&holder);
}

void terrain_info_window::update(object_info &c) {
}

void terrain_info_window::init(object_info &c) {
    common_info_window::init(c);

    if (c.can_play_sound && !open_sounds.empty()) {
        c.can_play_sound = 0;
        const xstring &wav = open_sounds[rand() % open_sounds.size()];
        window_building_play_sound(&c, wav.c_str());
    }

    ui.begin_widget(pos);
    ui.event(terrain_info_window_init{pos, c.grid_offset, (int)c.terrain_type}, section(), __func__);
    ui.end_widget();

    xstring help = help_id;
    if (!help.empty()) {
        c.help_link = help;
    } else {
        c.help_link = {};
        help = "message_table_of_contents";
    }
    c.help_id = 0;
    window_message_setup_help_id(help);
}
