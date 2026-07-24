// Integral-test helpers: dump mission1.pak scenario data WITHOUT JS mission overlays
// (see GamestateIO::load_mission_pak_raw). Used by D1b-style verification
// (tests/45_mission11_pak_dump.js) and ad-hoc mission dumps.

#include "js_game.h"

#include "building/building.h"
#include "building/building_type.h"
#include "city/city.h"
#include "city/city_religion.h"
#include "core/log.h"
#include "core/profiler.h"
#include "core/string.h"
#include "empire/empire.h"
#include "empire/empire_city.h"
#include "empire/trade_route.h"
#include "empire/type.h"
#include "figure/figure_type.h"
#include "game/game_environment.h"
#include "game/resource.h"
#include "grid/point.h"
#include "io/gamefiles/lang.h"
#include "io/gamestate/boilerplate.h"
#include "js/js_defines.h"
#include "platform/arguments.h"
#include "scenario/scenario.h"
#include "scenario/scenario_event_manager.h"
#include "scenario/types.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef main
#undef main
#endif

static void dump_marker(pcstr fmt, ...) {
    char buf[1280];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);

    if (g_args.is_integral_tests()) {
        logs::info("[test-marker] %s", buf);
    }
    logs::info("%s", buf);
}

static pcstr safe_token(pcstr name) {
    return (name && name[0]) ? name : "?";
}

static void append_resource_list(char* out, size_t out_sz, const bool* flags) {
    out[0] = 0;
    size_t used = 0;
    for (int r = 1; r < RESOURCES_MAX; r++) {
        if (!flags[r]) {
            continue;
        }
        pcstr name = resource_name((e_resource)r);
        const size_t nlen = strlen(name);
        if (used + nlen + 2 >= out_sz) {
            break;
        }
        if (used > 0) {
            out[used++] = ',';
        }
        memcpy(out + used, name, nlen);
        used += nlen;
        out[used] = 0;
    }
}

static pcstr climate_name(int climate) {
    switch (climate) {
    case CLIMATE_CENTRAL: return "central";
    case CLIMATE_NORTHERN: return "northern";
    case CLIMATE_DESERT: return "desert";
    default: return "?";
    }
}

static void dump_scenario_header(int scenario_id) {
    dump_marker("pak_dump_ok:%d", scenario_id);
    dump_marker("pak_start_year:%d", g_scenario.start_year);
    dump_marker("pak_player_rank:%d", g_scenario.player_rank);
    dump_marker("pak_player_faction:%d|incarnation:%d|pharaoh:%d",
        g_scenario.player_faction,
        g_scenario.player_incarnation,
        g_scenario.current_pharaoh);
    dump_marker("pak_enemy_id:%d(%s)",
        (int)g_scenario.enemy_id,
        safe_token(e_enemy_type_tokens.name(g_scenario.enemy_id)));
    dump_marker("pak_funds:%d|loan:%d|debt_interest:%d",
        g_scenario.finance.initial_funds,
        g_scenario.finance.rescue_loan,
        g_scenario.debt_interest_rate);
    dump_marker("pak_climate:%d(%s)|open_play:%d|image_id:%d",
        g_scenario.climate,
        climate_name(g_scenario.climate),
        g_scenario.is_open_play ? 1 : 0,
        g_scenario.image_id);
    dump_marker("pak_env:animals=%d|flotsam=%d|alt_predator=%d",
        g_scenario.env.has_animals ? 1 : 0,
        g_scenario.env.flotsam_enabled ? 1 : 0,
        g_scenario.alt_predator_type ? 1 : 0);
    dump_marker("pak_empire:id=%d|expanded=%d|expansion_year=%d",
        g_scenario.empire.id,
        g_scenario.empire.is_expanded,
        g_scenario.empire.expansion_year);

    pcstr subtitle = g_scenario.subtitle.c_str();
    pcstr brief = g_scenario.brief_description.c_str();
    dump_marker("pak_subtitle:%s", (subtitle && subtitle[0]) ? subtitle : "-");
    dump_marker("pak_brief:%s", (brief && brief[0]) ? brief : "-");
}

static void dump_win_criteria() {
    const auto& w = g_scenario.win_criteria;
    dump_marker("pak_win:pop=%d/%d|culture=%d/%d|prosperity=%d/%d|monuments=%d/%d|kingdom=%d/%d",
        w.population.enabled ? 1 : 0,
        w.population.goal,
        w.culture.enabled ? 1 : 0,
        w.culture.goal,
        w.prosperity.enabled ? 1 : 0,
        w.prosperity.goal,
        w.monuments.enabled ? 1 : 0,
        w.monuments.goal,
        w.kingdom.enabled ? 1 : 0,
        w.kingdom.goal);
    dump_marker("pak_win_housing:count=%d/%d|level=%d/%d",
        w.housing_count.enabled ? 1 : 0,
        w.housing_count.goal,
        w.housing_level.enabled ? 1 : 0,
        w.housing_level.goal);
    dump_marker("pak_win_time:limit=%d/%d|survival=%d/%d|m25=%d|m50=%d|m75=%d",
        w.time_limit.enabled ? 1 : 0,
        w.time_limit.years,
        w.survival_time.enabled ? 1 : 0,
        w.survival_time.years,
        w.milestone25_year,
        w.milestone50_year,
        w.milestone75_year);
}

static void dump_map_points() {
    dump_marker("pak_map:w=%d|h=%d|start_offset=%d|border=%d",
        g_scenario.map.width,
        g_scenario.map.height,
        g_scenario.map.start_offset,
        g_scenario.map.border_size);

    auto dump_tile = [](pcstr tag, tile2i t) {
        if (!t.valid()) {
            dump_marker("%s:invalid", tag);
            return;
        }
        dump_marker("%s:x=%d|y=%d", tag, t.x(), t.y());
    };

    dump_tile("pak_entry", g_scenario.entry_point);
    dump_tile("pak_exit", g_scenario.exit_point);
    dump_tile("pak_river_entry", g_scenario.river_entry_point);
    dump_tile("pak_river_exit", g_scenario.river_exit_point);
    dump_tile("pak_earthquake", g_scenario.earthquake_point);

    int herd_n = 0;
    for (int i = 0; i < MAX_PREDATOR_HERD_POINTS; i++) {
        tile2i t = g_scenario.herd_points_animals[i];
        if (!t.valid()) {
            continue;
        }
        e_figure_type ft = g_scenario.herd_type_animals[i];
        dump_marker("pak_herd:i=%d|x=%d|y=%d|type=%d(%s)",
            i,
            t.x(),
            t.y(),
            (int)ft,
            safe_token(e_figure_type_tokens.name(ft)));
        herd_n++;
    }
    dump_marker("pak_herd_count:%d", herd_n);

    int prey_n = 0;
    for (int i = 0; i < MAX_PREY_HERD_POINTS; i++) {
        tile2i t = g_scenario.herd_points_prey[i];
        if (!t.valid()) {
            continue;
        }
        dump_marker("pak_prey:i=%d|x=%d|y=%d", i, t.x(), t.y());
        prey_n++;
    }
    dump_marker("pak_prey_count:%d", prey_n);

    int fish_n = 0;
    for (int i = 0; i < MAX_FISH_POINTS; i++) {
        tile2i t = g_scenario.fishing_points[i];
        if (!t.valid()) {
            continue;
        }
        dump_marker("pak_fish:i=%d|x=%d|y=%d", i, t.x(), t.y());
        fish_n++;
    }
    dump_marker("pak_fish_count:%d", fish_n);

    int disembark_n = 0;
    for (int i = 0; i < MAX_DISEMBARK_POINTS; i++) {
        tile2i t = g_scenario.disembark_points[i];
        if (!t.valid()) {
            continue;
        }
        dump_marker("pak_disembark:i=%d|x=%d|y=%d", i, t.x(), t.y());
        disembark_n++;
    }
    dump_marker("pak_disembark_count:%d", disembark_n);
}

static void dump_invasion_points() {
    int land_n = 0;
    for (int i = 0; i < (int)g_scenario.invasion_points_land.size(); i++) {
        tile2i t = g_scenario.invasion_points_land[i];
        if (!t.valid()) {
            continue;
        }
        dump_marker("pak_inv_land:i=%d|x=%d|y=%d", i, t.x(), t.y());
        land_n++;
    }

    int sea_n = 0;
    for (int i = 0; i < (int)g_scenario.invasion_points_sea.size(); i++) {
        tile2i t = g_scenario.invasion_points_sea[i];
        if (!t.valid()) {
            continue;
        }
        dump_marker("pak_inv_sea:i=%d|x=%d|y=%d", i, t.x(), t.y());
        sea_n++;
    }

    dump_marker("pak_inv_land_count:%d", land_n);
    dump_marker("pak_inv_sea_count:%d", sea_n);
}

static void dump_monuments() {
    dump_marker("pak_monuments:first=%d|second=%d|third=%d",
        g_scenario.monuments.first,
        g_scenario.monuments.second,
        g_scenario.monuments.third);

    int burial_n = 0;
    for (int r = 1; r < RESOURCES_MAX; r++) {
        const int req = g_scenario.monuments.burial_provisions[r].required;
        const int dispatched = g_scenario.monuments.burial_provisions[r].dispatched;
        if (req <= 0 && dispatched <= 0) {
            continue;
        }
        dump_marker("pak_burial:res=%s(%d)|required=%d|dispatched=%d",
            safe_token(resource_name((e_resource)r)),
            r,
            req,
            dispatched);
        burial_n++;
    }
    dump_marker("pak_burial_count:%d", burial_n);
}

static void dump_gods() {
    char known[256] = {0};
    size_t used = 0;
    for (int g = 0; g < MAX_GODS; g++) {
        const e_god_status st = g_city.religion.gods[g].is_known;
        if (st == GOD_STATUS_UNKNOWN) {
            continue;
        }
        pcstr name = e_god_tokens.name((e_god)g);
        const size_t nlen = name ? strlen(name) : 1;
        if (used + nlen + 8 >= sizeof known) {
            break;
        }
        if (used > 0) {
            known[used++] = ',';
        }
        if (name) {
            memcpy(known + used, name, nlen);
            used += nlen;
        } else {
            known[used++] = '?';
        }
        used += snprintf(known + used, sizeof known - used, ":%d", (int)st);
    }
    dump_marker("pak_gods:%s", known[0] ? known : "-");
}

static void dump_empire_cities() {
    char sells[512];
    char buys[512];
    int trade_cities = 0;

    for (int i = 0; i < g_empire.get_cities().size(); i++) {
        const empire_city& city = g_empire.get_cities()[i];
        if (!city.in_use) {
            continue;
        }

        pcstr name = (pcstr)lang_get_string(195, city.name_id);
        if (!name || !name[0]) {
            name = "?";
        }

        append_resource_list(sells, sizeof sells, city.sells_resource);
        append_resource_list(buys, sizeof buys, city.buys_resource);

        const bool has_trade = sells[0] || buys[0] || city.can_trade();
        if (!has_trade && city.type != EMPIRE_CITY_OURS) {
            continue;
        }

        trade_cities++;
        dump_marker("pak_city:%s|id=%d|type=%d|open=%d|sea=%d|cost=%d|route=%d|sells=%s|buys=%s",
            name,
            i,
            (int)city.type,
            city.is_open ? 1 : 0,
            city.is_sea_trade ? 1 : 0,
            (int)city.cost_to_open,
            city.route_id,
            sells[0] ? sells : "-",
            buys[0] ? buys : "-");

        // Per-route yearly limits (only non-zero).
        if (city.route_id > 0) {
            const trade_route& route = city.get_route();
            char limits[512] = {0};
            size_t used = 0;
            for (int r = 1; r < RESOURCES_MAX; r++) {
                const int lim = route.limit((e_resource)r, trade_route::e_limit_base_only);
                if (lim <= 0) {
                    continue;
                }
                pcstr rn = resource_name((e_resource)r);
                const int n = snprintf(limits + used, sizeof limits - used, "%s%s=%d",
                    used ? "," : "",
                    rn ? rn : "?",
                    lim);
                if (n <= 0 || used + (size_t)n >= sizeof limits) {
                    break;
                }
                used += (size_t)n;
            }
            if (limits[0]) {
                dump_marker("pak_route_limits:city=%s|route=%d|%s", name, city.route_id, limits);
            }
        }
    }

    dump_marker("pak_trade_city_count:%d", trade_cities);
}

static pcstr invader_name(int item) {
    switch (item) {
    case EVENT_INVADER_ENEMY: return "enemy";
    case EVENT_INVADER_EGYPT: return "egypt";
    case EVENT_INVADER_PHARAOH: return "pharaoh";
    case EVENT_INVADER_BEDUINS: return "beduins";
    default: return "?";
    }
}

static pcstr trigger_name(int trigger) {
    switch (trigger) {
    case EVENT_TRIGGER_ONCE: return "once";
    case EVENT_TRIGGER_ONLY_VIA_EVENT: return "chain_only";
    case EVENT_TRIGGER_RECURRING: return "recurring";
    case EVENT_TRIGGER_ALREADY_FIRED: return "already_fired";
    case EVENT_TRIGGER_ACTIVATED_8: return "activated_8";
    case EVENT_TRIGGER_BY_RATING: return "by_rating_legacy10";
    case EVENT_TRIGGER_ACTIVATED_12: return "activated_12";
    case EVENT_TRIGGER_BY_FAVOUR: return "by_favour";
    case EVENT_TRIGGER_BY_FAVOUR_IN_USE: return "by_favour_in_use";
    default: return "?";
    }
}

static pcstr invasion_semantics(int trigger, int invader, int amount) {
    if (trigger == EVENT_TRIGGER_BY_FAVOUR || trigger == EVENT_TRIGGER_BY_FAVOUR_IN_USE) {
        return "favour_kr_punishment";
    }
    if (trigger == EVENT_TRIGGER_ONLY_VIA_EVENT) {
        return "chain_child";
    }
    if (invader == EVENT_INVADER_PHARAOH && amount >= 32) {
        return "pharaoh_army_large";
    }
    return "timed_or_recurring";
}

static void dump_scenario_events() {
    const int n = g_scenario.events.events_count();
    int requests = 0;
    int invasions = 0;
    int distant = 0;
    int other = 0;

    for (int i = 0; i < n; i++) {
        const event_ph_t* ev = g_scenario.events.at(i);
        if (!ev || ev->type == EVENT_TYPE_NONE) {
            continue;
        }

        dump_marker(
            "pak_event:i=%d|type=%d(%s)|year=%d|month=%d|item=%d|amount=%d|months=%d|"
            "loc=%d,%d,%d,%d|sender=%d|subtype=%d|city=%d|trigger=%d(%s)|active=%d|"
            "ok=%d|refuse=%d|late=%d|defeat=%d|attack=%d",
            i,
            (int)ev->type,
            safe_token(e_event_type_tokens.name((e_event_type)ev->type)),
            (int)ev->time.year,
            (int)ev->time.month,
            (int)ev->item.value,
            (int)ev->amount.value,
            (int)ev->months_initial,
            (int)ev->location_fields[0],
            (int)ev->location_fields[1],
            (int)ev->location_fields[2],
            (int)ev->location_fields[3],
            (int)ev->sender_faction,
            (int)ev->subtype,
            (int)ev->city_id,
            (int)ev->event_trigger_type,
            trigger_name((int)ev->event_trigger_type),
            ev->is_active ? 1 : 0,
            (int)ev->on_completed_action,
            (int)ev->on_refusal_action,
            (int)ev->on_too_late_action,
            (int)ev->on_defeat_action,
            (int)ev->invasion_attack_target);

        if (ev->type == EVENT_TYPE_REQUEST) {
            requests++;
            const int subtype = (int)ev->subtype;
            if (subtype == EVENT_SUBTYPE_CITY_ASKS_FOR_TROOPS) {
                dump_marker("pak_request:year=%d|month=%d|res=troops(subtype1)|amount=%d|months=%d|item_raw=%d",
                    (int)ev->time.year,
                    (int)ev->time.month,
                    (int)ev->amount.value,
                    (int)ev->months_initial,
                    (int)ev->item.value);
            } else {
                dump_marker("pak_request:year=%d|month=%d|res=%s(%d)|amount=%d|months=%d",
                    (int)ev->time.year,
                    (int)ev->time.month,
                    safe_token(resource_name((e_resource)ev->item.value)),
                    (int)ev->item.value,
                    (int)ev->amount.value,
                    (int)ev->months_initial);
            }
        } else if (ev->type == EVENT_TYPE_INVASION) {
            invasions++;
            dump_marker(
                "pak_invasion_event:year=%d|month=%d|amount=%d|invader=%s(%d)|trigger=%s(%d)|"
                "semantics=%s|loc=%d,%d|sender=%d|subtype=%d|attack=%d|ok=%d|refuse=%d|late=%d|defeat=%d",
                (int)ev->time.year,
                (int)ev->time.month,
                (int)ev->amount.value,
                invader_name((int)ev->item.value),
                (int)ev->item.value,
                trigger_name((int)ev->event_trigger_type),
                (int)ev->event_trigger_type,
                invasion_semantics((int)ev->event_trigger_type, (int)ev->item.value, (int)ev->amount.value),
                (int)ev->location_fields[0],
                (int)ev->location_fields[1],
                (int)ev->sender_faction,
                (int)ev->subtype,
                (int)ev->invasion_attack_target,
                (int)ev->on_completed_action,
                (int)ev->on_refusal_action,
                (int)ev->on_too_late_action,
                (int)ev->on_defeat_action);
        } else if (ev->type == EVENT_TYPE_DISTANT_BATTLE || ev->type == EVENT_TYPE_DISTANT_BATTLE_WON) {
            distant++;
            dump_marker("pak_distant_battle:year=%d|month=%d|amount=%d|city=%d|trigger=%s(%d)|months=%d",
                (int)ev->time.year,
                (int)ev->time.month,
                (int)ev->amount.value,
                (int)ev->city_id,
                trigger_name((int)ev->event_trigger_type),
                (int)ev->event_trigger_type,
                (int)ev->months_initial);
        } else {
            other++;
        }
    }

    dump_marker("pak_event_count:%d", n);
    dump_marker("pak_request_count:%d", requests);
    dump_marker("pak_invasion_event_count:%d", invasions);
    dump_marker("pak_distant_battle_count:%d", distant);
    dump_marker("pak_other_event_count:%d", other);
}

static void dump_legacy_tables() {
    // Caesar3-style tables — often empty in Pharaoh (events live in scenario_events).
    int inv_n = 0;
    for (int i = 0; i < 40; i++) {
        const invasion_t& inv = g_scenario.invasions[i];
        if (!inv.type) {
            continue;
        }
        dump_marker("pak_legacy_invasion:i=%d|year=%d|month=%d|type=%d|amount=%d|from=%d|attack=%d",
            i,
            inv.year,
            inv.month,
            inv.type,
            inv.amount,
            inv.from,
            (int)inv.attack_type);
        inv_n++;
    }
    dump_marker("pak_legacy_invasion_count:%d", inv_n);

    int demand_n = 0;
    for (int i = 0; i < 40; i++) {
        const demand_change_t& d = g_scenario.demand_changes[i];
        if (!d.year && !d.resource) {
            continue;
        }
        dump_marker("pak_demand_change:i=%d|year=%d|month=%d|res=%s(%d)|route=%d|rise=%d",
            i,
            d.year,
            d.month,
            safe_token(resource_name(d.resource)),
            (int)d.resource,
            d.route_id,
            d.is_rise);
        demand_n++;
    }
    dump_marker("pak_demand_change_count:%d", demand_n);

    int price_n = 0;
    for (int i = 0; i < 40; i++) {
        const price_change_t& p = g_scenario.price_changes[i];
        if (!p.year && !p.resource) {
            continue;
        }
        dump_marker("pak_price_change:i=%d|year=%d|month=%d|res=%s(%d)|amount=%d|rise=%d",
            i,
            p.year,
            p.month,
            safe_token(resource_name(p.resource)),
            (int)p.resource,
            p.amount,
            p.is_rise);
        price_n++;
    }
    dump_marker("pak_price_change_count:%d", price_n);
}

static void dump_starting_buildings() {
    int counts[BUILDING_MAX] = {0};
    int total = 0;

    for (building* b = building_begin(); b != building_end(); ++b) {
        if (!b || !b->is_valid()) {
            continue;
        }
        if (b->type > BUILDING_NONE && b->type < BUILDING_MAX) {
            counts[b->type]++;
            total++;
        }
        // Skip vacant lots / houses in the per-building list to keep logs readable;
        // still counted in the summary above.
        if (b->type >= BUILDING_HOUSE_VACANT_LOT && b->type <= BUILDING_HOUSE_PALATIAL_ESTATE) {
            continue;
        }
        dump_marker("pak_building:id=%d|type=%d(%s)|x=%d|y=%d",
            b->id,
            (int)b->type,
            safe_token(e_building_type_tokens.name(b->type)),
            b->tile.x(),
            b->tile.y());
    }

    dump_marker("pak_building_total:%d", total);
    for (int t = 1; t < BUILDING_MAX; t++) {
        if (counts[t] <= 0) {
            continue;
        }
        dump_marker("pak_building_count:type=%d(%s)|n=%d",
            t,
            safe_token(e_building_type_tokens.name((e_building_type)t)),
            counts[t]);
    }
}

// Returns 1 on successful dump, 0 if skipped (--no-resource) or load failed.
static int __test_mission_pak_dump(int scenario_id) {
    if (g_args.no_resource()) {
        dump_marker("pak_dump_skipped:no_resource");
        return 0;
    }

    if (!GamestateIO::load_mission_pak_raw(scenario_id)) {
        logs::info("[test] load_mission_pak_raw(%d) failed", scenario_id);
        dump_marker("pak_dump_fail:%d", scenario_id);
        return 0;
    }

    dump_scenario_header(scenario_id);
    dump_win_criteria();
    dump_map_points();
    dump_invasion_points();
    dump_monuments();
    dump_gods();
    dump_empire_cities();
    dump_scenario_events();
    dump_legacy_tables();
    dump_starting_buildings();

    dump_marker("pak_dump_done:%d", scenario_id);
    return 1;
}
ANK_FUNCTION_1(__test_mission_pak_dump);
