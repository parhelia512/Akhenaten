#include "figure_antelope_hunter.h"

#include "city/city.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_antelope_hunter)

sound_key figure_antelope_hunter::phrase_key() const {
    if (action_state(ACTION_16_OSTRICH_HUNTER_INVESTIGATE, ACTION_9_OSTRICH_HUNTER_CHASE_PREY, ACTION_15_OSTRICH_HUNTER_HUNT)) {
        return "antelope_hunter_hunting";
    } else if (action_state() == ACTION_8_RECALCULATE) {
        if (g_city.sentiment.value > 40) {
            return "antelope_hunter_city_is_good";
        }
    }

    return "antelope_hunter_back";
}
