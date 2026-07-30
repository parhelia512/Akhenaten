#include "city_overlay_flood_basin.h"

#include "building/building.h"
#include "figure/figure.h"
#include "game/game_config.h"
#include "graphics/color.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "grid/basin.h"
#include "grid/building.h"
#include "grid/image.h"
#include "grid/property.h"
#include "grid/terrain.h"

city_overlay_flood_basin g_city_overlay_flood_basin;

static int terrain_keep_normal() {
    return TERRAIN_TREE | TERRAIN_ROCK | TERRAIN_WATER | TERRAIN_ROAD | TERRAIN_ELEVATION
        | TERRAIN_ACCESS_RAMP | TERRAIN_RUBBLE | TERRAIN_WALL | TERRAIN_CANAL;
}

bool city_overlay_flood_basin::show_figure(const figure *f) const {
    return false;
}

void city_overlay_flood_basin::draw_custom_top(vec2i pixel, tile2i point, painter &ctx) const {
    const int grid_offset = point.grid_offset();
    color color_mask = 0;

    if (!game_features::gameplay_enhanced_flood_basins.to_bool()) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = map_image_at(grid_offset);
        command.pixel = pixel;
        command.mask = color_mask;
        command.location = SOURCE_LOCATION;
        return;
    }

    // Crest: amber tint so the embankment reads against sealed interiors.
    if (map_terrain_is(grid_offset, TERRAIN_DIKE)) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = map_image_at(grid_offset);
        command.pixel = pixel;
        command.mask = COLOR_MASK_AMBER_40;
        command.location = SOURCE_LOCATION;
        return;
    }

    if (map_terrain_is(grid_offset, terrain_keep_normal()) && !map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = map_image_at(grid_offset);
        command.pixel = pixel;
        command.mask = color_mask;
        command.location = SOURCE_LOCATION;
        return;
    }

    const uint16_t basin_id = map_basin_id_at(grid_offset);
    if (basin_id != 0 || map_terrain_is(grid_offset, TERRAIN_FLOODPLAIN) || map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        // Sealed interior: soft green desirability band. Open FP / buildings: flat empty tint.
        int offset = 0;
        if (basin_id != 0) {
            offset = 3 + (int)(basin_id % 4); // distinct soft bands per basin
        }
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

int city_overlay_flood_basin::get_column_height(const building *b) const {
    return COLUMN_TYPE_NONE;
}

bool city_overlay_flood_basin::show_building(const building *b) const {
    return false;
}
