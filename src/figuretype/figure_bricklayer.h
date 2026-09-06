#pragma once

#include "figure/figure.h"

enum e_bricklayer_action : uint16_t {
    ACTION_0_BRICKLAYER_CREATED = 0,
    ACTION_1_BRICKLAYER_GOING = 1,
    ACTION_2_BRICKLAYER_GOING_TO_PLACE = 2,
    ACTION_3_BRICKLAYER_WAITING_RESOURCES = 3,
    ACTION_4_BRICKLAYER_LAY_BRICKS = 4,
    ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE = 5,
    ACTION_6_BRICKLAYER_RETURN_HOME = 6,
    ACTION_7_BRICKLAYER_EXIT_FROM_MONUMENT = 7,
    ACTION_8_BRICKLAYER_RANDOM_TILE = 8,
    ACTION_9_BRICKLAYER_DESTROY = 9,
    ACTION_10_BRICKLAYER_CREATED_ROAMING = 10,
    ACTION_11_BRICKLAYER_GOING_TO_STATUE = 11,
    ACTION_12_BRICKLAYER_WORK_STATUE = 12,

    ACTION_13_BRICKLAYER_MAX
};
using figure_bricklayer_action_tokens_t = token_holder<e_bricklayer_action, ACTION_0_BRICKLAYER_CREATED, ACTION_13_BRICKLAYER_MAX>;
extern const figure_bricklayer_action_tokens_t figure_bricklayer_action_tokens;

class figure_bricklayer : public figure_impl {
public:
    FIGURE_METAINFO(FIGURE_BRICKLAYER, figure_bricklayer)
    figure_bricklayer(figure *f) : figure_impl(f) {}

    struct runtime_data_t {
        short idle_wait_count;
        building_id destination_bid;
    } FIGURE_RUNTIME_DATA_T;

    virtual void on_create() override {}
    virtual void figure_action() override;
    virtual void on_destroy() override;
    virtual void update_animation() override;
};
