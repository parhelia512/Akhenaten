#include "cheats.h"

#include "building/construction/build_planner.h"
#include "building/building_house.h"
#include "building/building_type.h"
#include "building/destruction.h"
#include "building/monuments.h"
#include "city/city.h"
#include "city/city_health.h"
#include "city/city_message.h"
#include "city/city_resource.h"
#include "city/city_warnings.h"
#include "core/string.h"
#include "figure/figure.h"
#include "figuretype/figure_locust.h"
#include "game/game_events.h"
#include "graphics/color.h"
#include "graphics/font.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "js/js_game.h"
#include "scenario/scenario_invasion.h"
#include "window/window_building_info.h"
#include "window/window_city.h"
#include "window/console.h"
#include "dev/debug.h"

#include <iostream>
#include <string.h>

#ifndef _WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

static void game_cheat_spawn_nobles(pcstr);
static void game_cheat_clear_progress(pcstr);
static void game_cheat_crop_busters(pcstr);

using cheat_command = void(pcstr);

struct cheat_command_handle {
    const char* name;
    cheat_command* command;
};

// OG Cleopatra phrase "Crop Busters" (space). Also accept cropbusters.
static cheat_command_handle g_cheat_commands[] = {{"spawnnobles", game_cheat_spawn_nobles},
                                                  {"clearprogress", game_cheat_clear_progress},
                                                  {"crop busters", game_cheat_crop_busters},
                                                  {"cropbusters", game_cheat_crop_busters}};

struct cheats_data_t {
    bool is_cheating;
};

cheats_data_t g_cheats_data;

void game_cheat_force_activate() {
    g_cheats_data.is_cheating = true;
}

bool game_cheat_is_active() {
    return g_cheats_data.is_cheating;
}

void game_cheat_activate() {
    if (g_window_manager.window_is("window_building_info")) {
        g_cheats_data.is_cheating = (window_building_info_get_type() == BUILDING_WELL);
    } else {
        g_cheats_data.is_cheating = 0;
    }
}

static void game_cheat_clear_progress(pcstr args) {
    map_monuments_clear();
}

static void game_cheat_spawn_nobles(pcstr args) {
    int count = 0;
    parse_integer(args ? args : "10", count);

    svector<building *, 1000> buildings;
    buildings_house_do([&] (auto house) {
        if (house->house_population() > 0) {
            buildings.push_back(&house->base);
        }
    });

    int step = std::max<int>(1, (int)buildings.size() / count);
    for (int i = 0; i < buildings.size(); i += step) {
        if (!buildings[i]->has_road_access) {
            continue;
        }
        buildings[i]->create_roaming_figure(FIGURE_NOBLES, (e_figure_action)ACTION_125_ROAMER_ROAMING, BUILDING_SLOT_SERVICE);
    }
}

static void game_cheat_crop_busters(pcstr) {
    figure_locust::apply_plague();
    messages::popup("message_plague_of_locusts", 0, 0);
}

void game_cheat_parse_command(pcstr command) {
    if (!command || !*command) {
        return;
    }

    // Prefix match so multi-word cheats ("crop busters") work.
    for (auto& handle : g_cheat_commands) {
        const size_t namelen = strlen(handle.name);
        if (strnicmp(command, handle.name, (int)namelen) != 0) {
            continue;
        }
        if (command[namelen] != '\0' && command[namelen] != ' ') {
            continue;
        }
        pcstr args = command[namelen] == '\0' ? nullptr : (command + namelen + 1);
        handle.command(args);
        return;
    }
}

void __debug_crash() {
    events::emit(event_city_warning{ "Trying to crash the game" });
    const int *p = nullptr;
    std::cout << *p;
}
ANK_FUNCTION(__debug_crash)

