#include "city_overlay_irrigation.h"

#include "building/building.h"
#include "building/building_type.h"
#include "figure/figure.h"
#include "graphics/color.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "grid/building.h"
#include "grid/image.h"
#include "grid/irrigation_value.h"
#include "grid/point.h"
#include "grid/property.h"
#include "grid/terrain.h"
#include "js/js_game.h"
#include "core/profiler.h"

#include <algorithm>

int __irrigation_value_at_tile(int x, int y) {
    return g_irrigation_value.get(tile2i(x, y).grid_offset());
}
ANK_FUNCTION_2(__irrigation_value_at_tile)

static int terrain_on_irrigation_overlay() {
    return TERRAIN_TREE | TERRAIN_ROCK | TERRAIN_WATER | TERRAIN_ROAD | TERRAIN_ELEVATION
        | TERRAIN_ACCESS_RAMP | TERRAIN_RUBBLE | TERRAIN_WALL | TERRAIN_DIKE;
}

static int get_irrigation_image_offset(int value) {
    if (value <= 0) {
        return 0;
    }
    return std::min(value / 10, 9);
}

city_overlay_irrigation g_city_overlay_irrigation;

bool city_overlay_irrigation::show_figure(const figure *f) const {
    return false;
}

void city_overlay_irrigation::draw_custom_top(vec2i pixel, tile2i point, painter &ctx) const {
    int grid_offset = point.grid_offset();
    color color_mask = 0;

    if (map_terrain_is(grid_offset, terrain_on_irrigation_overlay()) && !map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = map_image_at(grid_offset);
        command.pixel = pixel;
        command.mask = color_mask;
        command.location = SOURCE_LOCATION;
        return;
    }

    if (map_terrain_is(grid_offset, TERRAIN_CANAL) || map_terrain_is(grid_offset, TERRAIN_FLOODPLAIN)
        || map_terrain_is(grid_offset, TERRAIN_BUILDING) || g_irrigation_value.get(grid_offset) > 0) {
        int offset = get_irrigation_image_offset(g_irrigation_value.get(grid_offset));
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = image_id_from_group(GROUP_TERRAIN_DESIRABILITY) + offset;
        command.pixel = pixel;
        command.mask = color_mask;
        command.location = SOURCE_LOCATION;
        return;
    }

    auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
    command.image_id = map_image_at(grid_offset);
    command.pixel = pixel;
    command.mask = color_mask;
    command.location = SOURCE_LOCATION;
}

int city_overlay_irrigation::get_column_height(const building *b) const {
    return COLUMN_TYPE_NONE;
}

bool city_overlay_irrigation::show_building(const building *b) const {
    // Tile-color overlay like fertility: keep structures flattened into the irrigation map.
    return false;
}
