#include "city/city_recorded_paths.h"

#include "building/building.h"
#include "building/construction/build_planner.h"
#include "city/city_figures.h"
#include "core/bstring.h"
#include "core/calc.h"
#include "core/profiler.h"
#include "core/variant.h"
#include "figure/figure.h"
#include "game/game.h"
#include "graphics/color.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "graphics/view/view.h"
#include "js/js_game.h"

static void draw_recorded_path_tiles(painter &ctx, const recorded_path_tiles_t &tiles, color mask, int base_img) {
    if (tiles.empty()) {
        return;
    }

    tile2i start((int)tiles[0]);
    if (start.valid()) {
        vec2i pixel = g_camera.lookup_tile_to_pixel(start);
        build_planner::draw_building_ghost(ctx, base_img + 3, pixel, mask);
    }

    for (size_t t = 1; t < tiles.size(); t++) {
        tile2i from((int)tiles[t - 1]);
        tile2i to((int)tiles[t]);
        if (!from.valid() || !to.valid()) {
            continue;
        }
        int img_index = 10;
        switch (calc_general_direction(from, to)) {
        case DIR_0_TOP_RIGHT:
        case DIR_4_BOTTOM_LEFT:
            img_index = 0;
            break;
        case DIR_2_BOTTOM_RIGHT:
        case DIR_6_TOP_LEFT:
            img_index = 1;
            break;
        default:
            break;
        }
        vec2i pixel = g_camera.lookup_tile_to_pixel(to);
        build_planner::draw_building_ghost(ctx, base_img + img_index, pixel, mask);
    }
}

void building_draw_usable_paths(int bid) {
    building *b = building_get(bid);
    if (!b || !b->is_valid()) {
        return;
    }
    building *main = b->main();
    const int mid = main ? main->id : bid;

    static const color path_colors[BUILDING_RECORDED_PATHS] = {
        COLOR_MASK_AMBER,
        COLOR_MASK_GREEN,
        COLOR_MASK_BLUE,
        COLOR_MASK_RED,
    };
    const int base_img = image_id_from_group(PACK_CUSTOM, 1);
    painter ctx = game.painter();

    for (int i = 0; i < BUILDING_RECORDED_PATHS; i++) {
        const int path_id = g_recorded_paths.building_path_at(mid, i);
        if (!path_id) {
            continue;
        }
        draw_recorded_path_tiles(ctx, g_recorded_paths.tiles(path_id), path_colors[i], base_img);
    }

    for (figure *f : map_figures()) {
        if (!f || f->is_dead() || f->trail_path_id <= 0) {
            continue;
        }
        bool linked = false;
        building *dest = f->destination();
        if (dest && dest->is_valid()) {
            building *dest_main = dest->main();
            linked = dest_main && dest_main->id == mid;
        }
        if (!linked) {
            building *home = f->home();
            if (home && home->is_valid()) {
                building *home_main = home->main();
                linked = home_main && home_main->id == mid;
            }
        }
        if (!linked) {
            continue;
        }
        draw_recorded_path_tiles(ctx, g_recorded_paths.tiles(f->trail_path_id), COLOR_MASK_NONE, base_img);
    }
}

static void __building_draw_usable_paths(int bid) {
    building_draw_usable_paths(bid);
}
ANK_FUNCTION_1(__building_draw_usable_paths)
