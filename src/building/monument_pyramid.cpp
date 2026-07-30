#include "monument_pyramid.h"

#include "graphics/view/view.h"
#include "monuments.h"
#include "core/direction.h"
#include "core/custom_span.hpp"
#include "graphics/image.h"
#include "widget/city/tile_draw.h"
#include "widget/city/flat_draw.h"
#include "window/building/common.h"
#include "city/city_warnings.h"
#include "city/city_buildings.h"
#include "city/city_figures.h"
#include "figure/figure.h"
#include "game/game.h"
#include "game/undo.h"
#include "city/city_resource.h"
#include "city/city_message.h"
#include "grid/random.h"
#include "grid/tiles.h"
#include "grid/grid.h"
#include "grid/terrain.h"
#include "grid/building.h"
#include "grid/property.h"
#include "grid/image.h"
#include "grid/building_tiles.h"
#include "graphics/view/lookup.h"
#include "graphics/graphics.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/lang_text.h"
#include "figuretype/figure_worker.h"
#include "construction/build_planner.h"
#include "widget/widget_city.h"
#include "grid/routing/routing_grids.h"
#include "dev/debug.h"
#include "js/js_game.h"
#include "grid/canals.h"
#include "building/monument_mastaba.h"
#include "graphics/image_groups.h"

#include <numeric>
#include <string>

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_stepped_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_stepped_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_stepped_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_stepped_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_stepped_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_stepped_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_large_stepped_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_large_stepped_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_large_stepped_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_stepped_pyramid_complex)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_bent_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_bent_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_bent_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_bent_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_bent_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_bent_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_small_pyramid_wall)

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_pyramid)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_pyramid_corner)
REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_medium_pyramid_wall)

struct pyramid_part {
    e_building_type type;
    tile2i offset;
    building *b;
};

struct monument_small_stepped_pyramid : public monument {
    monument_small_stepped_pyramid() : monument{ BUILDING_SMALL_STEPPED_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600} });
        phases.push_back({ 7, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 8, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 9, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({10, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({11, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({12, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({13, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({14, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({15, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({16, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({17, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({18, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({19, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({20, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({21, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({22, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({23, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({24, monument_phase_resource{RESOURCE_NONE, 0} });
    }
} g_monument_small_stepped_pyramid;

struct monument_medium_stepped_pyramid : public monument {
    monument_medium_stepped_pyramid() : monument{ BUILDING_MEDIUM_STEPPED_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600} });
        phases.push_back({ 7, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 8, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 9, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 10, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 11, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 12, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 13, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 14, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 15, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 16, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 17, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 18, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 19, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 20, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 21, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 22, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 23, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 24, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 25, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 26, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 27, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 28, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 29, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 30, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 31, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        phases.push_back({ 32, monument_phase_resource{RESOURCE_NONE, 0} });
    }
} g_monument_medium_stepped_pyramid;

// Large stepped pyramid (20×20). Schedule: foundation 0–6, then brick courses → finish@36.
// Also used by the stepped pyramid complex until causeway/temples change costs.
// Top-down marble polish is NOT stepped — true pyramids only.
struct monument_large_stepped_pyramid : public monument {
    monument_large_stepped_pyramid() : monument{ BUILDING_LARGE_STEPPED_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600} });
        // 20×20 = 5 layers × 6 brick courses after phase 6 → finish at 36
        // (medium still finishes at 32; shared height draw handles both).
        for (int p = 7; p <= 35; ++p) {
            phases.push_back({ (uint8_t)p, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        }
        phases.push_back({ 36, monument_phase_resource{RESOURCE_NONE, 0} });
    }
} g_monument_large_stepped_pyramid;

// Bent pyramids share the stepped construction/rendering pipeline, so their phase
// schedules mirror the stepped ones (the phase count drives the layer rendering:
// small = 24 phases over an 8x8 footprint, medium = 32 phases over 12x12).
struct monument_small_bent_pyramid : public monument {
    monument_small_bent_pyramid() : monument{ BUILDING_SMALL_BENT_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600} });
        for (int p = 7; p <= 23; ++p) {
            phases.push_back({ (uint8_t)p, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        }
        phases.push_back({ 24, monument_phase_resource{RESOURCE_NONE, 0} });
    }
} g_monument_small_bent_pyramid;

struct monument_medium_bent_pyramid : public monument {
    monument_medium_bent_pyramid() : monument{ BUILDING_MEDIUM_BENT_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600} });
        for (int p = 7; p <= 31; ++p) {
            phases.push_back({ (uint8_t)p, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800} });
        }
        phases.push_back({ 32, monument_phase_resource{RESOURCE_NONE, 0} });
    }
} g_monument_medium_bent_pyramid;


struct monument_small_pyramid : public monument {
    monument_small_pyramid() : monument{ BUILDING_SMALL_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800}, {RESOURCE_LIMESTONE, 1200} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000}, {RESOURCE_LIMESTONE, 1000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200}, {RESOURCE_LIMESTONE, 800} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400}, {RESOURCE_LIMESTONE, 600} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600}, {RESOURCE_LIMESTONE, 400} });
        for (int p = 7; p <= 23; ++p) {
            phases.push_back({ (uint8_t)p, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800}, {RESOURCE_LIMESTONE, 200} });
        }
        // Polish: no new limestone; stonemasons via need_stonemason().
        // Count = init_tiles/4 layers (8→2). Phases 24–25 collide with stepped
        // hight_impl large band — building_small_pyramid draws completed height instead.
        constexpr int k_polish_phases = 8 / 4; // == num layers
        for (int i = 0; i < k_polish_phases; ++i) {
            phases.push_back({ (uint8_t)(24 + i), monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        }
        phases.push_back({ 26, monument_phase_resource{RESOURCE_NONE, 0} }); // last id; finish = set_phase(27)
    }
} g_monument_small_pyramid;

// True medium (12×12): height mirrors medium stepped (courses through 31), then
// polish top-down (layers = 12/4 = 3) without new limestone; terminal 35.
struct monument_medium_pyramid : public monument {
    monument_medium_pyramid() : monument{ BUILDING_MEDIUM_PYRAMID } {
        phases.push_back({ 0, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 1, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        phases.push_back({ 2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_STONE, 4800}, {RESOURCE_LIMESTONE, 1200} });
        phases.push_back({ 3, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 2000}, {RESOURCE_STONE, 4000}, {RESOURCE_LIMESTONE, 1000} });
        phases.push_back({ 4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1600}, {RESOURCE_STONE, 3200}, {RESOURCE_LIMESTONE, 800} });
        phases.push_back({ 5, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 1200}, {RESOURCE_STONE, 2400}, {RESOURCE_LIMESTONE, 600} });
        phases.push_back({ 6, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 800}, {RESOURCE_STONE, 1600}, {RESOURCE_LIMESTONE, 400} });
        for (int p = 7; p <= 31; ++p) {
            phases.push_back({ (uint8_t)p, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, 400}, {RESOURCE_STONE, 800}, {RESOURCE_LIMESTONE, 200} });
        }
        // Polish after height — phase numbers stay outside small's 24–25 band and
        // after medium height courses; draw uses completed ornaments for phase >= 32.
        constexpr int k_polish_phases = 12 / 4;
        for (int i = 0; i < k_polish_phases; ++i) {
            phases.push_back({ (uint8_t)(32 + i), monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_NONE, 0} });
        }
        phases.push_back({ 35, monument_phase_resource{RESOURCE_NONE, 0} }); // finish = set_phase(36)
    }
} g_monument_medium_pyramid;

template<typename T>
const building_pyramid::base_params &pyramid_base_params(const building_static_params &params) {
    using static_params = typename T::static_params;
    const auto &bparams = (const static_params &)params;
    return (const building_pyramid::base_params &)bparams;
}

tile2i building_pyramid::footprint_nw() const {
    // Orientation can put a corner/wall at the linked-list head — geometric NW is
    // the min (x,y) across the multipart chain, not main()->tile().
    int min_x = tile().x();
    int min_y = tile().y();
    for (const building *part = &main()->base; part; part = part->has_next() ? part->next() : nullptr) {
        min_x = std::min(min_x, part->tile.x());
        min_y = std::min(min_y, part->tile.y());
    }
    return tile2i(min_x, min_y);
}

tile2i building_pyramid::center_point() const {
    const auto &itiles = pyramid_params().init_tiles;
    tile2i tnw = footprint_nw();
    tile2i tend = tnw.shifted(itiles.x, itiles.y);
    tile2i ctile = tnw.add(tend).div(2);
    building* b = building_at(ctile);
    // Mid-footprint must be a pyramid part; fall back to NW tile if the map
    // was corrupted / mid-demolition so sleds don't null-deref.
    return b ? b->tile : tnw;
}

grid_area building_pyramid::get_area() const {
    // Sleds deliver when the load is inside this rect. Part buildings are 2×2;
    // using size()-1 made the area miss center_point() and drop cargo undelivered.
    const auto &itiles = pyramid_params().init_tiles;
    tile2i begin = footprint_nw();
    tile2i end = begin.shifted(itiles.x - 1, itiles.y - 1);
    return { begin, end };
}

bool building_pyramid::get_route_citizen_land_type(int grid_offset, int &land_result) const {
    if (is_finished()) {
        land_result = CITIZEN_N1_BLOCKED;
        return true;
    }

    if (phase() > 6) {
        const auto &itiles = pyramid_params().init_tiles;
        tile2i maint = footprint_nw();
        tile2i end = maint.shifted(itiles.x, itiles.y);
        tile2i tile(grid_offset);
        land_result = (tile.x() == maint.x() || tile.y() == maint.y() || tile.x() == end.x()) ? CITIZEN_N1_BLOCKED : CITIZEN_2_PASSABLE_TERRAIN;
        return true;
    }

    land_result = CITIZEN_2_PASSABLE_TERRAIN;
    return true;
}

bool building_pyramid::target_route_tile_blocked(int grid_offset) const {
    return is_finished();
}

const building_pyramid::base_params &get_pyramid_params(e_building_type type) {
    const auto &params = building_static_params::get(type);

    switch (params.type) {
    case BUILDING_SMALL_STEPPED_PYRAMID: 
    case BUILDING_SMALL_STEPPED_PYRAMID_CORNER:     
    case BUILDING_SMALL_STEPPED_PYRAMID_CONE: 
    case BUILDING_SMALL_STEPPED_PYRAMID_WALL: 
        return pyramid_base_params<building_small_stepped_pyramid>(params);

    case BUILDING_MEDIUM_STEPPED_PYRAMID:
    case BUILDING_MEDIUM_STEPPED_PYRAMID_CORNER:
    case BUILDING_MEDIUM_STEPPED_PYRAMID_CONE:
    case BUILDING_MEDIUM_STEPPED_PYRAMID_WALL:
        return pyramid_base_params<building_medium_stepped_pyramid>(params);

    case BUILDING_LARGE_STEPPED_PYRAMID:
    case BUILDING_LARGE_STEPPED_PYRAMID_CORNER:
    case BUILDING_LARGE_STEPPED_PYRAMID_CONE:
    case BUILDING_LARGE_STEPPED_PYRAMID_WALL:
        return pyramid_base_params<building_large_stepped_pyramid>(params);

    case BUILDING_STEPPED_PYRAMID_COMPLEX:
        return pyramid_base_params<building_stepped_pyramid_complex>(params);

    case BUILDING_SMALL_BENT_PYRAMID:
    case BUILDING_SMALL_BENT_PYRAMID_CORNER:
    case BUILDING_SMALL_BENT_PYRAMID_CONE:
    case BUILDING_SMALL_BENT_PYRAMID_WALL:
        return pyramid_base_params<building_small_bent_pyramid>(params);

    case BUILDING_MEDIUM_BENT_PYRAMID:
    case BUILDING_MEDIUM_BENT_PYRAMID_CORNER:
    case BUILDING_MEDIUM_BENT_PYRAMID_CONE:
    case BUILDING_MEDIUM_BENT_PYRAMID_WALL:
        return pyramid_base_params<building_medium_bent_pyramid>(params);

    case BUILDING_SMALL_PYRAMID:
    case BUILDING_SMALL_PYRAMID_CORNER:
    case BUILDING_SMALL_PYRAMID_CONE:
    case BUILDING_SMALL_PYRAMID_WALL:
        return pyramid_base_params<building_small_pyramid>(params);

    case BUILDING_MEDIUM_PYRAMID:
    case BUILDING_MEDIUM_PYRAMID_CORNER:
    case BUILDING_MEDIUM_PYRAMID_CONE:
    case BUILDING_MEDIUM_PYRAMID_WALL:
        return pyramid_base_params<building_medium_pyramid>(params);
    }

    static building_pyramid::base_params dummy;
    return dummy;
};

void building_stepped_pyramid::set_tile_progress(tile2i tile, int v) {
    building_pyramid::set_tile_progress(tile, v);
    if (phase() == 2) {
        map_terrain_add(tile, v > 0 ? TERRAIN_CANAL : TERRAIN_NONE);
        return;
    } 

    if (phase() >= 3 && phase() < 7) {
        map_terrain_add(tile, TERRAIN_CANAL);
        return;
    }
}

void building_stepped_pyramid::preview::setup_preview_graphics(build_planner &planer) const {
    const auto &params = building_static_params::get(planer.build_type);
    const auto &base_params = get_pyramid_params(planer.build_type);

    const vec2i init_tiles = base_params.init_tiles;

    switch (g_camera.orientation / 2) {
    case 0: planer.init_tiles(init_tiles.y, init_tiles.x); break;
    case 1: planer.init_tiles(init_tiles.x, init_tiles.y); break;
    case 2: planer.init_tiles(init_tiles.y, init_tiles.x); break;
    case 3: planer.init_tiles(init_tiles.x, init_tiles.y); break;
    }
}

void building_stepped_pyramid::preview::ghost_preview(build_planner &planer, painter &ctx, tile2i start, tile2i end, vec2i pixel) const {
    const auto &params = building_static_params::get(planer.build_type);
    const auto &base_params = get_pyramid_params(planer.build_type);

    int image_id = params.base_img();
    auto get_image = [image_id] (tile2i tile, tile2i start, vec2i size) {
        if (tile == start) {
            return image_id;
        }

        if (tile == start.shifted(size.x - 1, 0)) {
            return image_id - 2;
        }

        if (tile == start.shifted(size.x - 1, size.y - 1)) {
            return image_id - 4;
        }

        if (tile == start.shifted(0, size.y - 1)) {
            return image_id - 6;
        }

        if (tile.y() == start.y()) { return image_id - 1; }
        if (tile.y() == start.y() + size.y - 1) { return image_id - 5; }
        if (tile.x() == start.x()) { return image_id - 7; }
        if (tile.x() == start.x() + size.x - 1) { return image_id - 3; }

        return (image_id + 5 + (tile.x() + tile.y()) % 7);
    };

    vec2i size{ 1, 1 };
    vec2i size_b = base_params.init_tiles;
    switch (g_camera.orientation / 2) {
    case 0: size = { size_b.x, size_b.y }; break;
    case 1: size = { size_b.y, size_b.x }; break;
    case 2: size = { size_b.x, size_b.y }; break;
    case 3: size = { size_b.y, size_b.x }; break;
    }

    for (int i = 0; i < size.x; ++i) {
        for (int j = 0; j < size.y; ++j) {
            vec2i p = pixel + (vec2i(-30, 15) * i) + (vec2i(30, 15) * j);
            int image_id = get_image(end.shifted(i, j), end, size);

            auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile_full);
            command.image_id = image_id;
            command.pixel = p;
            command.mask = COLOR_MASK_GREEN;
        }
    }
}

void map_pyramid_tiles_add(int building_id, tile2i tile, int size, int image_id, int terrain) {
    int x_leftmost, y_leftmost;
    switch (g_camera.orientation) {
    case DIR_0_TOP_RIGHT: x_leftmost = 0; y_leftmost = 1; break;
    case DIR_2_BOTTOM_RIGHT: x_leftmost = y_leftmost = 0; break;
    case DIR_4_BOTTOM_LEFT: x_leftmost = 1; y_leftmost = 0; break;
    case DIR_6_TOP_LEFT: x_leftmost = y_leftmost = 1; break;
    default:
        return;
    }

    if (!map_grid_is_inside(tile, size)) {
        return;
    }

    int x_proper = x_leftmost * (size - 1);
    int y_proper = y_leftmost * (size - 1);
    for (int dy = 0; dy < size; dy++) {
        for (int dx = 0; dx < size; dx++) {
            int grid_offset = tile.shifted(dx, dy).grid_offset();
            map_terrain_remove(grid_offset, TERRAIN_CLEARABLE);
            map_terrain_add(grid_offset, terrain);
            map_building_set(grid_offset, building_id);
            map_property_clear_constructing(grid_offset);
            map_property_set_multi_tile_size(grid_offset, size);
            map_monuments_set_progress(tile2i(grid_offset), 0);
            if (image_id >= 0) {
                map_image_set(grid_offset, image_id);
            }
            map_property_set_multi_tile_xy(grid_offset, dx, dy, dx == x_proper && dy == y_proper);
        }
    }
}

bool building_stepped_pyramid::need_workers() const {
    if (!is_main()) {
        return false;
    }

    const auto &w = runtime_data().workers;
    return std::find(w.begin(), w.end(), 0) != w.end();
}

void building_stepped_pyramid::finalize(building *b, const vec2i size_b) {
    building *part = b;
    building *main = b->main();

    while (!!part) {
        auto monument = part->dcast_monument();
        monument->runtime_data().phase = MONUMENT_FINISHED;
        part = part->has_next() ? part->next() : nullptr;
    }
}

void building_stepped_pyramid::remove_worker(figure_id fid) {
    // Worker list lives on the chain head (guilds / work camps always target
    // is_main, but parts may still receive remove_worker via destination_bid).
    if (!is_main()) {
        main()->remove_worker(fid);
        return;
    }
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == fid) {
            wid = 0;
            return;
        }
    }
}

void building_stepped_pyramid::add_workers(figure_id fid) {
    if (!is_main()) {
        main()->add_workers(fid);
        return;
    }
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == 0) {
            wid = fid;
            return;
        }
    }
}

int building_stepped_pyramid::get_image(const building_static_params& params, int orientation, tile2i tile, tile2i start, tile2i end) {
    int image_id = params.base_img();
    int base_image_id = image_id - 7;
    bool insidex = (tile.x() > start.x() && tile.x() < end.x());
    bool insidey = (tile.y() > start.y() && tile.y() < end.y());
    int random = params.first_img(animkeys().ground_phase_0) + ((tile.x() + tile.y()) % 7);
    int result = random;
    if (tile == start) { // top corner
        result = image_id;
    } else if (tile == tile2i(start.x(), end.y())) {
        result = image_id - 2;
    } else if (tile == end) {
        result = image_id - 4;
    } else if (tile == tile2i(end.x(), start.y())) {
        result = image_id - 6;
    } else if (tile.x() == start.x()) {
        result = image_id - 1;
    } else if (tile.y() == end.y()) {
        result = image_id - 3;
    } else if (tile.y() == start.y()) {
        result = (insidex || insidey) ? image_id - 7 : random;
    } else if (tile.x() == end.x()) {
        result = image_id - 5;
    }

    if (result < random) {
        int offset = result - base_image_id;
        result = (base_image_id + (offset + (8 - g_camera.orientation)) % 8);
        return result;
    }

    return result;
}

int building_stepped_pyramid::get_channel_image(int orientation, tile2i tile, tile2i start, tile2i end, int channel_base_id) {  
    bool north = map_terrain_is(tile.shifted(0, -1), TERRAIN_CANAL);
    bool east  = map_terrain_is(tile.shifted(1, 0), TERRAIN_CANAL);
    bool south = map_terrain_is(tile.shifted(0, 1), TERRAIN_CANAL);
    bool west  = map_terrain_is(tile.shifted(-1, 0), TERRAIN_CANAL);

    int neighbors_count = (north ? 1 : 0) + (east ? 1 : 0) + (south ? 1 : 0) + (west ? 1 : 0);

    // 0: zero tile left-corner 
    // 1-2: lines (vert or horz)
    // 3-4: corners (+ city orientation)
    // 5-7: T-corners (4 directions)
    // 8 - cross

    int variant = 0;
    int orientation_offset = (g_camera.orientation / 2) % 4;

    if (neighbors_count == 0) {
        variant = 0;
    } else if (neighbors_count == 1) {
        variant = 0;
    } else if (neighbors_count == 2) {
        if (north && south) {
            variant = (orientation_offset % 2 == 0) ? 1 : 2;
        } else if (east && west) {
            variant = (orientation_offset % 2 == 0) ? 2 : 1;
        } else {
            // corners
            if (north && east) {
                variant = 0 + (orientation_offset % 4);
            } else if (east && south) {
                variant = 1 + (orientation_offset % 4);
            } else if (south && west) {
                variant = 2 + (orientation_offset % 4);
            } else if (west && north) {
                variant = 3 + (orientation_offset % 4);
            }
        }
    } else if (neighbors_count == 3) {
        // T-corners
        if (!north) {
            variant = 5 + ((0 + orientation_offset) % 4);
        } else if (!east) {
            variant = 5 + ((1 + orientation_offset) % 4);
        } else if (!south) {
            variant = 5 + ((2 + orientation_offset) % 4);
        } else { // !west
            variant = 5 + ((3 + orientation_offset) % 4);
        }
    } else {
        variant = 8;
    }

    return channel_base_id + variant;
}

void building_stepped_pyramid::on_create(int orientation) {
    runtime_data().stair_index = 0xff;
}

void building_stepped_pyramid::assign_stair() {
    tile2i mtile = footprint_nw();
    tile2i btile = tile();
    auto it = std::find_if(pyramid_params().stairs.begin(), pyramid_params().stairs.end(), [&] (auto &it) { return mtile.shifted(it.part) == btile; });
    runtime_data().stair_index = (it != pyramid_params().stairs.end())
                                    ? (uint8_t)std::distance(pyramid_params().stairs.begin(), it)
                                    : 0xff;
}

void building_stepped_pyramid::on_post_load() {
    building_monument::on_post_load();

    assign_stair();
}

void building_stepped_pyramid::on_config_reload() {
    // stairs[] is reloaded from pyramid.js on --mixed / reload_scripts, but each
    // part keeps a cached stair_index from place/load — rebind so part/tex/offset
    // edits apply without restarting the mission.
    assign_stair();
}

bool building_stepped_pyramid::draw_unfinished_height_ornaments(painter &ctx, vec2i point, tile2i tile, color color_mask, const vec2i tiles_size) {
    if (phase() == 6 && runtime_data().alt_image > 0) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = runtime_data().alt_image;
        command.pixel = point;
        command.mask = color_mask;
        command.use_sort_pixel = true;
        command.sort_pixel = point + vec2i(0, 1);
        command.location = SOURCE_LOCATION;
    }

    // Flat view: keep light construction cues only — no tall height_impl tiers.
    if (city_flat_should_flatten_building(base)) {
        return true;
    }

    if (phase() < 6) {
        return false;
    }

    return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, tiles_size);
}

bool building_stepped_pyramid::draw_ornaments_and_animations_flat_impl(painter &ctx, vec2i point, tile2i tile, color color_mask, const vec2i tiles_size) {
    // Use the same implementation as building_mastaba::draw_ornaments_and_animations_flat_impl
    // This can be customized later if pyramid needs different rendering
    if (is_finished()) {
        return false;
    }

    if (is_main()) {
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = first_img(animkeys().enter);
        command.pixel = g_camera.lookup_tile_to_pixel(access_point());
        command.use_sort_pixel = true;
        command.sort_pixel = command.pixel + vec2i(0, 1);
        command.location = SOURCE_LOCATION;
    }

    int clear_land_id = first_img(animkeys().clear_land);
    int image_grounded = first_img(animkeys().base_grounded);
    tile2i fnw = footprint_nw();
    color_mask = (color_mask ? color_mask : 0xffffffff);

    auto &monumentd = base.dcast_monument()->runtime_data();
    if (monumentd.phase == 0) {
        for (int dy = 0; dy < base.size; dy++) {
            for (int dx = 0; dx < base.size; dx++) {
                tile2i ntile = base.tile.shifted(dx, dy);
                vec2i offset = g_camera.lookup_tile_to_pixel(ntile);
                uint32_t progress = map_monuments_get_progress(ntile);

                if (progress > 0 && progress <= 200) {
                    auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                    command.image_id = image_grounded + ((dy * 4 + dx) & 7);
                    command.pixel = offset;
                    command.mask = ((0xff * progress / 200) << COLOR_BITSHIFT_ALPHA) | (color_mask & 0x00ffffff);
                    command.flags = ImgFlag_Alpha;
                    command.use_sort_pixel = true;
                    command.sort_pixel = offset + vec2i(0, 1);
                    command.location = SOURCE_LOCATION;
                }
            }
        }

        int image_stick = current_params().first_img(animkeys().image_stick);
        const image_t *img = image_get(image_stick);
        tile2i left_top = base.tile.shifted(0, 0);
        if (left_top == fnw && map_monuments_get_progress(left_top) == 0) {
            vec2i offset = g_camera.lookup_tile_to_pixel(left_top);
            auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
            command.image_id = image_stick;
            command.pixel = offset;
            command.mask = color_mask;
            command.use_sort_pixel = true;
            command.sort_pixel = offset + vec2i(0, 1);
            command.location = SOURCE_LOCATION;
        }

        tile2i right_top = base.tile.shifted(1, 0);
        if (right_top == fnw.shifted(tiles_size.y - 1, 0) && map_monuments_get_progress(right_top) == 0) {
            vec2i offset = g_camera.lookup_tile_to_pixel(right_top);
            auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
            command.image_id = image_stick;
            command.pixel = offset;
            command.mask = color_mask;
            command.use_sort_pixel = true;
            command.sort_pixel = offset + vec2i(0, 1);
            command.location = SOURCE_LOCATION;
        }

        tile2i left_bottom = base.tile.shifted(0, 1);
        if (left_bottom == fnw.shifted(0, tiles_size.x - 1) && map_monuments_get_progress(left_bottom) == 0) {
            vec2i offset = g_camera.lookup_tile_to_pixel(left_bottom);
            auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
            command.image_id = image_stick;
            command.pixel = offset;
            command.mask = color_mask;
            command.use_sort_pixel = true;
            command.sort_pixel = offset + vec2i(0, 1);
            command.location = SOURCE_LOCATION;
        }

        tile2i right_bottom = base.tile.shifted(1, 1);
        if (right_bottom == fnw.shifted(tiles_size.y - 1, tiles_size.x - 1) && map_monuments_get_progress(right_bottom) == 0) {
            vec2i offset = g_camera.lookup_tile_to_pixel(right_bottom);
            auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
            command.image_id = image_stick;
            command.pixel = offset;
            command.mask = color_mask;
            command.use_sort_pixel = true;
            command.sort_pixel = offset + vec2i(0, 1);
            command.location = SOURCE_LOCATION;
        }
    } else if (monumentd.phase == 1) {
        for (int dy = 0; dy < base.size; dy++) {
            for (int dx = 0; dx < base.size; dx++) {
                tile2i ntile = base.tile.shifted(dx, dy);
                vec2i offset = g_camera.lookup_tile_to_pixel(ntile);
                uint32_t progress = map_monuments_get_progress(ntile);
                if (progress < 200) {
                    auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                    command.image_id = image_grounded + ((dy * 4 + dx) & 7);
                    command.pixel = offset;
                    command.mask = color_mask;
                    command.use_sort_pixel = true;
                    command.sort_pixel = offset + vec2i(0, 1);
                    command.location = SOURCE_LOCATION;
                }

                if (progress > 0 && progress <= 200) {
                    int img = get_image(current_params(), base.orientation, base.tile.shifted(dx, dy), fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1));

                    auto& command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile);
                    command.image_id = img;
                    command.pixel = offset;
                    command.mask = ((0xff * progress / 200) << COLOR_BITSHIFT_ALPHA) | (color_mask & 0x00ffffff);
                    command.flags = ImgFlag_Alpha;
                    command.location = SOURCE_LOCATION;
                }
            }
        }
    } else if (monumentd.phase == 2) {
        const xstring& base_key = animkeys().ditches_phase_1;
        int channel_base_id = current_params().animations[base_key].first_img();
        for (int dy = 0; dy < base.size; dy++) {
            for (int dx = 0; dx < base.size; dx++) {
                tile2i ntile = base.tile.shifted(dx, dy);
                vec2i offset = g_camera.lookup_tile_to_pixel(ntile);
                uint32_t progress = map_monuments_get_progress(ntile);
                if (progress < 200) {
                    int img = get_image(current_params(), base.orientation, base.tile.shifted(dx, dy), fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1));

                    auto& command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                    command.image_id = img;
                    command.pixel = offset;
                    command.mask = color_mask;
                    command.use_sort_pixel = true;
                    command.sort_pixel = offset + vec2i(0, 1);
                    command.location = SOURCE_LOCATION;

                    int channel_img = get_channel_image(base.orientation, ntile, fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1), channel_base_id);
                    if (channel_img > 0 && progress > 0) {

                        auto &channel_command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile);
                        channel_command.image_id = channel_img;
                        channel_command.pixel = offset;

                        const uint8_t alpha = (0xff * progress / 200);
                        channel_command.mask = ((alpha << COLOR_BITSHIFT_ALPHA) | (color_mask & 0x00ffffff));
                        channel_command.flags = (progress < 200) ? ImgFlag_Alpha : 0;
                        channel_command.location = SOURCE_LOCATION;
                    }
                } else {
                    int channel_img = get_channel_image(base.orientation, ntile, fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1), channel_base_id);
                    auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                    command.image_id = channel_img;
                    command.pixel = offset;
                    command.mask = color_mask;
                    command.use_sort_pixel = true;
                    command.sort_pixel = offset + vec2i(0, 1);
                    command.location = SOURCE_LOCATION;
                }
            }
        }
    } else if (monumentd.phase == 3) {
        const xstring &base_key_1 = animkeys().ditches_phase_1;
        const xstring &base_key_2 = animkeys().ditches_phase_2;
        int channel_base_id_1 = current_params().animations[base_key_1].first_img();
        int channel_base_id_2 = current_params().animations[base_key_2].first_img();
        draw_phase_3_5_tile(ctx, color_mask, channel_base_id_1, channel_base_id_2, tiles_size);
    } else if (monumentd.phase == 4) {
        const xstring &base_key_2 = animkeys().ditches_phase_2;
        const xstring &base_key_3 = animkeys().ditches_phase_3;
        int channel_base_id_2 = current_params().animations[base_key_2].first_img();
        int channel_base_id_3 = current_params().animations[base_key_3].first_img();
        draw_phase_3_5_tile(ctx, color_mask, channel_base_id_2, channel_base_id_3, tiles_size);
    } else if (monumentd.phase == 5) {
        const xstring &base_key_3 = animkeys().ditches_phase_3;
        const xstring &base_key_4 = animkeys().ditches_phase_4;
        int channel_base_id_3 = current_params().animations[base_key_3].first_img();
        int channel_base_id_4 = current_params().animations[base_key_4].first_img();
        draw_phase_3_5_tile(ctx, color_mask, channel_base_id_3, channel_base_id_4, tiles_size);
    }

    return true;
}

void building_stepped_pyramid::setup_phase_6_basement() {
    if (!is_main()) {
        return;
    }

    tile2i ctile = center_point();

    int image_basement = current_params().animations["basement"].first_img();
    {
        tile2i left_top = ctile.shifted(-2, 0);
        auto m = building_at(left_top)->dcast_monument();
        auto &md = m->runtime_data();
        md.alt_image = image_basement + 1;
    }

    {
        tile2i right_top = ctile.shifted(0, 0);
        auto m = building_at(right_top)->dcast_monument();
        auto &md = m->runtime_data();
        md.alt_image = image_basement + 3;
    }

    {
        tile2i left_bottom = ctile.shifted(-2, -2);
        auto m = building_at(left_bottom)->dcast_monument();
        auto &md = m->runtime_data();
        md.alt_image = image_basement + 2;
    }

    {
        tile2i right_bottom = ctile.shifted(0, -2);
        auto m = building_at(right_bottom)->dcast_monument();
        auto &md = m->runtime_data();
        md.alt_image = image_basement;
    }
}

void building_stepped_pyramid::setup_phase_6_tiles() {
    const auto &itiles = pyramid_params().init_tiles;

    tile2i fnw = footprint_nw();
    tile2i end = fnw.shifted(itiles.y - 1, itiles.x - 1);

    const xstring &base_key_4 = animkeys().ditches_phase_4;
    int channel_base_id_4 = current_params().animations[base_key_4].first_img();

    for (int dy = 0; dy < size(); dy++) {
        for (int dx = 0; dx < size(); dx++) {
            tile2i ntile = base.tile.shifted(dx, dy);
            int base_img = get_channel_image(base.orientation, ntile, fnw, end, channel_base_id_4);
            map_image_set(ntile, base_img);
        }
    }    
}

void building_stepped_pyramid::draw_phase_3_5_tile(painter& ctx, color color_mask, int channel_base_id_1, int channel_base_id_2, const vec2i tiles_size) {
    tile2i fnw = footprint_nw();
    tile2i end = fnw.shifted(tiles_size.y - 1, tiles_size.x - 1);

    for (int dy = 0; dy < base.size; dy++) {
        for (int dx = 0; dx < base.size; dx++) {
            tile2i ntile = base.tile.shifted(dx, dy);
            vec2i offset = g_camera.lookup_tile_to_pixel(ntile);
            uint32_t progress = map_monuments_get_progress(ntile);
            if (progress < 200) {
                int ditch_empty_img = get_channel_image(base.orientation, ntile, fnw, end, channel_base_id_1);

                auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                command.image_id = ditch_empty_img;
                command.pixel = offset;
                command.mask = color_mask;
                command.use_sort_pixel = true;
                command.sort_pixel = offset + vec2i(0, 1);
                command.location = SOURCE_LOCATION;

                int ditch_water_img = get_channel_image(base.orientation, ntile, fnw, end, channel_base_id_2);
                if (ditch_water_img > 0 && progress > 0) {

                    auto &channel_command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile);
                    channel_command.image_id = ditch_water_img;
                    channel_command.pixel = offset;

                    const uint8_t alpha = (0xff * progress / 200);
                    channel_command.mask = ((alpha << COLOR_BITSHIFT_ALPHA) | (color_mask & 0x00ffffff));
                    channel_command.flags = (progress < 200) ? ImgFlag_Alpha : 0;
                    channel_command.location = SOURCE_LOCATION;
                }
            } else {
                int channel_img = get_channel_image(base.orientation, ntile, fnw, end, channel_base_id_2);
                auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
                command.image_id = channel_img;
                command.pixel = offset;
                command.mask = color_mask;
                command.use_sort_pixel = true;
                command.sort_pixel = offset + vec2i(0, 1);
                command.location = SOURCE_LOCATION;
            }
        }
    }
}

int building_stepped_pyramid::get_bricks_image(int orientation, tile2i tile, tile2i start, tile2i end, int layer) {
    return get_masonry_image(orientation, tile, start, end, layer, false);
}

int building_stepped_pyramid::get_masonry_image(int orientation, tile2i tile, tile2i start, tile2i end, int layer, bool polished) {
    layer %= 6;

    // NW origin building is placed as filler_type but must use corner sprites.
    // Do not use is_main() — orientation can put a wall/corner first in the chain.
    const bool is_nw_origin = (base.tile == footprint_nw());
    const bool is_floor = building_type_any_of(type(), {
        BUILDING_SMALL_STEPPED_PYRAMID, BUILDING_MEDIUM_STEPPED_PYRAMID, BUILDING_LARGE_STEPPED_PYRAMID,
        BUILDING_STEPPED_PYRAMID_COMPLEX,
        BUILDING_SMALL_BENT_PYRAMID, BUILDING_MEDIUM_BENT_PYRAMID,
        BUILDING_SMALL_PYRAMID, BUILDING_SMALL_PYRAMID_CONE,
        BUILDING_MEDIUM_PYRAMID, BUILDING_MEDIUM_PYRAMID_CONE
    });
    if (is_floor && !is_nw_origin) {
        const xstring floor_key = polished && current_params().first_img("base_polish") > 0
            ? "base_polish" : "base_bricks";
        return current_params().first_img(floor_key) + layer;
    }

    tile = building_at(tile)->tile;
    start = building_at(start)->tile;
    end = building_at(end)->tile;

    const bool is_corner = building_type_any_of(type(), {
        BUILDING_SMALL_STEPPED_PYRAMID_CORNER, BUILDING_MEDIUM_STEPPED_PYRAMID_CORNER, BUILDING_LARGE_STEPPED_PYRAMID_CORNER,
        BUILDING_SMALL_BENT_PYRAMID_CORNER, BUILDING_MEDIUM_BENT_PYRAMID_CORNER,
        BUILDING_SMALL_PYRAMID_CORNER, BUILDING_MEDIUM_PYRAMID_CORNER
    });
    if (is_corner || is_nw_origin) {
        const xstring corner_key = polished && current_params().first_img("corner_polish") > 0
            ? "corner_polish" : "corner_bricks";
        int image_corner_bricks = current_params().first_img(corner_key) + (layer * 8);
        const bool at_min_x = (tile.x() == start.x());
        const bool at_min_y = (tile.y() == start.y());
        const bool at_max_x = (tile.x() == end.x());
        const bool at_max_y = (tile.y() == end.y());
        if (at_min_x && at_min_y) return image_corner_bricks + 0;
        if (at_min_x && at_max_y) return image_corner_bricks + 1;
        if (at_max_x && at_max_y) return image_corner_bricks + 2; // visual bottom → stepped_pyramid_00043 at layer 5
        if (at_max_x && at_min_y) return image_corner_bricks + 3;
        return image_corner_bricks;
    }

    const bool is_wall = building_type_any_of(type(), {
        BUILDING_SMALL_STEPPED_PYRAMID_WALL, BUILDING_MEDIUM_STEPPED_PYRAMID_WALL, BUILDING_LARGE_STEPPED_PYRAMID_WALL,
        BUILDING_SMALL_BENT_PYRAMID_WALL, BUILDING_MEDIUM_BENT_PYRAMID_WALL,
        BUILDING_SMALL_PYRAMID_WALL, BUILDING_MEDIUM_PYRAMID_WALL
    });
    if (is_wall) {
        const xstring wall_key = polished && current_params().first_img("wall_polish") > 0
            ? "wall_polish" : "wall_bricks";
        int image_wall_bricks = current_params().first_img(wall_key) + (layer * 8);
        if (tile.x() == start.x()) return image_wall_bricks + 0;
        if (tile.y() == start.y()) return image_wall_bricks + 3;
        if (tile.x() == end.x()) return image_wall_bricks + 2;
        if (tile.y() == end.y()) return image_wall_bricks + 1;
        return image_wall_bricks;
    }

    return 0;
}

void building_stepped_pyramid::draw_ornaments_and_animations_stairs_impl(painter &ctx, vec2i point, tile2i tile, color color_mask, const vec2i tiles_size) {
    // phase() is uint8_t: MONUMENT_FINISHED (-1) becomes 255, which would match every stair.
    if (is_finished()) {
        return;
    }

    auto &d = runtime_data();
    const auto &stairs = pyramid_params().stairs;
    if (d.stair_index == 0xff || d.stair_index >= stairs.size()) {
        return;
    }

    const auto &stair = stairs[d.stair_index];
    if (phase() >= stair.phase) {
        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = stair.tex.first_img();
        command.pixel = point + stair.offset;
        command.mask = color_mask;
    }
}

bool building_stepped_pyramid::draw_ornaments_and_animations_hight_impl(painter &ctx, vec2i point, tile2i tile, color color_mask, const vec2i tiles_size) {
    color_mask = (color_mask ? color_mask : 0xffffffff);
    tile2i fnw = footprint_nw();
    const auto &d = runtime_data();

    auto fill_tiles_height = [] (painter &ctx, tile2i tile, int img) {
        auto image = image_get(img);
        int iso_size = image->isometric_size() - 1;
        grid_tiles tiles = map_grid_get_tiles(tile, tile.shifted(iso_size, iso_size));
        for (auto &t : tiles) {
            map_building_height_set(t.grid_offset(), image->isometric_top_height);
        }
    };

    auto &monumentd = runtime_data();
    if (phase() == 6) {
        uint32_t progress = map_monuments_get_progress(tile);
        if (progress >= 200) {
            int img = get_bricks_image(base.orientation, tile, fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1), phase() - 6);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point;
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        }
    } else if (phase() > 6 && phase() < 12) {
        uint32_t progress = map_monuments_get_progress(tile);
        int img = get_bricks_image(base.orientation, tile, fnw, fnw.shifted(tiles_size.y - 1, tiles_size.x - 1), (progress >= 200) ? (phase() - 6) : (phase() - 7));
        vec2i offset = g_camera.lookup_tile_to_pixel(tile);

        auto& command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point;
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
        draw_ornaments_and_animations_stairs_impl(ctx, point, tile, color_mask, tiles_size);
    } else if (phase() >= 12 && phase() < 18) {
        uint32_t progress = map_monuments_get_progress(tile);
        if (d.layer == 0) {
            auto layer_area = get_layer_area(0);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point;
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else {
            int layer_img = current_params().first_img("base_bricks") + 5;
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = layer_img;
            command.pixel = point;
            command.mask = color_mask;

            int base_level = (phase() - 12) + ((progress >= 200) ? 1 : 0);
            if (base_level > 0) {
                auto layer_area = get_layer_area(1);

                int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, base_level - 1);
                auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
                command.image_id = img;
                command.pixel = point - vec2i{0, TILE_HEIGHT_PIXELS * 3};
                command.mask = color_mask;
            }
        }
        draw_ornaments_and_animations_stairs_impl(ctx, point, tile, color_mask, tiles_size);
    } else if (phase() >= 18 && phase() < 24) {
        uint32_t progress = map_monuments_get_progress(tile);
        if (d.layer == 0) {
            auto layer_area = get_layer_area(0);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point;
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 1) {
            auto layer_area = get_layer_area(1);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 3 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else {
            int layer_img = current_params().first_img("base_bricks") + 5;
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = layer_img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 3 };
            command.mask = color_mask;

            int base_level = (phase() - 18) + ((progress >= 200) ? 1 : 0);
            if (base_level > 0) {
                auto layer_area = get_layer_area(2);

                int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, base_level - 1);
                auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
                command.image_id = img;
                command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 6 };
                command.mask = color_mask;
            }
        }
        draw_ornaments_and_animations_stairs_impl(ctx, point, tile, color_mask, tiles_size);
    } else if (phase() >= 24 && phase() < 30) {
        uint32_t progress = map_monuments_get_progress(tile);
        if (d.layer == 0) {
            auto layer_area = get_layer_area(0);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point;
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 1) {
            auto layer_area = get_layer_area(1);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 3 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 2) {
            auto layer_area = get_layer_area(2);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 6 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else {
            // Building layer 3 (fourth tier): underlay at L2 height, bricks at L3.
            int layer_img = current_params().first_img("base_bricks") + 5;
            vec2i offset = g_camera.lookup_tile_to_pixel(tile);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = layer_img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 6 };
            command.mask = color_mask;

            int base_level = (phase() - 24) + ((progress >= 200) ? 1 : 0);
            if (base_level > 0) {
                auto layer_area = get_layer_area(3);

                int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, base_level - 1);
                auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
                command.image_id = img;
                command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 9 };
                command.mask = color_mask;
            }
        }
        draw_ornaments_and_animations_stairs_impl(ctx, point, tile, color_mask, tiles_size);
    } else if (phase() >= 30 && phase() < 36) {
        // Large apex (layer 4 / fifth tier). Medium never sets layer≥3 here — its
        // parts stay on 0..2 and draw as finished lower tiers through phase 31.
        uint32_t progress = map_monuments_get_progress(tile);
        if (d.layer == 0) {
            auto layer_area = get_layer_area(0);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point;
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 1) {
            auto layer_area = get_layer_area(1);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 3 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 2) {
            auto layer_area = get_layer_area(2);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 6 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else if (d.layer == 3) {
            auto layer_area = get_layer_area(3);
            int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, 5);

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 9 };
            command.mask = color_mask;

            fill_tiles_height(ctx, tile, img);
        } else {
            // Building layer 4: underlay at L3 height, bricks at L4 (*12).
            int layer_img = current_params().first_img("base_bricks") + 5;

            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
            command.image_id = layer_img;
            command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 9 };
            command.mask = color_mask;

            int base_level = (phase() - 30) + ((progress >= 200) ? 1 : 0);
            if (base_level > 0) {
                auto layer_area = get_layer_area(4);

                int img = get_bricks_image(base.orientation, tile, layer_area.begin, layer_area.end, base_level - 1);
                auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
                command.image_id = img;
                command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 12 };
                command.mask = color_mask;
            }
        }
        draw_ornaments_and_animations_stairs_impl(ctx, point, tile, color_mask, tiles_size);
    } else if (is_finished() || phase() == 24 || phase() == 32 || phase() == 36) {
        // Complete pyramid: last construction phase (small 24 / medium 32 / large 36)
        // or MONUMENT_FINISHED. Note: phase==24 is shadowed by `>=24 && <30` above for
        // in-progress draws — finished uses phase 255. Shared body:
        return draw_completed_height_ornaments(ctx, point, tile, color_mask, tiles_size);
    }

    return true;
}

bool building_stepped_pyramid::draw_completed_height_ornaments(painter &ctx, vec2i point, tile2i tile, color color_mask, const vec2i tiles_size) {
    color_mask = (color_mask ? color_mask : 0xffffffff);
    const auto &d = runtime_data();
    const bool polished = use_polish_sprites_for_layer(d.layer);

    auto fill_tiles_height = [] (painter &ctx, tile2i tile, int img) {
        auto image = image_get(img);
        int iso_size = image->isometric_size() - 1;
        grid_tiles tiles = map_grid_get_tiles(tile, tile.shifted(iso_size, iso_size));
        for (auto &t : tiles) {
            map_building_height_set(t.grid_offset(), image->isometric_top_height);
        }
    };

    if (d.layer == 0) {
        auto layer_area = get_layer_area(0);
        int img = get_masonry_image(base.orientation, tile, layer_area.begin, layer_area.end, 5, polished);

        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point;
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
    } else if (d.layer == 1) {
        auto layer_area = get_layer_area(1);
        int img = get_masonry_image(base.orientation, tile, layer_area.begin, layer_area.end, 5, polished);

        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 3 };
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
    } else if (d.layer == 2) {
        auto layer_area = get_layer_area(2);
        int img = get_masonry_image(base.orientation, tile, layer_area.begin, layer_area.end, 5, polished);

        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 6 };
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
    } else if (d.layer == 3) {
        auto layer_area = get_layer_area(3);
        int img = get_masonry_image(base.orientation, tile, layer_area.begin, layer_area.end, 5, polished);

        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 9 };
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
    } else if (d.layer == 4) {
        auto layer_area = get_layer_area(4);
        int img = get_masonry_image(base.orientation, tile, layer_area.begin, layer_area.end, 5, polished);

        auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_drawtile_full);
        command.image_id = img;
        command.pixel = point - vec2i{ 0, TILE_HEIGHT_PIXELS * 12 };
        command.mask = color_mask;

        fill_tiles_height(ctx, tile, img);
    }

    return true;
}

span_const<uint16_t> building_stepped_pyramid::active_workers() const {
    auto &monumentd = runtime_data();
    return span_const<uint16_t>(monumentd.workers);
}

void building_stepped_pyramid::update_day(const vec2i tiles_size) {
    auto &monumentd = runtime_data();
    // Callers also guard, but keep a local bail — after FINISHED, phase() as
    // uint8_t is 255 and must not re-enter promote / ring logic.
    if (is_finished()) {
        return;
    }

    grid_tiles tiles = map_grid_get_tiles(&base, 0);
    tile2i tile2works = map_grid_area_first(tiles, [] (tile2i tile) { return map_monuments_get_progress(tile) < 200; });
    bool all_tiles_finished = (tile2works == tile2i{ -1, -1 });
    building_impl *part = this;
    if (!is_main()) {
        return;
    }

    if (all_tiles_finished) {
        int curr_phase = phase();
        const int nphases = phases();
        // Last schedule index is a terminal marker (RESOURCE_NONE) — same pattern as
        // mastaba phase 8. Keep tiles at 200 when entering it so the next day can
        // advance past size → FINISHED. Clearing here left progress at 0 forever
        // (no mason work on an empty terminal) and natural finish never happened.
        const bool entering_terminal = (curr_phase + 1 >= nphases - 1);
        const bool finishing = (curr_phase + 1 >= nphases);
        if (!entering_terminal) {
            map_grid_area_foreach(tiles, [] (tile2i tile) { map_monuments_set_progress(tile, 0); });
        }
        while (part) {
            verify_no_crash(part->dcast_monument());
            part->dcast_monument()->set_phase(curr_phase + 1);
            part = part->has_next() ? part->next() : nullptr;
        }
        // set_phase(phases()) stores FINISHED immediately — the old
        // `phase() >= phases()` head-check never saw a live phase==size and
        // congrats never fired on natural completion. Post them here once.
        if (finishing) {
            // Use schedule btype (shared by corner/wall parts), not the part's
            // building type — orientation can make a corner the is_main() head.
            const e_building_type mon_type = config().btype;
            const bool is_bent = (mon_type == BUILDING_SMALL_BENT_PYRAMID
                || mon_type == BUILDING_MEDIUM_BENT_PYRAMID);
            const bool is_true = (mon_type == BUILDING_SMALL_PYRAMID
                || mon_type == BUILDING_MEDIUM_PYRAMID);
            pcstr congrats = is_true ? "pyramid_congratulations"
                : (is_bent ? "bent_pyramid_congratulations" : "stepped_pyramid_congratulations");
            city_message &message = city_message_post_with_popup_delay(MESSAGE_CAT_MONUMENTS, true, congrats, type(), tile().grid_offset());
            message.hide_img = true;
            return;
        }
    }

    // Resource gate: claimed tiles (progress 1) become workable (2) once enough
    // materials are on-site. Mastaba uses phase >= 2; using >= 6 here left phases
    // 2–5 with masons stuck on progress 1 forever (need_stonemason starts at 2).
    if (phase() >= 2) {
        int minimal_percent = 100;
        for (e_resource r = RESOURCES_MIN; r < RESOURCES_MAX; ++r) {
            bool need_resource = needs_resource(r);
            if (need_resource) {
                minimal_percent = std::min<int>(minimal_percent, monumentd.resources_pct[r]);
            }
        }
        minimal_percent = std::clamp(minimal_percent, 0, 100);

        grid_tiles tiles = map_grid_get_tiles(&base, 0);
        tiles.resize(tiles.size() * minimal_percent / 100);

        for (auto &tile : tiles) {
            int progress = map_monuments_get_progress(tile);
            if (progress == 1) {
               set_tile_progress(tile, 2);
            }
        }
    }
}

void building_stepped_pyramid::on_place_checks() {
    const tile2i tiles_to_check[] = { tile(), tile().shifted(1, 0), tile().shifted(0, 1), tile().shifted(1, 1) };
    bool has_water = false;
    for (const auto &t : tiles_to_check) {
        has_water |= map_terrain_is(t, TERRAIN_GROUNDWATER);
    }

    construction_warnings warnings;
    warnings.add_if(!has_water, "#needs_groundwater");

    assign_stair();
}

void building_stepped_pyramid::update_count() const {
    if (!is_main()) {
        return;
    }

    building_monument::update_count();
}

void building_stepped_pyramid::update_month() {
    if (!is_main()) {
        return;
    }

    auto &monumentd = runtime_data();
    for (uint16_t &w_id : monumentd.workers) {
        figure* f = figure_get(w_id);
        if (!f || !f->is_alive() || f->destination() != &base) {
            w_id = 0;
        }
    }
}

void building_stepped_pyramid::update_map_orientation(int map_orientation) {
    if (is_finished()) {
        // Finished stepped pyramids are drawn via height ornaments (full tiers).
        // Do not reuse mastaba brick lookup — wrong footprint (3,9) and images → black grid.
        map_building_tiles_add(id(), tile(), base.size, building_image_get(), TERRAIN_BUILDING);
    }
}

bool building_stepped_pyramid::force_draw_flat_tile(painter &ctx, tile2i tile, vec2i pixel, color mask) {
    if (is_finished()) {
        return false;
    }

    return (phase() < 7);
}

void building_stepped_pyramid::bind_dynamic(io_buffer *iob, size_t version) {
    auto &monumentd = runtime_data();

    iob->bind____skip(36);
    iob->bind_u16(monumentd.alt_image);
    iob->bind_u8(base.orientation);
    for (int i = 0; i < 5; i++) {
        iob->bind_u16(monumentd.workers[i]);
    }
    iob->bind_i8(monumentd.phase);
    iob->bind_u8(monumentd.layer);
    iob->bind_u8(monumentd.stair_index);
    iob->bind_u8(monumentd.variant);

    for (int i = 0; i < RESOURCES_MAX; i++) {
        iob->bind_u8(monumentd.resources_pct[i]);
    }

    // funeral_done appended (mastaba reclaims a former skip byte instead).
    if (version >= 179) {
        iob->bind_u8(monumentd.funeral_done);
    }
    // Preexisting sealed tomb flag.
    if (version >= 180) {
        iob->bind_u8(monumentd.preexisting);
    }
}

int building_small_stepped_pyramid::building_image_get() const {
    switch (runtime_data().phase) {
    case MONUMENT_START:
        return building_static_params::get(BUILDING_SMALL_STEPPED_PYRAMID).base_img();
    default:
        return building_static_params::get(BUILDING_SMALL_STEPPED_PYRAMID).base_img() + 1;
    }

    return 0;
}

void building_small_stepped_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

bool building_small_stepped_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_small_stepped_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_small_stepped_pyramid::config() const {
    return g_monument_small_stepped_pyramid;
}

// Helper function to check if a block is a corner block (by block index)
bool is_corner_block(int block_x, int block_y, int blocks_x, int blocks_y) {
    return (block_x == 0 && block_y == 0) ||                                  // top-left
        (block_x == blocks_x - 1 && block_y == 0) ||                       // top-right
        (block_x == blocks_x - 1 && block_y == blocks_y - 1) ||            // bottom-right
        (block_x == 0 && block_y == blocks_y - 1);                         // bottom-left
}

// Helper function to check if a block is on the perimeter (by block index)
bool is_on_perimeter_block(int block_x, int block_y, int blocks_x, int blocks_y) {
    return block_x == 0 || block_x == blocks_x - 1 ||
        block_y == 0 || block_y == blocks_y - 1;
}

void building_stepped_pyramid::change_parts_types_in_layer(tile2i begin, const vec2i layer_size, uint8_t layer) {
    hvector<pyramid_part, 256> parts;

    // Parts are 2x2 tiles, so calculate number of blocks in each dimension
    const int part_size = 2;
    int blocks_x = layer_size.x / part_size;  // e.g., 10/2 = 5
    int blocks_y = layer_size.y / part_size;  // e.g., 10/2 = 5

    const auto &pi_params = pyramid_params();

    // Fill pyramid parts for the first layer
    // Iterate through blocks (each block is 2x2 tiles)
    for (int block_y = 0; block_y < blocks_y; block_y++) {
        for (int block_x = 0; block_x < blocks_x; block_x++) {
            // Calculate tile offset for this block (top-left corner of 2x2 block)
            int tile_x = block_x * part_size;
            int tile_y = block_y * part_size;
            tile2i part_tile = begin.shifted(tile_x, tile_y);

            // Skip only the real NW building of layer 0. Inner layers' NW
            // corner is a normal part — skipping it leaves the apex 4×4 with a
            // stale filler at layer-1 and a hollow/L-shaped top.
            // Use footprint_nw (not list head): orientation can make a wall is_main.
            if (part_tile == footprint_nw()) {
                continue;
            }

            // Determine part type based on block position
            e_building_type part_type;
            if (is_on_perimeter_block(block_x, block_y, blocks_x, blocks_y)) {
                // Block is on perimeter
                if (is_corner_block(block_x, block_y, blocks_x, blocks_y)) {
                    part_type = pi_params.corner_type;
                } else {
                    part_type = pi_params.wall_type;
                }
            } else {
                // Block is interior (filler)
                part_type = pi_params.filler_type;
            }

            parts.push_back({ part_type, {tile_x, tile_y}, building_at(part_tile) });
        }
    }

    for (auto &part : parts) {
        part.b->clear_impl();
        part.b->type = part.type;
        auto *mon = part.b->dcast_monument();
        mon->runtime_data().layer = layer;
        mon->runtime_data().stair_index = 0xff;
    }

    // clear_impl resets stair_index — rebind ramps for the new ring types.
    for (building *b = base.main(); b; b = b->has_next() ? b->next() : nullptr) {
        if (building_pyramid *pyr = b->dcast_pyramid()) {
            static_cast<building_stepped_pyramid *>(pyr)->assign_stair();
        }
    }
}

building_stepped_pyramid::layer_area building_stepped_pyramid::get_layer_area(int layer) const {
    const auto &pi_params = pyramid_params();
    vec2i layer_size = pi_params.init_tiles;
    layer_size -= vec2i{ layer * 4, layer * 4 };
    tile2i begin_tile = footprint_nw().shifted(layer * 2, layer * 2);
    tile2i end_tile = begin_tile.shifted(layer_size - vec2i{1, 1});

    return { begin_tile, end_tile, layer_size };
}

void building_stepped_pyramid::on_place(int orientation, int variant) {
    building_pyramid::on_place(orientation, variant);

    base.prev_part_building_id = 0;
    map_pyramid_tiles_add(id(), tile(), base.size, -1, TERRAIN_BUILDING);

    hvector<pyramid_part, 256> parts;
    
    // Get pyramid size from params
    const auto &pi_params = pyramid_params();
    vec2i pyramid_size = pi_params.init_tiles;
    
    // Parts are 2x2 tiles, so calculate number of blocks in each dimension
    const int part_size = 2;
    int blocks_x = pyramid_size.x / part_size;  // e.g., 10/2 = 5
    int blocks_y = pyramid_size.y / part_size;  // e.g., 10/2 = 5
    
    // Fill pyramid parts for the first layer
    // Iterate through blocks (each block is 2x2 tiles)
    for (int block_y = 0; block_y < blocks_y; block_y++) {
        for (int block_x = 0; block_x < blocks_x; block_x++) {
            // Calculate tile offset for this block (top-left corner of 2x2 block)
            int tile_x = block_x * part_size;
            int tile_y = block_y * part_size;
            
            // Skip the main building position (top-left block)
            if (block_x == 0 && block_y == 0) {
                continue;
            }
            
            // Determine part type based on block position
            e_building_type part_type;
            if (is_on_perimeter_block(block_x, block_y, blocks_x, blocks_y)) {
                // Block is on perimeter
                if (is_corner_block(block_x, block_y, blocks_x, blocks_y)) {
                    part_type = pi_params.corner_type;
                } else {
                    part_type = pi_params.wall_type;
                }
            } else {
                // Block is interior (filler)
                part_type = pi_params.filler_type;
            }

            parts.push_back({part_type, {tile_x, tile_y}, nullptr});
        }
    }

    // Create all part buildings
    for (auto &part : parts) {
        part.b = building_create(part.type, tile().shifted(part.offset), 0);
        game_undo_add_building(part.b);
        tile2i btile_add = tile().shifted(part.offset);
        map_pyramid_tiles_add(part.b->id, btile_add, part.b->size, -1, TERRAIN_BUILDING);
    }

    // Add main building to parts list
    pyramid_part main{type(), {0, 0}, &base};
    switch (orientation) {
    case 0: parts.insert(parts.begin(), main); break;
    case 1: parts.insert(parts.begin() + 1, main); break;
    case 2: parts.push_back(main); break;
    case 3: parts.push_back(main); break;
    }

    // Link all parts together
    building* prev_part = nullptr;
    for (auto &part : parts) {
        part.b->prev_part_building_id = prev_part ? prev_part->id : 0;
        if (prev_part) {
            prev_part->next_part_building_id = part.b->id;
        }
        prev_part = part.b;
    }

    // Bind ramp sprites to the matching 2×2 parts. Without this, stair_index stays 0
    // and every part draws stairs[0] at ground level along the edge.
    for (auto &part : parts) {
        if (building_pyramid *pyr = part.b->dcast_pyramid()) {
            static_cast<building_stepped_pyramid *>(pyr)->assign_stair();
        }
    }
}

void building_stepped_pyramid::on_phase_changed(int old, int current) {
    if (current >= 2) {
        int terrain = TERRAIN_BUILDING;
        if (current >= 3) {
            terrain |= TERRAIN_CANAL;
        }
        map_building_tiles_add(id(), tile(), size(), this->building_image_get(), terrain);
    }

    if (current == 6) {
        setup_phase_6_tiles();
        setup_phase_6_basement();
    }

    if (current == 12 && is_main()) {
        auto area = get_layer_area(1);
        change_parts_types_in_layer(area.begin, area.size, 1);
    }

    if (current == 18 && is_main()) {
        auto area = get_layer_area(2);
        change_parts_types_in_layer(area.begin, area.size, 2);
    }

    if (current == 24 && is_main()) {
        auto area = get_layer_area(3);
        if (area.size.x > 0 && area.size.y > 0) {
            change_parts_types_in_layer(area.begin, area.size, 3);
        }
    }

    if (current == 30 && is_main()) {
        auto area = get_layer_area(4);
        if (area.size.x > 0 && area.size.y > 0) {
            change_parts_types_in_layer(area.begin, area.size, 4);
        }
    }

    if (current != MONUMENT_FINISHED) {
        auto &d = runtime_data();
        for (e_resource resource = RESOURCE_NONE; resource < RESOURCES_MAX; ++resource) {
            d.resources_pct[resource] = 0;
        }
    }
}

void building_medium_stepped_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_medium_stepped_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_medium_stepped_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_medium_stepped_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_medium_stepped_pyramid::config() const {
    return g_monument_medium_stepped_pyramid;
}

// --- Large stepped pyramid (20×20) ---------------------------------------------
// Thin per-type wrappers around the shared stepped machinery, keyed on their own
// current_params() (init_tiles [20,20]).

void building_large_stepped_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_large_stepped_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_large_stepped_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_large_stepped_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_large_stepped_pyramid::config() const {
    return g_monument_large_stepped_pyramid;
}

// --- Stepped pyramid complex (20×20, on-land) -----------------------------------
// Same wrappers as large (TYPE-keyed current_params). Phase schedule shared with large.

void building_stepped_pyramid_complex::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_stepped_pyramid_complex::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_stepped_pyramid_complex::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_stepped_pyramid_complex::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_stepped_pyramid_complex::config() const {
    // Same cadence as large until causeway/temples land; keep btype = COMPLEX for any
    // schedule readers.
    static monument m = [] {
        monument copy = g_monument_large_stepped_pyramid;
        copy.btype = BUILDING_STEPPED_PYRAMID_COMPLEX;
        return copy;
    }();
    return m;
}

// --- Bent pyramids -------------------------------------------------------------
// Thin per-type wrappers around the shared stepped machinery. Each keys on its own
// current_params() (TYPE = BENT), so the rendered art comes from PACK_BENT_PYRAMID.

void building_small_bent_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_small_bent_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_small_bent_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_small_bent_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_small_bent_pyramid::config() const {
    return g_monument_small_bent_pyramid;
}

void building_medium_bent_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_medium_bent_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_medium_bent_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_medium_bent_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_ornaments_and_animations_hight_impl(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

const monument &building_medium_bent_pyramid::config() const {
    return g_monument_medium_bent_pyramid;
}

// --- True (smooth) small pyramid (polish phases) --------------------------------

void building_small_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_small_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_small_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_small_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_completed_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    // Polish (24–25) and terminal (26): full-height silhouette with top-down casing.
    // Do not call unfinished→hight_impl (phase≥24 hits large L3 construction band).
    if (phase() >= 24) {
        if (city_flat_should_flatten_building(base)) {
            return true;
        }
        return draw_completed_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

bool building_small_pyramid::need_stonemason() {
    if (is_finished() || !is_main()) {
        return false;
    }
    // Clear/level (0–1) = work-camp peasants; casing + polish = stonemasons.
    // Skip the terminal schedule marker (last index) — tiles stay at 200, no work left.
    const int p = phase();
    if (p < 2 || p >= phases() - 1) {
        return false;
    }
    // Respect worker slots so multiple guilds don't over-assign.
    return need_workers();
}

void building_small_pyramid::on_phase_changed(int old, int current) {
    // Stepped on_phase_changed treats 24/30 as large L3/L4 ring raises. True small
    // uses 24–25 for polish — do shared tile/resource bookkeeping only.
    if (current == 24 || current == 30) {
        if (current >= 2) {
            int terrain = TERRAIN_BUILDING;
            if (current >= 3) {
                terrain |= TERRAIN_CANAL;
            }
            map_building_tiles_add(id(), tile(), size(), building_image_get(), terrain);
        }
        if (current != MONUMENT_FINISHED) {
            auto &d = runtime_data();
            for (e_resource resource = RESOURCE_NONE; resource < RESOURCES_MAX; ++resource) {
                d.resources_pct[resource] = 0;
            }
        }
        return;
    }
    building_stepped_pyramid::on_phase_changed(old, current);
}

bool building_small_pyramid::use_polish_sprites_for_layer(int layer) const {
    if (is_finished()) {
        return true;
    }
    const int p = phase();
    // Top-down: first polish phase covers upper layers, second covers base.
    if (p >= 25) {
        return true;
    }
    if (p >= 24) {
        return layer >= 1;
    }
    return false;
}

const monument &building_small_pyramid::config() const {
    return g_monument_small_pyramid;
}

// --- True (smooth) medium pyramid (C3a2 + C3.4 polish after height) ------------

void building_medium_pyramid::update_day() {
    building_impl::update_day();

    if (is_finished()) {
        return;
    }

    building_stepped_pyramid::update_day(current_params().init_tiles);
}

int building_medium_pyramid::building_image_get() const {
    switch (phase()) {
    case MONUMENT_START:
        return current_params().base_img();
    default:
        return current_params().base_img() + 1;
    }

    return 0;
}

bool building_medium_pyramid::draw_ornaments_and_animations_flat(painter &ctx, vec2i point, tile2i tile, color mask) {
    return draw_ornaments_and_animations_flat_impl(ctx, point, tile, mask, current_params().init_tiles);
}

bool building_medium_pyramid::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    if (is_finished()) {
        return draw_completed_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    // Polish/terminal: full-height casing. Height courses use 24/30 as brick bands —
    // only divert once polish begins (phase >= 32).
    if (phase() >= k_polish_phase_begin) {
        if (city_flat_should_flatten_building(base)) {
            return true;
        }
        return draw_completed_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
    }
    return draw_unfinished_height_ornaments(ctx, point, tile, color_mask, current_params().init_tiles);
}

bool building_medium_pyramid::need_stonemason() {
    if (is_finished() || !is_main()) {
        return false;
    }
    const int p = phase();
    if (p < 2 || p >= phases() - 1) {
        return false;
    }
    return need_workers();
}

bool building_medium_pyramid::use_polish_sprites_for_layer(int layer) const {
    if (is_finished()) {
        return true;
    }
    const int p = phase();
    // Top-down across 3 layers (0..2): 32→top, 33→mid+, 34+→all.
    if (p >= k_polish_phase_begin + 2) {
        return true;
    }
    if (p >= k_polish_phase_begin + 1) {
        return layer >= 1;
    }
    if (p >= k_polish_phase_begin) {
        return layer >= 2;
    }
    return false;
}

const monument &building_medium_pyramid::config() const {
    return g_monument_medium_pyramid;
}