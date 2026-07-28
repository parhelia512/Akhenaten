#pragma once

#include "figure_animal.h"

enum e_asp_action : uint16_t {
    ACTION_8_ASP_RECALCULATE = 8,
    ACTION_9_ASP_CHASE_PREY = 9,
    ACTION_10_ASP_MOVING = 10,
    ACTION_12_ASP_INVESTIGATE = 12,
    ACTION_18_ASP_EATING = 18,
    ACTION_19_ASP_IDLE = 19,
    ACTION_20_ASP_ATTACK = 20,
    ACTION_21_ASP_SUCCESS_KILL = 21,
    ACTION_24_ASP_CREATED = 24,
    ACTION_25_ASP_LOOKING_FOR_ATTACK = 25,
    ACTION_196_ASP_AT_REST = 196,
};

class figure_asp : public figure_animal {
public:
    FIGURE_METAINFO(FIGURE_ASP, figure_asp)
    figure_asp(figure *f) : figure_animal(f) {}

    struct static_params : public figure_static_params {
        uint16_t max_hungry;
        uint16_t max_hunting_distance;
        uint8_t chase_speed_mult;
    } FIGURE_STATIC_DATA_T;

    struct runtime_data_t {
        short hungry;
        uint8_t curse_raid;
        uint16_t raid_days_left;
    } FIGURE_RUNTIME_DATA_T;

    virtual void on_create() override;
    virtual void on_post_load() override;
    virtual void before_poof() override;
    virtual void figure_action() override;
    virtual void update_animation() override;
    virtual void update_day() override;

    virtual void herd_moved() override;
    virtual void herd_rest() override;
    virtual void moveto(tile2i tile) override;

    virtual e_minimap_figure_color minimap_color() const override { return FIGURE_COLOR_ANIMAL; }

    void setup_curse_raid(int days);
    bool is_curse_raid() const;

private:
    void release_target();
    int find_prey();
    bool pick_roost_destination();
};
ANK_CONFIG_STRUCT(figure_asp::static_params, max_hungry, max_hunting_distance, chase_speed_mult)

void figure_asp_setup_curse_raid(figure &f, int days);
