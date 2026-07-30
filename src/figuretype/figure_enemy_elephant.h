#pragma once

#include "figuretype/figure_enemy_fast_sword.h"

// Egyptian war elephant (ES4). Reuses fast-sword formation/melee AI; dedicated type
// so HP/speed/AoE can diverge. March SFX skipped in figure_enemy_fast_sword
// (camel/elephant filter). Full trampling AoE is a follow-up.
class figure_enemy_elephant : public figure_enemy_fast_sword {
public:
    figure_enemy_elephant(figure *f) : figure_enemy_fast_sword(f) {}
};

class figure_egyptian_elephant : public figure_enemy_elephant {
public:
    FIGURE_METAINFO(FIGURE_ENEMY_EGYPTIAN_ELEPHANT, figure_egyptian_elephant)
    figure_egyptian_elephant(figure *f) : figure_enemy_elephant(f) {}

    struct static_params : public base_params_t, public figure_static_params {
    } FIGURE_STATIC_DATA_T;

    virtual int8_t interval_attack_delay() const override { return current_params().interval_attack_delay; }
};
ANK_CONFIG_STRUCT(figure_egyptian_elephant::static_params, interval_attack_delay)
