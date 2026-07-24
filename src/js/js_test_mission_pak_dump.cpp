// Integral-test helpers: dump empire cities / scenario events / invasion points
// from mission1.pak WITHOUT applying JS mission overlays (see load_mission_pak_raw).
// Used by D1b-style verification (tests/45_mission11_pak_dump.js).

#include "js_game.h"

#include "core/log.h"
#include "core/profiler.h"
#include "core/string.h"
#include "empire/empire.h"
#include "empire/empire_city.h"
#include "empire/type.h"
#include "figure/figure_type.h"
#include "game/resource.h"
#include "grid/point.h"
#include "io/gamefiles/lang.h"
#include "io/gamestate/boilerplate.h"
#include "js/js_defines.h"
#include "platform/arguments.h"
#include "scenario/scenario.h"
#include "scenario/scenario_event_manager.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef main
#undef main
#endif

static void dump_marker(pcstr fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);

    if (g_args.is_integral_tests()) {
        logs::info("[test-marker] %s", buf);
    }
    logs::info("%s", buf);
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
    }

    dump_marker("pak_trade_city_count:%d", trade_cities);
}

static void dump_scenario_events() {
    const int n = g_scenario.events.events_count();
    int requests = 0;
    int invasions = 0;

    for (int i = 0; i < n; i++) {
        const event_ph_t* ev = g_scenario.events.at(i);
        if (!ev || ev->type == EVENT_TYPE_NONE) {
            continue;
        }

        pcstr type_name = e_event_type_tokens.name((e_event_type)ev->type);
        if (!type_name) {
            type_name = "?";
        }

        dump_marker(
            "pak_event:i=%d|type=%d(%s)|year=%d|month=%d|item=%d|amount=%d|months=%d|"
            "loc=%d,%d,%d,%d|sender=%d|subtype=%d|city=%d|trigger=%d|active=%d",
            i,
            (int)ev->type,
            type_name,
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
            ev->is_active ? 1 : 0);

        if (ev->type == EVENT_TYPE_REQUEST) {
            requests++;
            pcstr res = resource_name((e_resource)ev->item.value);
            dump_marker("pak_request:year=%d|month=%d|res=%s(%d)|amount=%d|months=%d",
                (int)ev->time.year,
                (int)ev->time.month,
                res ? res : "?",
                (int)ev->item.value,
                (int)ev->amount.value,
                (int)ev->months_initial);
        }
        if (ev->type == EVENT_TYPE_INVASION) {
            invasions++;
            dump_marker("pak_invasion_event:year=%d|month=%d|amount=%d|loc=%d,%d|sender=%d|subtype=%d",
                (int)ev->time.year,
                (int)ev->time.month,
                (int)ev->amount.value,
                (int)ev->location_fields[0],
                (int)ev->location_fields[1],
                (int)ev->sender_faction,
                (int)ev->subtype);
        }
    }

    dump_marker("pak_event_count:%d", n);
    dump_marker("pak_request_count:%d", requests);
    dump_marker("pak_invasion_event_count:%d", invasions);
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

    dump_marker("pak_dump_ok:%d", scenario_id);
    dump_marker("pak_start_year:%d", g_scenario.start_year);
    dump_marker("pak_player_rank:%d", g_scenario.player_rank);
    dump_marker("pak_enemy_id:%d(%s)",
        (int)g_scenario.enemy_id,
        e_enemy_type_tokens.name(g_scenario.enemy_id)
            ? e_enemy_type_tokens.name(g_scenario.enemy_id)
            : "?");
    dump_marker("pak_funds:%d|loan:%d",
        g_scenario.finance.initial_funds,
        g_scenario.finance.rescue_loan);

    dump_win_criteria();
    dump_empire_cities();
    dump_scenario_events();
    dump_invasion_points();

    dump_marker("pak_dump_done:%d", scenario_id);
    return 1;
}
ANK_FUNCTION_1(__test_mission_pak_dump);
