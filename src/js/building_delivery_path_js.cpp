#include "city/city_recorded_paths.h"

#include "building/building.h"
#include "building/construction/build_planner.h"
#include "city/object_info.h"
#include "core/bstring.h"
#include "core/variant.h"
#include "game/game.h"
#include "graphics/color.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "graphics/view/view.h"
#include "graphics/window.h"
#include "input/keyboard.h"
#include "js/js_game.h"
#include "widget/widget_city.h"
#include "window/window_info.h"

namespace {

building *resolve_hover_or_info_building() {
    if (g_window_manager.window_is("window_building_info")) {
        building_id bid = common_info_window::get_object_info().bid;
        if (bid) {
            building *b = building_get(bid);
            if (b && b->is_valid()) {
                return b->main();
            }
        }
        return nullptr;
    }

    if (!g_screen_city.current_tile.valid()) {
        return nullptr;
    }
    building *b = building_at(g_screen_city.current_tile);
    if (!b || !b->is_valid()) {
        return nullptr;
    }
    return b->main();
}

void draw_recorded_path(int path_id, color mask) {
    const auto &tiles = g_recorded_paths.tiles(path_id);
    if (tiles.empty()) {
        return;
    }
    painter ctx = game.painter();
    const int img = image_id_from_group(PACK_CUSTOM, 1) + 3;
    for (uint16_t off : tiles) {
        tile2i t((int)off);
        if (!t.valid()) {
            continue;
        }
        vec2i coords = g_camera.lookup_tile_to_pixel(t);
        build_planner::draw_building_ghost(ctx, img, coords, mask);
    }
}

} // namespace

static int __city_hover_building_id() {
    building *b = resolve_hover_or_info_building();
    return b ? b->id : 0;
}
ANK_FUNCTION(__city_hover_building_id)

static bvariant_map __building_recorded_paths(int bid) {
    bvariant_map m;
    building *b = building_get(bid);
    if (!b || !b->is_valid()) {
        return m;
    }
    building *main = b->main();
    const int mid = main ? main->id : bid;
    // Expose as path0..path2 (newest first); 0 = empty.
    for (int i = 0; i < BUILDING_RECORDED_PATHS; i++) {
        bstring32 key;
        key.printf("path%d", i);
        m[key.c_str()] = (int32_t)g_recorded_paths.building_path_at(mid, i);
    }
    m["building"] = (int32_t)mid;
    return m;
}
ANK_FUNCTION_1(__building_recorded_paths)

static void __recorded_path_draw(int path_id, color mask) {
    draw_recorded_path(path_id, mask);
}
ANK_FUNCTION_2(__recorded_path_draw)

static int __keyboard_modifiers() {
    return (int)keyboard_t::modifiers();
}
ANK_FUNCTION(__keyboard_modifiers)
