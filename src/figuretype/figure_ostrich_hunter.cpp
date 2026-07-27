#include "figure_ostrich_hunter.h"

#include "city/city.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_ostrich_hunter)

sound_key figure_ostrich_hunter::phrase_key() const {
    if (action_state(ACTION_16_OSTRICH_HUNTER_INVESTIGATE, ACTION_9_OSTRICH_HUNTER_CHASE_PREY, ACTION_15_OSTRICH_HUNTER_HUNT)) {
        svector<sound_key, 10> keys{"hunter_hunting",
                                   "hunter_test_1",
                                   "hunter_test_2",
                                   "hunter_test_3"};
        int index = rand() % keys.size();
        return keys[index];
    } else if (action_state() == ACTION_8_RECALCULATE) {
        if (g_city.sentiment.value > 40) {
            return "hunter_city_is_good";
        }
    }

    return "hunter_back";
}
