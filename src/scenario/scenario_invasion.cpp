#include "scenario_invasion.h"

#include "building/destruction.h"
#include "city/city.h"
#include "city/city_warnings.h"
#include "game/game_events.h"
#include "city/city_message.h"
#include "core/calc.h"
#include "core/random.h"
#include "empire/empire_object.h"
#include "figure/figure.h"
#include "figure/formation.h"
#include "figure/figure_names.h"
#include "figuretype/figure_enemy.h"
#include "game/difficulty.h"
#include "game/game.h"
#include "grid/grid.h"
#include "grid/terrain.h"
#include "scenario/map.h"
#include "scenario/scenario.h"
#include "dev/debug.h"
#include "core/log.h"
#include "empire/empire.h"
#include "js/js_game.h"
#include "figure/enemy_army.h"
#include "scenario/scenario_event_manager.h"

const e_attack_faction_tokens_t ANK_CONFIG_ENUM(e_attack_faction_tokens);

e_formation_attack_type formation_attack_from_event_target(int invasion_attack_target) {
    // Pak EVENT_ATTACK_TARGET_* is 0..4; FORMATION_ATTACK_RANDOM is 5 (SIMPLE sits at 4).
    switch (invasion_attack_target) {
    case 0: // EVENT_ATTACK_TARGET_FOOD
        return FORMATION_ATTACK_FOOD_CHAIN;
    case 1: // EVENT_ATTACK_TARGET_VAULTS
        return FORMATION_ATTACK_GOLD_STORES;
    case 2: // EVENT_ATTACK_TARGET_BEST_BUILDINGS
        return FORMATION_ATTACK_BEST_BUILDINGS;
    case 3: // EVENT_ATTACK_TARGET_TROOPS
        return FORMATION_ATTACK_TROOPS;
    case 4: // EVENT_ATTACK_TARGET_RANDOM
    default:
        return FORMATION_ATTACK_RANDOM;
    }
}

declare_console_command_p(start_invasion) {
    invasion_opts_t opts;
    opts.mode = ATTACK_TYPE_ENEMIES;
    opts.enemy_type = (e_enemy_type)parse_integer_from<bstring32>(is); // 0 type, 1 kingdome, 2 seth natives
    opts.size = parse_integer_from<bstring32>(is);
    int tilex = parse_integer_from<bstring32>(is);
    int tiley = parse_integer_from<bstring32>(is);
    opts.invasion_point = { tilex, tiley };
    opts.invasion_id = 23;
    opts.want_destroy = parse_integer_from<bstring32>(is);

    scenario_invasion_start(opts);

    events::emit(event_city_warning{ "Started invasion" });
}

declare_console_command_p(start_invasion_fast) {
    invasion_opts_t opts;
    opts.mode = ATTACK_TYPE_ENEMIES;
    opts.enemy_type = ENEMY_0_BARBARIAN; // 0 type, 1 kingdome, 2 seth natives
    opts.size = 10;
    opts.attack_type = FORMATION_ATTACK_FOOD_CHAIN;
    opts.invasion_point = tile2i::invalid;
    opts.invasion_id = 23;
    opts.want_destroy = 5;
    tile2i tile = scenario_start_invasion_impl(opts);
    if (tile.valid()) {
        events::emit(event_message{ true, "message_barbarians_attack", g_invasions.last_internal_invasion_id, tile.grid_offset(), SOURCE_LOCATION });
    }
}

enemy_properties_t ANK_VARIABLE(enemy_barbarian);
enemy_properties_t ANK_VARIABLE(enemy_assyrian);
enemy_properties_t ANK_VARIABLE(enemy_canaanite);
enemy_properties_t ANK_VARIABLE(enemy_egyptian);
enemy_properties_t ANK_VARIABLE(enemy_hittite);
enemy_properties_t ANK_VARIABLE(enemy_hyksos);
enemy_properties_t ANK_VARIABLE(enemy_kushite);
enemy_properties_t ANK_VARIABLE(enemy_libian);
enemy_properties_t ANK_VARIABLE(enemy_nubian);
enemy_properties_t ANK_VARIABLE(enemy_persian);
enemy_properties_t ANK_VARIABLE(enemy_phoenician);
enemy_properties_t ANK_VARIABLE(enemy_roman);
enemy_properties_t ANK_VARIABLE(enemy_seapeople);

std::array<enemy_properties_t *, ENEMY_COUNT> g_enemy_properties = {
    &enemy_barbarian,
    &enemy_assyrian,
    &enemy_canaanite,
    &enemy_egyptian,
    &enemy_hittite,
    &enemy_hyksos,
    &enemy_kushite,
    &enemy_libian,
    &enemy_nubian,
    &enemy_persian,
    &enemy_phoenician,
    &enemy_roman,
    &enemy_seapeople,
};

const int LOCAL_UPRISING_NUM_ENEMIES[20] = {0, 0, 0, 0, 0, 3, 3, 3, 0, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 9};

invasion_warning_t g_invasion_warning;
invasion_data_t ANK_VARIABLE_N(g_invasions, "invasions");

void invasion_data_t::clear(void) {
    memset(warnings.data(), 0, warnings.size() * sizeof(invasion_warning_t));
    for (auto &p : event_pending) {
        p = {};
    }
}

int invasion_data_t::alloc_invasion_id() {
    // Prefer free slots with no live formations linked (formation_id == 0 after clear_formations
    // is not enough — check pending + whether any formation still references the id).
    for (int id = FIRST_EVENT_INVASION_SLOT; id < enemy_armies_t::MAX_ENEMY_ARMIES; ++id) {
        bool pending = false;
        for (const auto &p : event_pending) {
            if (p.in_use && p.invasion_id == (uint8_t)id) {
                pending = true;
                break;
            }
        }
        if (pending) {
            continue;
        }

        bool in_use = false;
        for (int fi = 1; fi < MAX_FORMATIONS; ++fi) {
            formation *m = formation_get(fi);
            if (m && m->in_use && !m->is_herd && !m->own_batalion && m->invasion_id == id) {
                in_use = true;
                break;
            }
        }
        if (!in_use) {
            return id;
        }
    }
    logs::warn("scenario_invasion: no free invasion_id slot (< %d)", enemy_armies_t::MAX_ENEMY_ARMIES);
    return -1;
}

bool invasion_data_t::register_event_pending(uint8_t invasion_id, int16_t event_id, uint8_t want_destroy) {
    if (invasion_id == 0 || event_id < 0) {
        return false;
    }
    for (auto &p : event_pending) {
        if (p.in_use) {
            continue;
        }
        p.in_use = true;
        p.enemies_seen = false;
        p.invasion_id = invasion_id;
        p.event_id = event_id;
        p.want_destroy = want_destroy;
        return true;
    }
    logs::warn("scenario_invasion: event_pending table full");
    return false;
}

bool invasion_data_t::has_pending_for_event(int16_t event_id) const {
    for (const auto &p : event_pending) {
        if (p.in_use && p.event_id == event_id) {
            return true;
        }
    }
    return false;
}

void invasion_data_t::process_event_resolutions() {
    for (auto &p : event_pending) {
        if (!p.in_use) {
            continue;
        }

        const int enemies = enemy_army_total_enemy_formations();
        // Count only formations for this invasion_id (global total can include other waves).
        int this_wave = 0;
        for (int fi = 1; fi < MAX_FORMATIONS; ++fi) {
            formation *m = formation_get(fi);
            if (m && m->in_use && !m->is_herd && !m->own_batalion && m->num_figures > 0
                && m->invasion_id == p.invasion_id) {
                this_wave++;
            }
        }

        if (this_wave > 0) {
            p.enemies_seen = true;
            continue;
        }
        if (!p.enemies_seen) {
            continue;
        }

        // Wave cleared after being seen — fire chain like Selima resolve.
        const int event_id = p.event_id;
        const uint8_t invasion_id = p.invasion_id;
        p = {};

        event_ph_t *event = g_scenario.events.at(event_id);
        if (!event || event->type != EVENT_TYPE_INVASION) {
            continue;
        }

        enemy_army *army = enemy_army_get_editable(invasion_id);
        const bool destroy_goal = army && army->buildings_to_destroy > 0
            && army->buildings_destroyed >= army->buildings_to_destroy;

        if (destroy_goal && event->on_refusal_action >= 0) {
            logs::info("akhenaten: invasion event %d id=%u destroy-goal → REFUSED", event_id, invasion_id);
            g_scenario.events.process_event(event->on_refusal_action, true, EVENT_ACTION_REFUSED, event_id);
        } else if (destroy_goal && event->on_defeat_action >= 0) {
            logs::info("akhenaten: invasion event %d id=%u destroy-goal → DEFEAT", event_id, invasion_id);
            g_scenario.events.process_event(event->on_defeat_action, true, EVENT_ACTION_DEFEAT, event_id);
        } else if (event->on_completed_action >= 0) {
            logs::info("akhenaten: invasion event %d id=%u wiped → COMPLETED", event_id, invasion_id);
            g_scenario.events.process_event(event->on_completed_action, true, EVENT_ACTION_COMPLETED, event_id);
        }
        (void)enemies;
    }
}

void invasion_data_t::init() {
    clear();
    int path_current = 1;
    int path_max = g_empire.get_max_invasion_path();

    if (path_max == 0) {
        return;
    }

    for (int i = 0; i < MAX_INVASIONS; i++) {
        invasion_warning_t& warning = warnings[1];
        random_generate_next();
        if (!g_scenario.invasions[i].type) {
            continue;
        }

        g_scenario.invasions[i].month = 2 + (random_byte() & 7);
        if (g_scenario.invasions[i].type == INVASION_TYPE_LOCAL_UPRISING
            || g_scenario.invasions[i].type == INVASION_TYPE_DISTANT_BATTLE) {
            continue;
        }

        for (int year = 1; year < 8; year++) {
            const empire_object* obj = g_empire.get_battle_icon(path_current, year);
            if (!obj) {
                continue;
            }

            warning.in_use = 1;
            warning.invasion_path_id = obj->invasion_path_id;
            warning.warning_years = obj->invasion_years;
            warning.pos = obj->pos;
            warning.image_id = obj->image_id;
            warning.invasion_id = i;
            warning.empire_object_id = obj->id;
            warning.month_notified = 0;
            warning.year_notified = 0;
            warning.months_to_go = 12 * g_scenario.invasions[i].year;
            warning.months_to_go += g_scenario.invasions[i].month;
            warning.months_to_go -= 12 * year;
        }

        path_current++;
        if (path_current > path_max) {
            path_current = 1;
        }
    }
}

bool scenario_invasion_exists_upcoming() {
    for (const auto& warning : g_invasions.warnings) {
        if (warning.in_use && warning.handled)
            return true;
    }
    return false;
}

void scenario_invasion_foreach_warning(std::function<void(vec2i, int)> callback) {
    for (const auto& warning : g_invasions.warnings) {
        if (warning.in_use && warning.handled)
            callback(warning.pos, warning.image_id);
    }
}

int scenario_invasion_count() {
    int num_invasions = 0;
    for (int i = 0; i < MAX_INVASIONS; i++) {
        if (g_scenario.invasions[i].type)
            num_invasions++;
    }
    return num_invasions;
}

static void determine_formations(int num_soldiers, int* num_formations, int soldiers_per_formation[]) {
    if (num_soldiers > 0) {
        if (num_soldiers <= 16) {
            *num_formations = 1;
            soldiers_per_formation[0] = num_soldiers;
        } else if (num_soldiers <= 32) {
            *num_formations = 2;
            soldiers_per_formation[1] = num_soldiers / 2;
            soldiers_per_formation[0] = num_soldiers - num_soldiers / 2;
        } else {
            *num_formations = 3;
            soldiers_per_formation[2] = num_soldiers / 3;
            soldiers_per_formation[1] = num_soldiers / 3;
            soldiers_per_formation[0] = num_soldiers - 2 * (num_soldiers / 3);
        }
    }
}

tile2i scenario_start_invasion_impl(invasion_opts_t opts) {
    auto &data = g_invasions;
    if (opts.size <= 0) {
        return tile2i::invalid;
    }

    int formations_per_type[3];
    int soldiers_per_formation[3][4];

    opts.size = std::clamp<int>(difficulty_adjust_enemies(opts.size), data.min_invasion_amount, data.max_invasion_amount);

    data.last_internal_invasion_id++;
    if (data.last_internal_invasion_id > 32000) {
        data.last_internal_invasion_id = 1;
    }

    // calculate soldiers per type
    int num_type1 = calc_adjust_with_percentage(opts.size, g_enemy_properties[opts.enemy_type]->percentage_type1);
    int num_type2 = calc_adjust_with_percentage(opts.size, g_enemy_properties[opts.enemy_type]->percentage_type2);
    int num_type3 = calc_adjust_with_percentage(opts.size, g_enemy_properties[opts.enemy_type]->percentage_type3);
    num_type1 += opts.size - (num_type1 + num_type2 + num_type3); // assign leftovers to type1

    for (int t = 0; t < 3; t++) {
        formations_per_type[t] = 0;
        for (int f = 0; f < 4; f++) {
            soldiers_per_formation[t][f] = 0;
        }
    }

    // calculate number of formations
    determine_formations(num_type1, &formations_per_type[0], soldiers_per_formation[0]);
    determine_formations(num_type2, &formations_per_type[1], soldiers_per_formation[1]);
    determine_formations(num_type3, &formations_per_type[2], soldiers_per_formation[2]);

    // determine invasion point
    tile2i invasion_tile;
    if (opts.enemy_type == ENEMY_3_EGYPTIAN) {
        invasion_tile = scenario_map_entry();
    } else {
        if (!opts.invasion_point.valid()) {
            auto &lands = g_scenario.invasion_points_land;
            svector<tile2i, 8> points;
            std::copy_if(lands.begin(), lands.end(), std::back_inserter(points), [] (auto &p) { return p.valid(); });
            if (points.empty()) {
                logs::warn("scenario_invasion: no valid land invasion points, falling back to map exit");
                invasion_tile = tile2i::invalid;
            } else {
                invasion_tile = points.at(rand() % points.size());
            }
        } else {
            invasion_tile = opts.invasion_point;
        }
    }

    if (!invasion_tile.valid()) {
        invasion_tile = scenario_map_exit();
    }

    // determine orientation
    int orientation = DIR_4_BOTTOM_LEFT;
    
    // check terrain — try entry / a few edge candidates if the primary tile is blocked
    auto tile_ok = [] (tile2i t) {
        if (!t.valid()) {
            return false;
        }
        if (map_terrain_is(t, TERRAIN_ELEVATION | TERRAIN_ROCK | TERRAIN_TREE)) {
            return false;
        }
        if (map_terrain_is(t, TERRAIN_WATER) && !map_terrain_is(t, TERRAIN_ROAD)) {
            return false;
        }
        return true;
    };

    if (!tile_ok(invasion_tile)) {
        tile2i candidates[] = {
            scenario_map_entry(),
            scenario_map_exit(),
            tile2i(1, g_scenario.map.height / 2),
            tile2i(g_scenario.map.width - 2, g_scenario.map.height / 2),
            tile2i(g_scenario.map.width / 2, 1),
            tile2i(g_scenario.map.width / 2, g_scenario.map.height - 2),
        };
        invasion_tile = tile2i::invalid;
        for (tile2i c : candidates) {
            if (tile_ok(c)) {
                invasion_tile = c;
                break;
            }
        }
    }

    if (!invasion_tile.valid()) {
        return tile2i::invalid;
    }

    if (map_terrain_is(invasion_tile, TERRAIN_WATER)) {
        if (!map_terrain_is(invasion_tile, TERRAIN_ROAD)) { // bridge
            return tile2i::invalid;
        }
    } else if (map_terrain_is(invasion_tile, TERRAIN_BUILDING | TERRAIN_CANAL | TERRAIN_GATEHOUSE | TERRAIN_WALL)) {
        building *b = building_get(invasion_tile.grid_offset());
        if (b->id) {
            g_city.maintenance.destroy_by_enemy(b);
        }
    }

    // spawn the lot!
    int seq = 0;
    for (int type = 0; type < 3; type++) {
        if (formations_per_type[type] <= 0) {
            continue;
        }

        e_figure_type figure_type = g_enemy_properties[opts.enemy_type]->figure_types[type];
        if (figure_type == FIGURE_NONE) {
            logs::error("No figure type for %s enemy", e_enemy_type_tokens.name(opts.enemy_type));
            continue;
        }

        for (int i = 0; i < formations_per_type[type]; i++) {
            int formation_id = formation_create_enemy(figure_type,
                                                      invasion_tile,
                                                      g_enemy_properties[opts.enemy_type]->layout,
                                                      orientation,
                                                      opts.enemy_type,
                                                      opts.attack_type,
                                                      opts.invasion_id,
                                                      data.last_internal_invasion_id);
            if (formation_id <= 0) {
                continue;
            }

            for (int fig = 0; fig < soldiers_per_formation[type][i]; fig++) {
                figure* f = figure_create(figure_type, invasion_tile, orientation);
                f->faction_id = 0;
                f->action_state = ACTION_151_ENEMY_INITIAL;
                f->wait_ticks = 200 * seq + 10 * fig + 10;
                f->formation_id = formation_id;
                f->name = figure_name_get(figure_type);
            }
            seq++;
        }
    }

    // Apply destroy-goal to the army slot (JS want_destroy_buildings / event heuristic).
    // Without this, enemy_army_achieved_destroy_goal never returns true.
    if (opts.invasion_id > 0
        && opts.invasion_id < enemy_armies_t::MAX_ENEMY_ARMIES
        && seq > 0) {
        enemy_army *army = enemy_army_get_editable((uint8_t)opts.invasion_id);
        army->buildings_to_destroy = opts.want_destroy;
        army->buildings_destroyed = 0;
    }

    return invasion_tile;
}

void scenario_invasion_process() {
    e_enemy_type enemy_id = g_scenario.enemy_id;
    for (auto& warning : g_invasions.warnings) {
        if (!warning.in_use)
            continue;

        // update warnings
        warning.months_to_go--;
        if (warning.months_to_go <= 0) {
            if (warning.handled != 1) {
                warning.handled = 1;
                warning.year_notified = game.simtime.year;
                warning.month_notified = game.simtime.month;
                if (warning.warning_years > 2)
                    events::emit(event_message{ false, "message_distant_battle", 0, 0, SOURCE_LOCATION });
                else if (warning.warning_years > 1)
                    events::emit(event_message{ false, "message_enemies_closing", 0, 0, SOURCE_LOCATION });
                else {
                    events::emit(event_message{ false, "message_enemies_at_the_door", 0, 0, SOURCE_LOCATION });
                }
            }
        }

        if (game.simtime.year >= g_scenario.start_year + g_scenario.invasions[warning.invasion_id].year
            && game.simtime.month >= g_scenario.invasions[warning.invasion_id].month) {
            // invasion attack time has passed
            warning.in_use = 0;
            if (warning.warning_years > 1) {
                continue;
            }

            // enemy invasions
            if (g_scenario.invasions[warning.invasion_id].type == INVASION_TYPE_ENEMY_ARMY) {
                invasion_opts_t opts;
                opts.enemy_type = enemy_id;
                opts.size = g_scenario.invasions[warning.invasion_id].amount;
                // opts.invasion_point = g_scenario.invasions[warning.invasion_id].from;
                opts.attack_type = g_scenario.invasions[warning.invasion_id].attack_type;
                opts.invasion_id = warning.invasion_id;
                tile2i invasion_tile = scenario_start_invasion_impl(opts);
                if (invasion_tile.valid()) {
                    events::emit(event_message{ true, "message_barbarians_attack", g_invasions.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });
                }
            }
            if (g_scenario.invasions[warning.invasion_id].type == INVASION_TYPE_KNGDOME) {
                invasion_opts_t opts;
                opts.enemy_type = ENEMY_3_EGYPTIAN;
                opts.size = g_scenario.invasions[warning.invasion_id].amount;
                // opts.invasion_point = g_scenario.invasions[warning.invasion_id].from;
                opts.attack_type = g_scenario.invasions[warning.invasion_id].attack_type;
                opts.invasion_id = warning.invasion_id;
                tile2i invasion_tile = scenario_start_invasion_impl(opts);
                if (invasion_tile.valid()) {
                    events::emit(event_message{ true, "message_legion_attacks", g_invasions.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });
                }
            }
        }
    }
    // local uprisings
    for (int i = 0; i < MAX_INVASIONS; i++) {
        if (g_scenario.invasions[i].type == INVASION_TYPE_LOCAL_UPRISING) {
            if (game.simtime.year == g_scenario.start_year + g_scenario.invasions[i].year && game.simtime.month == g_scenario.invasions[i].month) {
                invasion_opts_t opts;
                opts.enemy_type = ENEMY_0_BARBARIAN;
                opts.size = g_scenario.invasions[i].amount;
                // opts.invasion_point = g_scenario.invasions[warning.invasion_id].from;
                opts.attack_type = g_scenario.invasions[i].attack_type;
                opts.invasion_id = i;
                tile2i invasion_tile = scenario_start_invasion_impl(opts);
                if (invasion_tile.valid()) {
                    events::emit(event_message{ true, "message_local_uprising", g_invasions.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });
                }
            }
        }
    }
}

int map_invasion_point(tile2i point) {
    auto &lands = g_scenario.invasion_points_land;
    const auto it_land = std::find_if(lands.begin(), lands.end(), [point] (auto &p) { return p == point; });
    if (it_land != lands.end()) {
        return 1;
    }

    auto &sea = g_scenario.invasion_points_sea;
    const auto it_sea = std::find_if(sea.begin(), sea.end(), [point] (auto &p) { return p == point; });
    if (it_sea != sea.end()) {
        return 2;
    }

    return 0;
}

bool scenario_invasion_start_from_kingdome(int size) {
    auto &data = g_invasions;

    invasion_opts_t opts;
    opts.enemy_type = ENEMY_3_EGYPTIAN;
    opts.size = size;
    // opts.invasion_point = g_scenario.invasions[warning.invasion_id].from;
    opts.attack_type = FORMATION_ATTACK_BEST_BUILDINGS;
    opts.invasion_id = 24;

    tile2i invasion_tile = scenario_start_invasion_impl(opts);
    if (invasion_tile.valid()) {
        events::emit(event_message{ true, "message_kingdome_army_attack", data.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });
        return true;
    }
    return false;
}

void scenario_invasion_start(invasion_opts_t opts) {
    auto &data = g_invasions;
    switch (opts.mode) {
    case ATTACK_TYPE_ENEMIES: {
        // Prefer caller attack_type (JS invasion_attack_target / console / defaults).
        tile2i invasion_tile = scenario_start_invasion_impl(opts);
        if (invasion_tile.valid()) {
            events::emit(event_message{ true, "message_barbarians_attack", data.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });
        }
        break;
    }
    case ATTACK_TYPE_KINGDOME: {
        g_city.kingdome.force_attack(opts.size);
        break;
    }
    case ATTACK_TYPE_NATIVES: {
        opts.attack_type = FORMATION_ATTACK_FOOD_CHAIN;
        opts.enemy_type = ENEMY_0_BARBARIAN;
        tile2i invasion_tile = scenario_start_invasion_impl(opts);
        if (invasion_tile.valid())
            events::emit(event_message{ true, "message_local_wrath_of_seth", data.last_internal_invasion_id, invasion_tile.grid_offset(), SOURCE_LOCATION });

        break;
    }
    default:
        break;
    }
}

io_buffer* iob_invasion_warnings = new io_buffer([](io_buffer* iob, size_t version) {
    //    data.last_internal_invasion_id = invasion_id->read_u16();

    //    for (int i = 0; i < MAX_INVASION_WARNINGS; i++) {
    //        invasion_warning *w = &data.warnings[i];
    //        w->in_use = warnings->read_u8();
    //        w->handled = warnings->read_u8();
    //        w->invasion_path_id = warnings->read_u8();
    //        w->warning_years = warnings->read_u8();
    //        w->x = warnings->read_i16();
    //        w->y = warnings->read_i16();
    //        w->image_id = warnings->read_i16();
    //        w->empire_object_id = warnings->read_i16();
    //        w->month_notified = warnings->read_i16();
    //        w->year_notified = warnings->read_i16();
    //        w->months_to_go = warnings->read_i32();
    //        w->invasion_id = warnings->read_u8();
    //        warnings->skip(11);
    //    }

    // TODO (B3)
});

// B2 Phase 3: mid-fight invasion↔event pending survives .svx save/load.
// Layout (272 bytes): flag(4) + 32 × {in_use, enemies_seen, invasion_id, pad, event_id, want_destroy, pad}
io_buffer *iob_invasion_event_pending = new io_buffer([] (io_buffer *iob, size_t version) {
    int32_t native_flag = g_scenario.env.use_native_invasion_events ? 1 : 0;
    iob->bind(BIND_SIGNATURE_INT32, &native_flag);
    if (iob->is_read_access()) {
        g_scenario.env.use_native_invasion_events = native_flag != 0;
    }

    for (auto &p : g_invasions.event_pending) {
        iob->bind_bool(p.in_use);
        iob->bind_bool(p.enemies_seen);
        iob->bind_u8(p.invasion_id);
        iob->bind____skip(1);
        iob->bind(BIND_SIGNATURE_INT16, &p.event_id);
        iob->bind_u8(p.want_destroy);
        iob->bind____skip(1);
    }
    iob->bind____skip(12); // pad 260 → 272
});

const enemy_properties_t &invasion_data_t::get_prop(e_enemy_type type) {
    return *g_enemy_properties[type];
}
