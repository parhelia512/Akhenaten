#pragma once

#include "grid/point.h"
#include "grid/routing/routing.h"
#include "building/building_type.h"

struct routed_building_result {
    bool ok;
    int items;
};

routed_building_result place_routed_building(tile2i start, tile2i end, e_routed_mode type,
    e_building_type wall_type = BUILDING_MUD_WALL, bool write_wall_material = true);

int building_construction_place_canal(bool measure_only, tile2i start, tile2i end);
