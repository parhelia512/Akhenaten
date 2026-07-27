#include "animal_asp.h"

#include "city/city.h"
#include "core/profiler.h"
#include "core/random.h"
#include "figure/formation.h"
#include "figure/figure.h"
#include "figuretype/figure_animal.h"
#include "figuretype/figure_soldier.h"
#include "grid/terrain.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_asp);

namespace {

bool is_asp_prey(figure *f, int self_id) {
    if (!f || !f->is_valid() || f->is_dead() || !f->type || f->id == self_id) {
        return false;
    }

    // Prefer city walkers; skip other predators / wildlife herds.
    switch (f->type) {
    case FIGURE_ASP:
    case FIGURE_SCORPION:
    case FIGURE_LION:
    case FIGURE_HYENA:
    case FIGURE_HIPPO:
    case FIGURE_CROCODILE:
    case FIGURE_ANTELOPE:
    case FIGURE_OSTRICH:
    case FIGURE_FROG:
    case FIGURE_LOCUST:
    case FIGURE_BIRDS:
    case FIGURE_EXPLOSION:
    case FIGURE_STANDARD_BEARER:
    case FIGURE_TRADE_SHIP:
    case FIGURE_FISHING_BOAT:
    case FIGURE_MAP_FLAG:
    case FIGURE_FLOTSAM:
    case FIGURE_SHIPWRECK:
    case FIGURE_INDIGENOUS_NATIVE:
    case FIGURE_TOWER_SENTRY:
    case FIGURE_NATIVE_TRADER:
    case FIGURE_ARROW:
    case FIGURE_JAVELIN:
    case FIGURE_BOLT:
    case FIGURE_BALLISTA:
    case FIGURE_CREATURE:
        return false;

    default:
        break;
    }

    if (f->is_enemy() || f->is_herd()) {
        return false;
    }
    if (f->category() == figure_category_animal) {
        return false;
    }
    if (::smart_cast<figure_soldier>(f) && f->action_state == ACTION_80_SOLDIER_AT_REST) {
        return false;
    }
    return true;
}

} // namespace

void figure_asp::on_create() {
    figure_impl::on_create();
    auto &d = runtime_data();
    const uint16_t max_h = current_params().max_hungry;
    d.hungry = max_h ? (rand() % max_h) : 0;
    base.allow_move_type = EMOVE_TERRAIN;
}

void figure_asp::on_post_load() {
    base.allow_move_type = EMOVE_TERRAIN;
}

void figure_asp::herd_rest() {
    advance_action(ACTION_196_ASP_AT_REST);
}

void figure_asp::herd_moved() {
    advance_action(ACTION_8_ASP_RECALCULATE);
}

void figure_asp::moveto(tile2i tile) {
    advance_action(ACTION_10_ASP_MOVING, tile);
}

int figure_asp::find_prey() {
    int min_figure_id = 0;
    int min_distance = 10000;
    const int max_distance = current_params().max_hunting_distance;

    for (figure *f : map_figures()) {
        if (!is_asp_prey(f, id())) {
            continue;
        }
        int distance = calc_maximum_distance(tile(), f->tile);
        if (f->targeted_by_figure_id) {
            distance *= 2;
        }
        if (distance < min_distance) {
            min_distance = distance;
            min_figure_id = f->id;
        }
    }

    if (min_distance <= max_distance && min_figure_id) {
        return min_figure_id;
    }
    return 0;
}

bool figure_asp::pick_roost_destination() {
    if (figure_herd_roost(&base, /*step*/4, /*bias*/8, /*max_dist*/32, TERRAIN_IMPASSABLE_OSTRICH)) {
        return true;
    }
    if (base.formation_id) {
        return false;
    }

    const formation *m = formation_get(base.formation_id);
    const tile2i roost_base = (m && m->in_use) ? m->tile : tile();
    tile2i dest = random_around_point(roost_base, tile(), /*step*/4, /*bias*/8, /*max_dist*/32);
    if (!dest.valid() || map_terrain_is(dest, TERRAIN_IMPASSABLE_OSTRICH)) {
        return false;
    }
    base.destination_tile = dest;
    return true;
}

void figure_asp::figure_action() {
    OZZY_PROFILER_FUNCTION();
    g_city.figures.add_animal();

    base.allow_move_type = EMOVE_TERRAIN;
    base.roam_wander_freely = false;
    base.speed_multiplier = current_params().speed_mult;
    auto &d = runtime_data();

    const formation *m = formation_get(base.formation_id);
    const tile2i roost_base = (m && m->in_use) ? m->tile : tile();

    figure *prey = base.target_figure_id ? figure_get(base.target_figure_id) : nullptr;
    const int maxdist = (base.target_figure_id && prey && !prey->is_dead())
        ? calc_maximum_distance(tile(), prey->tile)
        : 999;

    switch (action_state()) {
    case ACTION_24_ASP_CREATED:
    case ACTION_19_ASP_IDLE:
    case ACTION_196_ASP_AT_REST:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            advance_action(ACTION_8_ASP_RECALCULATE);
        }
        if (d.hungry <= 0) {
            advance_action(ACTION_25_ASP_LOOKING_FOR_ATTACK);
        }
        break;

    case ACTION_9_ASP_CHASE_PREY:
        base.speed_multiplier = current_params().chase_speed_mult;
        if (!base.target_figure_id || !prey || prey->is_dead() || !is_asp_prey(prey, id())) {
            base.target_figure_id = 0;
            return advance_action(ACTION_8_ASP_RECALCULATE);
        }

        if (maxdist == 0) {
            advance_action(ACTION_20_ASP_ATTACK);
            base.wait_ticks = 30 + (random_byte() % 20);
        } else {
            do_goto(prey->tile, TERRAIN_USAGE_ANIMAL, ACTION_25_ASP_LOOKING_FOR_ATTACK, ACTION_8_ASP_RECALCULATE);
            if (direction() == DIR_FIGURE_CAN_NOT_REACH || direction() == DIR_FIGURE_REROUTE) {
                base.direction = DIR_0_TOP_RIGHT;
                advance_action(ACTION_8_ASP_RECALCULATE);
            }
        }
        break;

    case ACTION_25_ASP_LOOKING_FOR_ATTACK: {
        int target_id = find_prey();
        base.target_figure_id = target_id;
        if (base.target_figure_id) {
            figure_get(base.target_figure_id)->targeted_by_figure_id = id();
            advance_action(ACTION_9_ASP_CHASE_PREY);
        } else {
            advance_action(ACTION_12_ASP_INVESTIGATE);
            base.destination_tile = random_around_point(roost_base, tile(), /*step*/4, /*bias*/8, /*max_dist*/32);
        }
        break;
    }

    case ACTION_12_ASP_INVESTIGATE:
        do_goto(base.destination_tile, TERRAIN_USAGE_ANIMAL, ACTION_8_ASP_RECALCULATE, ACTION_8_ASP_RECALCULATE);
        if (direction() == DIR_FIGURE_CAN_NOT_REACH || direction() == DIR_FIGURE_REROUTE) {
            base.direction = DIR_0_TOP_RIGHT;
            advance_action(ACTION_8_ASP_RECALCULATE);
        }
        break;

    case ACTION_18_ASP_EATING:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            if (d.hungry <= 0) {
                advance_action(ACTION_8_ASP_RECALCULATE);
            } else {
                route_remove();
                base.destination_tile = random_around_point(roost_base, roost_base, /*step*/4, /*bias*/4, /*max_dist*/0);
                base.direction = calc_general_direction(tile(), base.destination_tile);
                advance_action(ACTION_196_ASP_AT_REST);
            }
        }
        break;

    case ACTION_21_ASP_SUCCESS_KILL:
        base.wait_ticks--;
        if (base.wait_ticks <= 0) {
            advance_action(ACTION_18_ASP_EATING);
            base.wait_ticks = 30 + (random_byte() % 20);
        }
        break;

    case ACTION_20_ASP_ATTACK: {
        if (base.target_figure_id == INVALID_FIGURE_ID || !prey || prey->is_dead()) {
            return advance_action(ACTION_8_ASP_RECALCULATE);
        }
        base.direction = calc_general_direction_safe(base.tile, prey->tile);
        auto prey_impl = prey->dcast();
        if (prey_impl) {
            if (maxdist == 0) {
                prey_impl->on_attacked(&base);
                if (prey->is_dead()) {
                    base.target_figure_id = 0;
                    route_remove();
                    advance_action(ACTION_21_ASP_SUCCESS_KILL);
                    const uint16_t max_h = current_params().max_hungry;
                    d.hungry = max_h ? (rand() % max_h) : 0;
                    base.wait_ticks = 30 + (random_byte() % 20);
                } else {
                    base.wait_ticks = 10;
                }
            } else {
                base.wait_ticks = 12;
                advance_action(ACTION_9_ASP_CHASE_PREY);
            }
        }
        break;
    }

    case ACTION_8_ASP_RECALCULATE:
        base.wait_ticks--;
        if (d.hungry <= 0) {
            advance_action(ACTION_25_ASP_LOOKING_FOR_ATTACK);
            break;
        }

        if (base.wait_ticks <= 0) {
            if (pick_roost_destination()) {
                base.wait_ticks = 0;
                advance_action(ACTION_10_ASP_MOVING);
                const int arrive = (random_byte() & 1) ? ACTION_19_ASP_IDLE : ACTION_196_ASP_AT_REST;
                do_goto(base.destination_tile, TERRAIN_USAGE_ANIMAL, arrive, ACTION_8_ASP_RECALCULATE);
            } else {
                base.wait_ticks = 5;
            }
        }
        break;

    case ACTION_10_ASP_MOVING:
        if (do_goto(base.destination_tile, TERRAIN_USAGE_ANIMAL, ACTION_19_ASP_IDLE, ACTION_8_ASP_RECALCULATE)) {
            if (random_byte() & 1) {
                advance_action(ACTION_196_ASP_AT_REST);
            }
            base.wait_ticks = 50;
        }
        break;

    default:
        advance_action(ACTION_8_ASP_RECALCULATE);
        break;
    }
}

void figure_asp::update_animation() {
    xstring anim_key = animkeys().walk;
    switch (action_state()) {
    case ACTION_8_ASP_RECALCULATE:
    case ACTION_19_ASP_IDLE:
    case ACTION_24_ASP_CREATED:
    case ACTION_196_ASP_AT_REST:
        anim_key = animkeys().idle;
        break;

    case ACTION_18_ASP_EATING:
        anim_key = animkeys().eating;
        break;

    case ACTION_12_ASP_INVESTIGATE:
    case ACTION_10_ASP_MOVING:
    case ACTION_9_ASP_CHASE_PREY:
        anim_key = animkeys().walk;
        break;

    case FIGURE_ACTION_149_CORPSE:
        // Group 3 is a short non-8-dir strip — use it as lying death.
        anim_key = animkeys().death;
        break;

    case ACTION_20_ASP_ATTACK:
    case ACTION_21_ASP_SUCCESS_KILL:
        anim_key = animkeys().attack;
        break;

    default:
        anim_key = animkeys().idle;
        break;
    }

    image_set_animation(anim_key);
}

void figure_asp::update_day() {
    figure_impl::update_day();

    auto &d = runtime_data();
    if (d.hungry > 0) {
        d.hungry--;
    }
}
