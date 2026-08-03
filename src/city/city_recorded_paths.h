#pragma once

#include "building/building.h"
#include "core/hvector.h"
#include "grid/point.h"

#include <cstdint>

constexpr int RECORDED_PATH_POOL_SIZE = 512;
constexpr int RECORDED_PATH_MAX_TILES = 256;
constexpr int BUILDING_RECORDED_PATHS = 3;

using recorded_path_tiles_t = hvector<uint16_t, RECORDED_PATH_MAX_TILES>;

struct recorded_paths_t {
    struct slot_t {
        bool used = false;
        recorded_path_tiles_t tiles;
    };

    slot_t slots[RECORDED_PATH_POOL_SIZE];
    // rings[bid][0] = newest; 0 = empty id
    uint16_t building_rings[MAX_BUILDINGS][BUILDING_RECORDED_PATHS] = {};

    void clear();
    int acquire();
    void release(int id);
    void clear_tiles(int id);
    void append(int id, int grid_offset);
    const recorded_path_tiles_t &tiles(int id) const;

    void building_push(building_id bid, int path_id);
    int building_path_at(building_id bid, int index) const; // 0 = newest
    void building_clear(building_id bid);

    // Transfer figure's trail to building; figure gets a fresh empty trail if still alive.
    void handoff_to_building(figure &f, building_id bid);
};

extern recorded_paths_t g_recorded_paths;

void figure_recorded_path_acquire(figure &f);
void figure_recorded_path_append_tile(figure &f);
void figure_recorded_path_release(figure &f);
