#pragma once

#include "figure/figure.h"

enum e_frog_action {
    ACTION_120_FROG_CREATED = 120,
    ACTION_121_FROG_ROAMING = 121,
};

// Cleopatra plague vermin (CF1). SprMain2 bmp "frog" g22–26.
// Ptah perform_frogs / EVENT_TYPE_FROGS — house infest-on-pass, not predator.
class figure_frog : public figure_impl {
public:
    FIGURE_METAINFO(FIGURE_FROG, figure_frog)
    figure_frog(figure *f) : figure_impl(f) {}

    struct static_params : public figure_static_params {
        uint8_t default_swarm = 10;
        uint8_t max_amount = 24;
        uint16_t plague_days = 80;      // TEMP ~5 months × 16 days
        uint8_t house_infest_days = 80; // TEMP lockout; community: UI ~5 mo, often shorter
        int8_t happiness_hit = -10;     // Major Plagues; mirror locust until exe dump
    } FIGURE_STATIC_DATA_T;

    struct runtime_data_t {
        uint16_t days_left = 0;
    } FIGURE_RUNTIME_DATA_T;

    virtual void on_create() override;
    virtual void figure_action() override;
    virtual void figure_roaming_action() override { /* free roam; no home return */ }
    virtual void update_animation() override;
    virtual void update_day() override;
    virtual sound_key phrase_key() const override;
    virtual e_minimap_figure_color minimap_color() const override { return FIGURE_COLOR_ANIMAL; }

    // Spawn N frogs from map borders. count<=0 → default. Returns first id, or 0.
    static int spawn_swarm(int count = 0);

    // Spawn + happiness + ambient. No popup (callers add message).
    static void apply_plague(int swarm_count = 0);

    // Mark house frog-infested: evict residents, set lockout timer.
    static void infest_house(building &b);
};
ANK_CONFIG_STRUCT(figure_frog::static_params,
    default_swarm, max_amount, plague_days, house_infest_days, happiness_hit)
