#pragma once

#include "building/monuments.h"
#include "building/building.h"
#include "core/svector.h"
#include "core/vec2i.h"

// Sun Temple: body 10×10 + path 8×2 + hall 3×3 (linked parts).
// Phase 0–1 = work-camp leveling on body only; 2–4 = progress().
class building_sun_temple : public building_monument {
public:
    BUILDING_METAINFO(BUILDING_SUN_TEMPLE, building_sun_temple, building_monument)
    virtual building_sun_temple *dcast_sun_temple() override { return this; }

    enum part_variant : uint8_t {
        PART_BODY = 0,
        PART_PATH = 1,
        PART_HALL = 2,
    };

    // EXTRA overlay (walls / plaza / ornaments). Offsets are provisional —
    // tune in sun_temple.js; body NW + placement rot0, then rotated in C++.
    struct plaza_overlay {
        xstring key;
        vec2i offset;
        vec2i pixel = {0, 0};
        int8_t min_phase = 4; // show when phase >= min (finished counts as 5+)
        int8_t max_phase = 99; // inclusive; use 1 for early stakes only
    };

    struct base_params {
        svector<monument_phase_resource, 4> placement_resources;
        uint8_t art_stages = 4;
        svector<uint16_t, 8> timber_loads;
        svector<uint16_t, 8> sandstone_loads;
        uint16_t build_sandstone = 160;
        vec2i init_tiles = {10, 15};
        vec2i path_size = {8, 2};
        vec2i hall_size = {3, 3};
        svector<vec2i, 4> part_path_offset;
        svector<vec2i, 4> part_hall_offset;
        svector<plaza_overlay, 24> plaza_overlays;
    };

    struct static_params : public base_params, public building_static_params {
        void archive_load(archive arch);
        void rebuild_construction();
    } BUILDING_STATIC_DATA_T;

    struct preview : building_planer_renderer {
        virtual void setup_preview_graphics(build_planner &planer) const override;
        virtual int can_place(build_planner &p, tile2i tile, tile2i end, int state) const override;
        virtual int finalize_check(build_planner &p, tile2i tile, tile2i end, int state) const override;
        virtual int construction_place(build_planner &p, tile2i tile, tile2i end, int orientation, int variant) const override;
        virtual void ghost_preview(build_planner &planer, painter &ctx, tile2i start, tile2i end, vec2i pixel) const override;
    };

    virtual void on_place(int orientation, int variant) override;
    virtual void on_place_update_tiles(int orientation, int variant) override;
    virtual void on_destroy() override;
    virtual void on_phase_changed(int old_phase, int current) override;
    virtual void update_day() override;
    virtual void update_map_orientation(int map_orientation) override;
    virtual int building_image_get() const override;
    virtual bool draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color mask) override;
    virtual bool need_stonemason() override;
    virtual bool need_workers() const override;
    virtual bool needs_resources() const override;
    virtual void add_workers(figure_id fid) override;
    virtual void remove_worker(figure_id fid) override;
    virtual const monument &config() const override;
    virtual tile2i center_point() const override;
    virtual tile2i access_point() const override;
    virtual grid_area get_area() const override;
    virtual void bind_dynamic(io_buffer *iob, size_t version) override;

    int art_stage() const;
    xstring anim_key_for(int stage, int orient_idx) const;
    int placement_amount(e_resource r) const;
    void refresh_part_tiles();
    static int yards_available(e_resource r);
    static bool has_unfinished_sun_temple();
};
ANK_CONFIG_STRUCT(building_sun_temple::plaza_overlay, key, offset, pixel, min_phase, max_phase)
ANK_CONFIG_STRUCT(building_sun_temple::static_params,
    placement_resources, art_stages, timber_loads, sandstone_loads, build_sandstone,
    init_tiles, path_size, hall_size, part_path_offset, part_hall_offset, plaza_overlays)
