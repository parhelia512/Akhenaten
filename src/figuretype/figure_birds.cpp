#include "figure_birds.h"

#include "figure/figure.h"
#include "figure/formation.h"
#include "figuretype/figure_animal.h"
#include "city/city.h"
#include "grid/terrain.h"
#include "grid/figure.h"
#include "core/random.h"
#include "sound/sound.h"
#include "graphics/animation.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_birds);

void figure_birds::figure_action() {
    const formation *m = formation_get(base.formation_id);
    g_city.figures.add_animal();

    switch (base.action_state) {
    case ACTION_24_BIRDS_SPAWNED:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            advance_action(ACTION_8_BIRDS_RECALCULATE);
        }
        break;

    case ACTION_8_BIRDS_RECALCULATE:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            const int research_radius = (m && m->reseach_radius > 0) ? m->reseach_radius : 16;
            if (figure_herd_roost(&base, /*step*/4, /*bias*/8, /*max_dist*/research_radius, TERRAIN_IMPASSABLE_OSTRICH)) {
                base.wait_ticks = 0;
                advance_action(ACTION_10_BIRDS_GOING);
            } else {
                base.wait_ticks = 5;
            }
        }
        break;

    case ACTION_16_BIRDS_FLEEING:
        // BH0b: rewrite like ostrich (roost → ACTION_10). Left as-is for v1.
        if (do_goto(base.destination_tile, TERRAIN_USAGE_ANIMAL, ACTION_18_BIRDS_ROOSTING + (random_byte() & 0x1), ACTION_8_BIRDS_RECALCULATE)) {
            if (map_has_figure_but(base.destination_tile, id())) {
                base.wait_ticks = 1;
                advance_action(ACTION_8_BIRDS_RECALCULATE);
            } else {
                base.wait_ticks = 50;
            }
        }
        break;

    case ACTION_10_BIRDS_GOING:
        if (do_goto(base.destination_tile, TERRAIN_USAGE_ANIMAL, ACTION_18_BIRDS_ROOSTING + (random_byte() & 0x1), ACTION_8_BIRDS_RECALCULATE)) {
            if (map_has_figure_but(base.destination_tile, id())) {
                base.wait_ticks = 1;
                advance_action(ACTION_8_BIRDS_RECALCULATE);
            } else {
                base.wait_ticks = 50;
            }
        }
        break;

    case ACTION_15_BIRDS_TERRIFIED:
    case ACTION_18_BIRDS_ROOSTING:
    case ACTION_19_BIRDS_IDLE:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            advance_action(ACTION_8_BIRDS_RECALCULATE);
        }
        break;

    default:
        advance_action(ACTION_8_BIRDS_RECALCULATE);
        break;
    }
}

void figure_birds::update_animation() {
    switch (action_state()) {
    case ACTION_8_BIRDS_RECALCULATE:
    case ACTION_19_BIRDS_IDLE:
        image_set_animation(animkeys().idle);
        break;

    case ACTION_18_BIRDS_ROOSTING:
        image_set_animation(animkeys().eating);
        break;

    case ACTION_16_BIRDS_FLEEING:
    case ACTION_10_BIRDS_GOING:
        image_set_animation(animkeys().walk);
        break;

    case ACTION_15_BIRDS_TERRIFIED:
        image_set_animation(animkeys().idle);
        base.animctx.frame = 0;
        break;

    case FIGURE_ACTION_149_CORPSE:
        image_set_animation(animkeys().death);
        break;

    default:
        image_set_animation(animkeys().eating);
        break;
    }
}

void figure_birds::before_poof() {
}

bool figure_birds::play_die_sound() {
    g_sound.play_effect(SOUND_EFFECT_OSTRICH_DIE);
    return true;
}

sound_key figure_birds::phrase_key() const {
    return "no_phrase";
}
