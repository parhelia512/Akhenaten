#include "figure_birds_hunter.h"

#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_birds_hunter)

sound_key figure_birds_hunter::phrase_key() const {
    if (action_state(ACTION_16_OSTRICH_HUNTER_INVESTIGATE, ACTION_9_OSTRICH_HUNTER_CHASE_PREY, ACTION_15_OSTRICH_HUNTER_HUNT)) {
        return "hunt_bird_birds_are_wily";
    }

    return "hunt_bird_birds_ready_for_roasting";
}
