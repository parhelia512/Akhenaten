#pragma once

#include "building/monuments.h"
#include "building/building.h"
#include "core/svector.h"
#include "core/vec2i.h"

class building_abu_simbel : public building_monument {
public:
    BUILDING_METAINFO(BUILDING_ABU_SIMBEL, building_abu_simbel, building_monument)
    virtual building_abu_simbel *dcast_abu_simbel() override { return this; }

    struct base_params {
        vec2i init_tiles = {9, 21}; // cliff bulk width × depth (orientation swaps)
        vec2i entrance_size = {3, 3}; // clear-land protrusion past far depth edge
        uint8_t art_stages = 8;
        // AS0.6 stub until pak/RE dump (Heaven W=Y only). Mason phases use 0.
        svector<uint16_t, 8> timber_loads{400, 400, 400, 200, 0, 0, 0, 0};
    };

    struct static_params : public base_params, public building_static_params {
        void archive_load(archive arch);
        void rebuild_construction();
    } BUILDING_STATIC_DATA_T;

    struct preview : building_planer_renderer {
        virtual void setup_preview_graphics(build_planner &planer) const override;
        virtual uint32_t ghost_ignore_terrain(build_planner &p, tile2i tile) const override;
        virtual bool ghost_allow_tile(build_planner &p, tile2i tile) const override;
        virtual int can_place(build_planner &p, tile2i tile, tile2i end, int state) const override;
        virtual int finalize_check(build_planner &p, tile2i tile, tile2i end, int state) const override;
        virtual void ghost_preview(build_planner &planer, painter &ctx, tile2i start, tile2i end, vec2i pixel) const override;
        virtual int construction_place(build_planner &p, tile2i tile, tile2i end, int orientation, int variant) const override;
    };

    virtual void on_place_update_tiles(int orientation, int variant) override;
    virtual void on_place_checks() override;
    virtual void on_destroy() override;
    virtual xstring demolish_blocked_message() const override;
    virtual void on_phase_changed(int old_phase, int current) override;
    virtual void update_day() override;
    virtual void update_map_orientation(int map_orientation) override;
    virtual int building_image_get() const override;
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
    xstring anim_key_for(int stage) const;
    vec2i bulk_size() const;  // oriented cliff bulk (9×21 / 21×9)
    vec2i total_size() const; // bulk + entrance depth (9×24 / 24×9)
    vec2i footprint_size() const { return total_size(); }

    // Any Abu Simbel on the map (finished or not) — only one allowed ever.
    static bool has_abu_simbel_on_map();

    // Local (dx,dy) in total footprint coords (origin = planner NW).
    static bool is_bulk_local(int dx, int dy, vec2i bulk);
    static bool is_entrance_local(int dx, int dy, vec2i bulk, vec2i entrance);
    static bool is_padding_local(int dx, int dy, vec2i bulk, vec2i entrance);

    bool need_carpenter_phase() const;
    bool has_stonemason_worker() const;
    bool has_carpenter_worker() const;
};
ANK_CONFIG_STRUCT(building_abu_simbel::static_params, init_tiles, entrance_size, art_stages, timber_loads)
