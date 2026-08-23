#include "city/object_info.h"

#include "window/window_info.h"
#include "game/game_events.h"
#include "js/js_game.h"
#include "js/js_global_object.h"

ANK_GLOBAL_OBJECT(common_info_window::get_object_info(), __object_info,
    bid,
    group_id,
    grid_offset,
    offset,
    figure_selected_index);

int __object_info_figure_count() {
    return (int)common_info_window::get_object_info().figure_ids.size();
}
ANK_FUNCTION(__object_info_figure_count)

int __object_info_figure_id() {
    return common_info_window::get_object_info().figure_get_id();
}
ANK_FUNCTION(__object_info_figure_id)

void __object_info_select_figure(int index) {
    object_info &info = common_info_window::get_object_info();
    if (index < 0 || index >= (int)info.figure_ids.size()) {
        return;
    }

    info.figure_selected_index = (uint8_t)index;
    info.can_play_sound = true;
    events::emit(event_update_tile_info{true});
}
ANK_FUNCTION_1(__object_info_select_figure)
