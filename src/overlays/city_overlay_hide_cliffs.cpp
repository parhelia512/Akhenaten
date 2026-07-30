#include "city_overlay_hide_cliffs.h"

#include "building/building.h"
#include "figure/figure.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "grid/building.h"
#include "grid/image.h"
#include "grid/property.h"
#include "grid/random.h"
#include "grid/terrain.h"

city_overlay_hide_cliffs g_city_overlay_hide_cliffs;

bool city_overlay_hide_cliffs::show_figure(const figure *f) const {
    return true;
}

bool city_overlay_hide_cliffs::show_building(const building *b) const {
    // Keep all buildings visible; this overlay only flattens bare cliff / ramp graphics.
    return true;
}

bool city_overlay_hide_cliffs::draw_custom_footprint(vec2i pixel, tile2i point, painter &ctx) const {
    int grid_offset = point.grid_offset();
    color color_mask = map_property_is_deleted(grid_offset) ? COLOR_MASK_RED : 0;

    // OG Hide Cliffs: replace bare elevation / access-ramp tiles with flat empty land
    // so cliffside tombs and construction stay readable. Leave building tiles alone;
    // they still draw through the normal overlay footprint path.
    const int cliff_bits = TERRAIN_ELEVATION | TERRAIN_ACCESS_RAMP;
    if (map_terrain_is(grid_offset, cliff_bits) && !map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        if (map_property_is_draw_tile(grid_offset)) {
            auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
            command.image_id = image_id_from_group(GROUP_TERRAIN_EMPTY_LAND) + (map_random_get(grid_offset) & 7);
            command.pixel = pixel;
            command.mask = color_mask;
            command.location = SOURCE_LOCATION;
        }
        return true;
    }

    return false;
}

int city_overlay_hide_cliffs::get_column_height(const building *b) const {
    return COLUMN_TYPE_NONE;
}
