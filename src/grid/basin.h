#pragma once

#include "grid/point.h"

#include <cstdint>

// Enhanced flood-basin detect (FB1). Flag gameplay_enhanced_flood_basins.
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

// Full rebuild of basin_id grid from TERRAIN_DIKE contours.
void map_basin_rebuild();

// Mark that DIKE topology may have changed. Rebuild is deferred until
// map_basin_rebuild_dirty() (no-op if not dirty).
void map_basin_mark_dirty();

// Rebuild only if map_basin_mark_dirty() was called since the last rebuild.
void map_basin_rebuild_dirty();

// DK4: remove up to max_tiles DIKE crests that border a sealed basin (Osiris
// destructive flood). Leaves ROAD on sluice tiles. Returns tiles removed;
// first_breach_offset (optional) gets the first removed tile for Go-to-Problem.
int map_basin_breach_perimeter(int max_tiles, int *first_breach_offset = nullptr);
