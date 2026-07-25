#pragma once

#include "figure_animal.h"
#include "core/tokenum.h"

enum e_ostrich_action {
    ACTION_0_OSTRICH_NONE = 0,
    ACTION_8_OSTRICH_RECALCULATE = 8,
    ACTION_10_OSTRICH_GOING = 10,
    ACTION_15_OSTRICH_TERRIFIED = 15,
    ACTION_16_OSTRICH_FLEEING = 16,
    ACTION_18_OSTRICH_ROOSTING = 18,
    ACTION_19_OSTRICH_IDLE = 19,
    ACTION_24_OSTRICH_SPAWNED = 24,
    ACTION_196_OSTRICH_AT_REST = 196,

    ACTION_197_OSTRICH_MAX
};
using e_ostrich_action_tokens_t = token_holder<e_ostrich_action, ACTION_0_OSTRICH_NONE, ACTION_197_OSTRICH_MAX>;
extern const e_ostrich_action_tokens_t e_ostrich_action_tokens;

/**
 * @class figure_ostrich
 * @brief Represents an ostrich animal figure in the game world.
 *
 * This class implements the behavior of wild ostriches that roam the map.
 * Ostriches can wander, roost, eat, and flee when threatened or damaged by arrows.
 * They use pathfinding to navigate around impassable terrain and avoid obstacles.
 *
 * Behavioral states include:
 * - Idle and roosting (resting/eating)
 * - Moving between locations
 * - Fleeing when hit by projectiles
 * - Terrified state when threatened
 *
 * Ostriches can be hunted by ostrich hunters for resource RESOURCE_GAMEMEAT.
 */
class figure_ostrich : public figure_animal {
public:
    FIGURE_METAINFO(FIGURE_OSTRICH, figure_ostrich)
    figure_ostrich(figure *f) : figure_animal(f) {}
    virtual figure_ostrich *dcast_ostrich() override { return this; }

    struct static_params : public figure_static_params {
        uint8_t scared_ticks;

        void archive_init();
    } FIGURE_STATIC_DATA_T;

    struct runtime_data_t {
        int8_t applied_damage;
        uint8_t scared_ticks;
    } FIGURE_RUNTIME_DATA_T;

    virtual void figure_action() override;
    virtual void update_animation() override;
    virtual void before_poof() override;
    virtual bool play_die_sound() override;
    virtual void apply_damage(int hit_dmg, figure_id attacker_id) override;

    virtual void herd_moved() override;
    virtual void herd_rest() override;
    virtual void herd_scare() override;
    virtual void moveto(tile2i tile) override;

    virtual e_minimap_figure_color minimap_color() const override { return FIGURE_COLOR_ANIMAL; }
};
ANK_CONFIG_STRUCT(figure_ostrich::static_params, scared_ticks)