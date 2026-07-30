#include "figure_zookeeper.h"

#include "building/building_entertainment.h"
#include "building/building_house.h"
#include "city/city.h"
#include "figure/service.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_zookeeper);

void figure_zookeeper::update_shows() {
    building *dest = destination();
    if (!dest) {
        return;
    }

    building_entertainment *entertainment = dest->dcast_entertainment();
    if (!entertainment) {
        return;
    }

    // Reuse juggler_visited as "animals present / show active" timer (same pattern as senet).
    entertainment->runtime_data().juggler_visited = 32;
}

sound_key figure_zookeeper::phrase_key() const {
    svector<sound_key, 10> keys;

    if (g_city.figures.total_invading_enemies() > 0) {
        return "zookeeper_defenses_are_weak";
    }

    int houses_in_disease = 0;
    buildings_house_do([&](auto house) {
        if (house->house_population() <= 0) {
            return;
        }
        if (house->base.disease_days > 0) {
            houses_in_disease = 1;
        }
    });
    if (houses_in_disease > 0) {
        return "zookeeper_danger_of_plague";
    }

    const int sentiment = g_city.sentiment.value;
    if (sentiment < 30) {
        keys.push_back("zookeeper_reputation_is_low");
    }

    if (g_city.sentiment.low_mood_cause == LOW_MOOD_NO_JOBS) {
        keys.push_back("zookeeper_high_unemployment");
    }

    if (g_city.sentiment.low_mood_cause == LOW_MOOD_NO_FOOD) {
        keys.push_back("zookeeper_no_food_in_city");
    }

    if (g_city.labor.workers_needed > 10) {
        keys.push_back("zookeeper_need_more_workers");
    }

    if (g_city.religion.least_mood() <= GOD_MOOD_INDIFIRENT) {
        keys.push_back("zookeeper_gods_are_angry");
    }

    if (g_city.festival.entertainment_is_low()) {
        keys.push_back("zookeeper_low_entertainment");
    }

    if (sentiment > 40) {
        keys.push_back("zookeeper_city_is_ok");
    }

    if (sentiment > 90) {
        keys.push_back("zookeeper_city_is_amazing");
    }

    if (keys.empty()) {
        return "zookeeper_city_is_ok";
    }

    return keys[rand() % keys.size()];
}

int figure_zookeeper::provide_service() {
    return figure_provide_culture(tile(), &base, [](building *b, figure * /*f*/) {
        auto house = b->dcast_house();
        if (house) {
            house->runtime_data().zookeeper = MAX_COVERAGE;
        }
    });
}

figure_sound_t figure_zookeeper::get_sound_reaction(xstring key) const {
    return current_params().sounds[key];
}
