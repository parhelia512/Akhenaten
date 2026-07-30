#pragma once

#include "figure/figure.h"

enum e_funeral_walker_action {
    ACTION_120_FUNERAL_CREATED = 120,
    ACTION_121_FUNERAL_GOING_TO_TOMB = 121,
    ACTION_122_FUNERAL_ARRIVED = 122,
    ACTION_123_FUNERAL_ABORT = 123, // path fail — do not set funeral_done
};

// One-shot funeral procession to a finished burial tomb.
class figure_funeral_walker : public figure_impl {
public:
    FIGURE_METAINFO(FIGURE_FUNERAL_WALKER, figure_funeral_walker)
    figure_funeral_walker(figure *f) : figure_impl(f) {}

    struct runtime_data_t {
        building_id target_tomb_id = 0;
    } FIGURE_RUNTIME_DATA_T;

    virtual void on_create() override;
    virtual void on_post_load() override;
    virtual void figure_action() override;
    virtual void figure_roaming_action() override { /* no home-return roam */ }
    virtual void update_animation() override;
    virtual sound_key phrase_key() const override;

    // Daily scan: spawn 1 walker per eligible finished tomb.
    // Returns first spawned figure id, or 0 if none.
    // force_ignore_road: tests may skip the road-access gate.
    static int try_spawn_all(bool force_ignore_road = false);

    static bool city_burial_provisions_complete();
    static bool is_burial_tomb(building &b);
    static bool tomb_needs_funeral(building &b);
    static bool tomb_has_active_funeral(building_id tomb_id);
    static tile2i tomb_destination_tile(building &tomb);
};
