#pragma once

#include "core/buffer.h"
#include "core/tokenum.h"
#include "core/vec2i.h"
#include "grid/point.h"

#include <stdint.h>

enum e_bridge_part {
    BRIDGE_PART_NONE = 0,
    BRIDGE_PART_RAMP_DIR_0 = 1,
    BRIDGE_PART_RAMP_DIR_2 = 2,
    BRIDGE_PART_RAMP_DIR_4 = 3,
    BRIDGE_PART_RAMP_DIR_6 = 4,
    BRIDGE_PART_MID_NE_SW = 5,
    BRIDGE_PART_MID_NW_SE = 6,
    BRIDGE_PART_MAX
};

using e_bridge_part_tokens_t = token_holder<e_bridge_part, BRIDGE_PART_NONE, BRIDGE_PART_MAX>;

int map_bridge_part_at(int grid_offset);
void map_bridge_part_set(int grid_offset, int value);
inline int map_bridge_part_at(tile2i tile) { return map_bridge_part_at(tile.grid_offset()); }
inline void map_bridge_part_set(tile2i tile, int value) { map_bridge_part_set(tile.grid_offset(), value); }

uint16_t map_bridge_type_at(int grid_offset);
void map_bridge_type_set(int grid_offset, uint16_t value);
inline uint16_t map_bridge_type_at(tile2i tile) { return map_bridge_type_at(tile.grid_offset()); }
inline void map_bridge_type_set(tile2i tile, uint16_t value) { map_bridge_type_set(tile.grid_offset(), value); }

void map_bridge_tile_clear(int grid_offset);
void map_bridge_grids_clear();

void map_bridge_grids_backup();
void map_bridge_grids_restore();

// After loading original / old saves: WATER + sprite 1..15 → bridge_part, type 0.
void map_bridge_migrate_from_sprite();

// Resolve bridge_styles[].type text alias → numeric index (0 if unknown).
uint16_t bridge_style_index(pcstr type);
// First image id for bridge_styles entry with matching index; falls back to GROUP_BUILDING_BRIDGE.
int bridge_style_image_base(uint16_t type);
// Look up parts[] draw: img offset from base + pixel pos. Falls back to C3 defaults.
bool bridge_style_part_draw(uint16_t type, int part, int &img_offset, vec2i &pos);
