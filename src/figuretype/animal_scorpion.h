#pragma once

#include "figure_animal.h"

enum e_scorpion_action : uint16_t {
    ACTION_8_SCORPION_RECALCULATE = 8,
    ACTION_9_SCORPION_CHASE_PREY = 9,
    ACTION_10_SCORPION_MOVING = 10,
    ACTION_12_SCORPION_INVESTIGATE = 12,
    ACTION_18_SCORPION_EATING = 18,
    ACTION_19_SCORPION_IDLE = 19,
    ACTION_20_SCORPION_ATTACK = 20,
    ACTION_21_SCORPION_SUCCESS_KILL = 21,
    ACTION_24_SCORPION_CREATED = 24,
    ACTION_25_SCORPION_LOOKING_FOR_ATTACK = 25,
    ACTION_196_SCORPION_AT_REST = 196,
};

class figure_scorpion : public figure_animal {
public:
    FIGURE_METAINFO(FIGURE_SCORPION, figure_scorpion)
    figure_scorpion(figure *f) : figure_animal(f) {}

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
ANK_CONFIG_STRUCT(figure_scorpion::static_params, max_hungry, max_hunting_distance, chase_speed_mult)

void figure_scorpion_setup_curse_raid(figure &f, int days);
