#include "building/construction_blessing.h"

#include "building/monument_mastaba.h"
#include "city/city_buildings.h"
#include "city/city_figures.h"
#include "city/city_message.h"
#include "core/custom_span.hpp"
#include "core/hvector.h"
#include "core/random.h"
#include "figure/figure.h"
#include "game/game_config.h"
#include "game/resource.h"
#include "grid/grid.h"

#include <algorithm>

namespace {

bool is_small_mastaba_family(e_building_type type) {
    return building_type_any_of(type,
        {BUILDING_SMALL_MASTABA, BUILDING_SMALL_MASTABA_SIDE, BUILDING_SMALL_MASTABA_WALL,
         BUILDING_SMALL_MASTABA_ENTRANCE});
}

void set_phase_all_parts(building_monument &main, int phase) {
    building *part = &main.base;
    while (part) {
        if (auto *monument = part->dcast_monument()) {
            monument->set_phase(phase);
        }
        part = part->has_next() ? part->next() : nullptr;
    }
}

void zero_footprint_progress(building_monument &main) {
    building *part = &main.base;
    while (part) {
        grid_tiles tiles = map_grid_get_tiles(part, 0);
        for (auto &tile : tiles) {
            map_monuments_set_progress(tile, 0);
        }
        part = part->has_next() ? part->next() : nullptr;
    }
}

void sync_parts_to_main_phase(building_monument &main) {
    const int main_phase = (int)main.runtime_data().phase;
    building *part = main.base.next();
    while (part) {
        if (auto *monument = part->dcast_monument()) {
            if ((int)monument->runtime_data().phase != main_phase) {
                monument->set_phase(main_phase);
            }
        }
        part = part->has_next() ? part->next() : nullptr;
    }
}

bool part_id_matches(const hvector<int, 64> &part_ids, int dest_id) {
    if (dest_id <= 0) {
        return false;
    }
    for (int id : part_ids) {
        if (id == dest_id) {
            return true;
        }
    }
    return false;
}

void abort_sled_cargo(figure &f) {
    f.set_resource(RESOURCE_NONE);
    f.poof();
}

} // namespace

int construction_blessing_cap_phase(const building_monument &monument) {
    const int total = monument.phases();
    return std::max(static_cast<int>(CONSTRUCTION_BLESSING_FIRST_MASONRY_PHASE), (total * 3) / 4);
}

bool is_construction_blessing_monument(building &b) {
    if (!building_monument_is_alive(b) || !b.is_main() || !is_small_mastaba_family(b.type)) {
        return false;
    }

    auto *monument = b.dcast_monument();
    if (!monument || !monument->is_unfinished()) {
        return false;
    }

    const int phase = (int)monument->runtime_data().phase;
    const int cap = construction_blessing_cap_phase(*monument);
    if (phase >= cap) {
        return false;
    }

    // Cannot bump without calling set_phase(phases()) → FINISHED.
    return phase + 1 < monument->phases();
}

building_monument *find_construction_blessing_target() {
    building_monument *best = nullptr;
    int best_id = 0x7fffffff;

    // Do not use buildings_valid_do: halted (MOTHBALLED) must still be found.
    for (auto &b : city_buildings()) {
        if (!is_construction_blessing_monument(b)) {
            continue;
        }
        if (b.id >= best_id) {
            continue;
        }
        best_id = b.id;
        best = b.dcast_monument();
    }

    return best;
}

void clear_deliveries_for_chain(building_monument &main) {
    // Heap-backed after stack fill — safe for medium/large mastaba chains.
    hvector<int, 64> part_ids;
    building *part = &main.base;
    while (part) {
        part_ids.push_back(part->id);
        building_monument_remove_all_deliveries(part->id);
        part = part->has_next() ? part->next() : nullptr;
    }

    hvector<int, 32> poofed_pullers;
    const auto sled_types = make_array(FIGURE_SLED, FIGURE_SLED_PULLER);

    // Pass 1: anyone already targeted at a chain part.
    figure_valid_do([&](figure &f) {
        if (!part_id_matches(part_ids, f.destination_building_id)) {
            return;
        }
        if (f.type == FIGURE_SLED_PULLER) {
            poofed_pullers.push_back(f.id);
        }
        abort_sled_cargo(f);
    }, sled_types);

    // Pass 2: sleds that only follow a puller (dest unset/stale) whose leader we just aborted.
    figure_valid_do([&](figure &f) {
        if (f.type != FIGURE_SLED || f.leading_figure_id <= 0) {
            return;
        }
        for (int puller_id : poofed_pullers) {
            if (f.leading_figure_id == puller_id) {
                abort_sled_cargo(f);
                return;
            }
        }
    }, make_array(FIGURE_SLED));
}

bool apply_construction_blessing(building_monument &main, int budget) {
    const int cap = construction_blessing_cap_phase(main);
    int phase = (int)main.runtime_data().phase;
    if (budget <= 0 || phase >= cap || phase + 1 >= main.phases()) {
        return false;
    }

    sync_parts_to_main_phase(main);

    const vec2i init_tiles = get_mastaba_params(BUILDING_SMALL_MASTABA).init_tiles;
    bool worked = false;

    while (budget > 0 && phase < cap) {
        const int next_phase = phase + 1;
        if (next_phase >= main.phases()) {
            break;
        }

        const bool was_site_prep = phase < CONSTRUCTION_BLESSING_FIRST_MASONRY_PHASE;
        const int old_phase = phase;

        // Clear only once a bump is guaranteed (invariant: false ⇒ deliveries intact).
        if (!worked) {
            clear_deliveries_for_chain(main);
        }

        zero_footprint_progress(main);
        if (old_phase >= CONSTRUCTION_BLESSING_FIRST_MASONRY_PHASE) {
            building_mastaba::update_images(&main.base, old_phase, init_tiles);
        }

        set_phase_all_parts(main, next_phase);

        worked = true;
        budget--;
        phase = (int)main.runtime_data().phase;

        if (was_site_prep) {
            break;
        }
    }

    return worked;
}

bool maybe_construction_blessing(e_god god, bool major, bool force_construction) {
    if (!game_features::gameopt_pyramid_speedup.to_bool()) {
        return false;
    }

    building_monument *target = find_construction_blessing_target();
    if (!target) {
        return false;
    }

    if (!force_construction && !anti_scum_random_bool()) {
        return false;
    }

    const int budget = major ? CONSTRUCTION_BLESSING_BUDGET_MAJOR : CONSTRUCTION_BLESSING_BUDGET_MINOR;
    if (!apply_construction_blessing(*target, budget)) {
        return false;
    }

    messages::god(god, major ? "message_construction_blessing" : "message_construction_blessing_minor");
    return true;
}
