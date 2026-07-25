#pragma once

#include "building/building.h"
#include "graphics/color.h"
#include "graphics/view/view.h"

class building_ship_bridge : public building_impl {
public:
    BUILDING_METAINFO(BUILDING_UNUSED_SHIP_BRIDGE_83, building_ship_bridge, building_impl)

    struct static_params : public building_static_params {
        uint8_t max_length = 0; // 0 → engine default (40)
        uint8_t min_length = 0; // 0 → engine default (5)
    } BUILDING_STATIC_DATA_T;
};
ANK_CONFIG_STRUCT(building_ship_bridge::static_params, max_length, min_length)

// C3 ship-bridge ornament frames (sprite ids 7–15). Unused in Pharaoh menus.
void city_draw_ship_bridge_tile(painter &ctx, int x, int y, int bridge_sprite_id, color color_mask);
