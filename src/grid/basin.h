#pragma once

#include "grid/point.h"

#include <cstdint>

// Enhanced flood-basin detect. Flag gameplay_enhanced_flood_basins.
// basin_id 0 = none / unsealed / flag OFF.

uint16_t map_basin_id_at(int grid_offset);
inline uint16_t map_basin_id_at(tile2i tile) {
    return map_basin_id_at(tile.grid_offset());
}

bool map_basin_is_sealed_at(int grid_offset);
inline bool map_basin_is_sealed_at(tile2i tile) {
    return map_basin_is_sealed_at(tile.grid_offset());
}

int map_basin_count();
int map_basin_area(uint16_t basin_id);
int map_basin_farm_count(uint16_t basin_id);

// First sealed basin among 8 neighbors (dike crest / corner tiles).
uint16_t map_basin_adjacent_id(tile2i tile);

void map_basin_rebuild();
void map_basin_mark_dirty();
void map_basin_rebuild_dirty();
int map_basin_breach_perimeter(int max_tiles, int *first_breach_offset = nullptr);
