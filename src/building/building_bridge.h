#pragma once

#include "building/building.h"
#include "graphics/color.h"
#include "graphics/view/view.h"

#include <stdint.h>

class building_low_bridge : public building_impl {
public:
    BUILDING_METAINFO(BUILDING_LOW_BRIDGE, building_low_bridge, building_impl)

    struct static_params : public building_static_params {
        uint8_t max_length = 0; // 0 → engine default (40)
        uint8_t min_length = 0; // 0 → engine default (2)
    } BUILDING_STATIC_DATA_T;
};
ANK_CONFIG_STRUCT(building_low_bridge::static_params, max_length, min_length)

void city_draw_bridge(vec2i pixel, tile2i point, painter &ctx);
// Low-bridge frames (part 1–6). Offsets: bridge_styles[].parts in bridge.js.
void city_draw_bridge_tile(painter &ctx, int x, int y, int bridge_sprite_id, color color_mask, uint16_t bridge_type = 0);

int bridge_span_max_length(bool is_ship_bridge);
int bridge_span_min_length(bool is_ship_bridge);
