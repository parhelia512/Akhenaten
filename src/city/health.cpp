#include "health.h"

#include "building/building.h"
#include "building/house.h"
#include "building/destruction.h"
#include "city/data_private.h"
#include "city/message.h"
#include "core/calc.h"
#include "core/random.h"
#include "game/tutorial.h"
#include "scenario/property.h"

int city_health() {
    return city_data.health.value;
}

void city_health_change(int amount) {
    city_data.health.value = calc_bound(city_data.health.value + amount, 0, 100);
}

void city_health_start_disease(int total_people, bool force, int plague_people) {
    if (!force && city_data.health.value >= 40) {
        return;
    }

    int chance_value = random_byte() & 0x3f;
    if (city_data.religion.bast_curse_active) {
        // force plague
        chance_value = 0;
        city_data.religion.bast_curse_active = false;
    }

    if (!force && (chance_value > 40 - city_data.health.value)) {
        return;
    }

    int sick_people = calc_adjust_with_percentage(total_people, 7 + (random_byte() & 3));
    if (plague_people > 0) {
        sick_people = plague_people;
    }

    if (sick_people <= 0) {
        return;
    }

    city_health_change(10);
    int people_to_plague = sick_people - city_data.health.num_mortuary_workers;
    if (people_to_plague <= 0) {
        city_message_post_with_popup_delay(MESSAGE_CAT_HEALTH_PROBLEM, MESSAGE_HEALTH_MALARIA_PROBLEM, 0, 0);
        return;
    }

    tutorial_on_disease();

    // kill people where has little common_health
    building *warn_building = nullptr;
    buildings_valid_do([&] (building &b) {
        if (people_to_plague <= 0 || !b.house_size || !b.house_population) {
            return;
        }

        if (b.common_health < 10) {
            warn_building = &b;
            people_to_plague -= b.house_population;
            building_mark_plague(&b);
        }
    });

    // kill people who don't have access to apothecary/physician
    buildings_valid_do([&] (building &b) {
        if (people_to_plague <= 0 || !b.house_size || !b.house_population) {
            return;
        }

        if (!(b.data.house.apothecary || b.data.house.physician)) {
            warn_building = &b;
            people_to_plague -= b.house_population;
            building_mark_plague(&b);
        }
    });

    // kill people in tents
    buildings_valid_do([&] (building &b) {
        if (people_to_plague <= 0 || !b.house_size || !b.house_population) {
            return;
        }

        if (b.subtype.house_level <= HOUSE_STURDY_HUT) {
            warn_building = &b;
            people_to_plague -= b.house_population;
            building_mark_plague(&b);
        }
    });

    // kill anyone
    buildings_valid_do([&] (building &b) {
        if (people_to_plague <= 0 || !b.house_size || !b.house_population) {
            return;
        }

        warn_building = &b;
        people_to_plague -= b.house_population;
        building_mark_plague(&b);
    });

    e_building_type btype = (warn_building ? warn_building->type : BUILDING_NONE);
    int grid_offset = (warn_building ? warn_building->tile.grid_offset() : 0);
    if (city_data.health.num_mortuary_workers > 0) {
        city_message_post_with_popup_delay(MESSAGE_CAT_HEALTH_PROBLEM, MESSAGE_HEALTH_DISEASE, btype, grid_offset);
    } else {
        city_message_post_with_popup_delay(MESSAGE_CAT_HEALTH_PROBLEM, MESSAGE_HEALTH_PLAGUE, btype, grid_offset);
    }
}

void city_health_update() {
    if (city_data.population.population < 200 || scenario_is_mission_rank(1) || scenario_is_mission_rank(2)) {
        city_data.health.value = 50;
        city_data.health.target_value = 50;
        return;
    }

    int total_population = 0;
    int healthy_population = 0;
    buildings_valid_do([&] (building &b) {
        if (!b.house_size || !b.house_population) {
            return;
        }

        total_population += b.house_population;
        if (b.subtype.house_level <= HOUSE_STURDY_HUT) {
            if (b.data.house.apothecary) {
                healthy_population += b.house_population;
            } else {
                healthy_population += b.house_population / 4;
            }
        } else if (b.data.house.physician) {
            if (b.house_days_without_food == 0) {
                healthy_population += b.house_population;
            } else {
                healthy_population += b.house_population / 4;
            }
        } else if (b.house_days_without_food == 0) {
            healthy_population += b.house_population / 4;
        }
    });

    city_data.health.target_value = calc_percentage(healthy_population, total_population);
    if (city_data.health.value < city_data.health.target_value) {
        city_data.health.value += 2;
        if (city_data.health.value > city_data.health.target_value) {
            city_data.health.value = city_data.health.target_value;
        }

    } else if (city_data.health.value > city_data.health.target_value) {
        city_data.health.value -= 2;
        if (city_data.health.value < city_data.health.target_value) {
            city_data.health.value = city_data.health.target_value;
        }
    }
    city_data.health.value = calc_bound(city_data.health.value, 0, 100);

    city_health_start_disease(total_population, false, 0);
}

void city_health_reset_mortuary_workers(void) {
    city_data.health.num_mortuary_workers = 0;
}

void city_health_add_mortuary_workers(int amount) {
    city_data.health.num_mortuary_workers += amount;
}
