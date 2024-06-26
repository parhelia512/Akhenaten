#include "cheats.h"

#include "building/construction/build_planner.h"
#include "building/building_type.h"
#include "building/house_population.h"
#include "building/destruction.h"
#include "building/monuments.h"
#include "city/finance.h"
#include "city/gods.h"
#include "city/victory.h"
#include "city/warning.h"
#include "city/health.h"
#include "city/resource.h"
#include "core/string.h"
#include "figure/figure.h"
#include "game/tick.h"
#include "game/tutorial.h"
#include "graphics/color.h"
#include "graphics/font.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "scenario/invasion.h"
#include "window/window_building_info.h"
#include "window/city.h"
#include "window/console.h"
#include "figure/formation_herd.h"
#include "dev/debug.h"

#include <string.h>
#include <iostream>

#ifndef _WIN32
#define stricmp strcasecmp
#endif

static void game_cheat_start_invasion(pcstr);
static void game_cheat_advance_year(pcstr);
static void game_cheat_cast_blessing(pcstr);
static void game_cheat_show_tooltip(pcstr);
static void game_cheat_victory(pcstr);
static void game_cheat_cast_upset(pcstr);
static void game_cheat_pop_milestone(pcstr);
static void game_cheat_fire(pcstr);
static void game_cheat_nodamage(pcstr);
static void game_cheat_spacious_apartment(pcstr);
static void game_cheat_spawn_nobles(pcstr);
static void game_cheat_kill_fish_boats(pcstr);
static void game_cheat_update_fish_points(pcstr);
static void game_cheat_tutorial_step(pcstr);
static void game_cheat_finish_phase(pcstr);
static void game_cheat_clear_progress(pcstr);
static void game_cheat_add_bricks(pcstr);
static void game_cheat_add_clay(pcstr);

using cheat_command = void(pcstr);

struct cheat_command_handle {
    const char* name;
    cheat_command* command;
};

static cheat_command_handle g_cheat_commands[] = {{"startinvasion", game_cheat_start_invasion},
                                                  {"addbricks", game_cheat_add_bricks},
                                                  {"addclay", game_cheat_add_clay},
                                                  {"nextyear", game_cheat_advance_year},
                                                  {"blessing", game_cheat_cast_blessing},
                                                  {"godupset", game_cheat_cast_upset},
                                                  {"showtooltip", game_cheat_show_tooltip},
                                                  {"victory", game_cheat_victory},
                                                  {"popmilestone", game_cheat_pop_milestone},
                                                  {"fire", game_cheat_fire},
                                                  {"nodamage", game_cheat_nodamage},
                                                  {"spawnnobles", game_cheat_spawn_nobles},
                                                  {"tutspaciousapt", game_cheat_spacious_apartment},
                                                  {"killfishboats", game_cheat_kill_fish_boats},
                                                  {"upfishpoints", game_cheat_update_fish_points},
                                                  {"tutorialstep", game_cheat_tutorial_step},
                                                  {"finishphase", game_cheat_finish_phase},
                                                  {"clearprogress", game_cheat_clear_progress}
};

struct cheats_data_t {
    bool is_cheating;
    int tooltip_enabled;
};

cheats_data_t g_cheats_data;

static int parse_word(pcstr string, pstr word) {
    int count = 0;
    while (*string && *string != ' ') {
        *word = *string;
        word++;
        string++;
        count++;
    }
    *word = '\0';
    return count + 1;
}

// return value is next argument index
int parse_integer(pcstr string, int &value) {
    bstring64 copy;
    int count = 0;
    while (*string && *string != ' ') {
        copy.data()[count] = *string;
        count++;
        string++;
    }
    copy.data()[count] = '\0';
    value = string_to_int(copy);
    return count + 1;
}

void game_cheat_activate() {
    if (window_is(WINDOW_BUILDING_INFO)) {
        g_cheats_data.is_cheating = window_building_info_get_int() == BUILDING_WELL;
    } else if (g_cheats_data.is_cheating && window_is(WINDOW_MESSAGE_DIALOG)) {
        g_cheats_data.is_cheating = true;
        scenario_invasion_start_from_cheat();
    } else {
        g_cheats_data.is_cheating = 0;
    }
}

int game_cheat_tooltip_enabled(void) {
    return g_cheats_data.tooltip_enabled;
}

void game_cheat_money(void) {
    if (g_cheats_data.is_cheating) {
        city_finance_process_cheat();
        window_invalidate();
    }
}

void game_cheat_victory(pcstr) {
    if (g_cheats_data.is_cheating) {
        city_victory_force_win();
    }
}

void game_cheat_breakpoint() {
    if (g_cheats_data.is_cheating) {
        //
    }
}

void game_cheat_console(bool force) {
    g_cheats_data.is_cheating |= force;
    if (g_cheats_data.is_cheating) {
        Planner.reset();
        window_city_show();
        window_console_show();
    }
}

static void game_cheat_finish_phase(pcstr args) {
    buildings_valid_do([&] (building &b) {
        if (!b.is_monument()) {
            return;
        }

        if (!building_monument_is_unfinished(&b)) {
            return;
        }

        building *part = &b;
        while (part) {
            grid_area area = map_grid_get_area(part->tile, part->size, 0);
            map_grid_area_foreach(area.tmin, area.tmax, [] (tile2i tile) {
                map_monuments_set_progress(tile, 200);
            });

            part = (part->next_part_building_id > 0) ? building_get(part->next_part_building_id) : nullptr;
        };
    });
}

static void game_cheat_clear_progress(pcstr args) {
    map_monuments_clear();
}

static void game_cheat_add_clay(pcstr args) {
    int clay = 0;
    parse_integer(args ? args : (pcstr )"100", clay);
    city_resource_add_items(RESOURCE_CLAY, clay);
    window_invalidate();

    city_warning_show_console("Added clay");
}

static void game_cheat_add_bricks(pcstr args) {
    int bricks = 0;
    parse_integer(args ? args : (pcstr )"100", bricks);
    city_resource_add_items(RESOURCE_BRICKS, bricks);
    window_invalidate();

    city_warning_show_console("Added bricks");
}

static void game_cheat_pop_milestone(pcstr args) {
    city_population_reached_milestone(true);
}

static void game_cheat_start_invasion(pcstr args) {
    int attack_type = 0;
    int size = 0;
    int invasion_point = 0;
    int index = parse_integer(args, attack_type); // 0 barbarians, 1 caesar, 2 mars natives
    index = parse_integer(args + index, size);
    parse_integer(args + index, invasion_point);
    scenario_invasion_start_from_console(attack_type, size, invasion_point);

    city_warning_show_console("Started invasion");
}

static void game_cheat_fire(pcstr args) {
    int count = 0;
    parse_integer(args ? args : (pcstr )"10", count);

    svector<building *, 1000> buildings;
    buildings_valid_do([&] (building &b) {
        buildings.push_back(&b);
    });

    int step = std::max<int>(1, (int)buildings.size() / count);
    for (int i = 0; i < buildings.size(); i += step) {
        building_destroy_by_fire(buildings[i]);
    }
}

static void game_cheat_spacious_apartment(pcstr args) {
    tutorial_on_house_evolve(HOUSE_SPACIOUS_APARTMENT);
}

static void game_cheat_nodamage(pcstr args) {
    buildings_valid_do([&] (building &b) {
        b.damage_risk = 0;
    });
}

static void game_cheat_update_fish_points(pcstr args) {
    int count = 0;
    parse_integer(args ? args : "10", count);

    formation_fish_update(count);
}

static void game_cheat_tutorial_step(pcstr args) {
    int step = 0;
    parse_integer(args ? args : "0", step);

    tutorial_update_step(step);
}

static void game_cheat_kill_fish_boats(pcstr ) {
    buildings_valid_do([&] (building &b) {
        if (b.type != BUILDING_FISHING_WHARF) {
            return;
        }

        b.data.industry.fishing_boat_id = 0;
    });

    for (int i = 0; i < MAX_FIGURES[GAME_ENV]; i++) {
        figure* f = figure_get(i);
        if (f->has_type(FIGURE_FISHING_BOAT)) {
            f->advance_action(FIGURE_ACTION_149_CORPSE);
        }
    }
}

static void game_cheat_spawn_nobles(pcstr args) {
    int count = 0;
    parse_integer(args ? args : "10", count);

    svector<building *, 1000> buildings;
    buildings_valid_do([&] (building &b) {
        if (b.house_size > 0) {
            buildings.push_back(&b);
        }
    });
    
    int step = std::max<int>(1, (int)buildings.size() / count);
    for (int i = 0; i < buildings.size(); i += step) {
        if (!buildings[i]->has_road_access) {
            continue;
        }
        buildings[i]->create_roaming_figure(FIGURE_NOBLES, FIGURE_ACTION_125_ROAMING);
    }
}

static void game_cheat_advance_year(pcstr args) {
    game_tick_cheat_year();

    city_warning_show_console("Year advanced");
}

static void game_cheat_cast_blessing(pcstr args) {
    int god_id = 0;
    parse_integer(args, god_id);
    city_god_blessing_cheat((e_god)god_id);

    city_warning_show_console("Casted blessing");
}

static void game_cheat_cast_upset(pcstr args) {
    int god_id = 0;
    parse_integer(args, god_id);
    city_god_upset_cheat((e_god)god_id);

    city_warning_show_console("Casted upset");
}

static void game_cheat_show_tooltip(pcstr args) {
    parse_integer(args, g_cheats_data.tooltip_enabled);

    city_warning_show_console("Show tooltip toggled");
}

void game_cheat_parse_command(pcstr command) {
    bstring256 command_to_call;
    int next_arg = parse_word(command, command_to_call);

    for (auto& handle : g_cheat_commands) {
        if (stricmp((char*)command_to_call, handle.name) == 0) {
            pcstr args = (next_arg >= strlen((const char *)command)) ? nullptr : (command + next_arg);
            handle.command(args);
        }
    }
}

static void game_cheat_add_money(std::istream &is, std::ostream &os) {
    std::string args; is >> args;
    int money = 0;
    parse_integer(args.empty() ? (pcstr )"100" : args.c_str(), money);
    city_finance_process_console(money);
    window_invalidate();

    city_warning_show_console("Added money");
}

declare_console_command(addmoney, game_cheat_add_money);