#pragma once

#include "core/vec2i.h"
#include "grid/point.h"
#include "figure/formation.h"
#include "figure/figure_type.h"
#include "core/archive.h"
#include "core/tokenum.h"

#include <array>
#include <functional>

enum e_attack_faction {
    ATTACK_TYPE_ENEMIES = 0,
    ATTACK_TYPE_KINGDOME = 1,
    ATTACK_TYPE_NATIVES = 2,
    ATTACK_TYPE_MAX
};
using e_attack_faction_tokens_t = token_holder<e_attack_faction, ATTACK_TYPE_ENEMIES, ATTACK_TYPE_MAX>;
extern const e_attack_faction_tokens_t e_attack_faction_tokens;

struct invasion_warning_t {
    bool in_use;
    bool handled;
    int invasion_path_id;
    int warning_years;
    vec2i pos;
    int image_id;
    int empire_object_id;
    int year_notified;
    int month_notified;
    int months_to_go;
    int invasion_id;
};

struct invasion_opts_t {
    e_attack_faction mode = ATTACK_TYPE_ENEMIES;
    e_enemy_type enemy_type = ENEMY_0_BARBARIAN;
    int size = 0;
    int invasion_id = 0;
    // FORMATION_ATTACK_*; pak EVENT_ATTACK_TARGET_RANDOM(4) maps to FORMATION_ATTACK_RANDOM(5)
    e_formation_attack_type attack_type = FORMATION_ATTACK_RANDOM;
    tile2i invasion_point;
    uint8_t want_destroy = 0;
};

// Map pak/JS invasion_attack_target (EVENT_ATTACK_TARGET_*) → formation attack_type.
e_formation_attack_type formation_attack_from_event_target(int invasion_attack_target);

// B2 pending: event-manager invasion awaiting wipe/destroy-goal resolve.
// invasion_id is an enemy_army slot (< MAX_ENEMY_ARMIES), never pak event_id.
struct invasion_event_pending_t {
    bool in_use = false;
    bool enemies_seen = false;
    uint8_t invasion_id = 0;
    int16_t event_id = -1;
    uint8_t want_destroy = 0;
};

struct enemy_properties_t {
    int percentage_type1;
    int percentage_type2;
    int percentage_type3;
    std::array<e_figure_type, 3> figure_types;
    xstring army_title;
    e_formation_layout layout;
};
ANK_CONFIG_STRUCT(enemy_properties_t, percentage_type1, percentage_type2, percentage_type3, figure_types, army_title, layout)

struct invasion_data_t {
    enum {
        MAX_PENDING_EVENT_INVASIONS = 32,
        // Prefer slots away from favour-helper hardcodes (24–26) and console (23).
        FIRST_EVENT_INVASION_SLOT = 1,
    };

    int last_internal_invasion_id;
    int min_invasion_amount;
    int max_invasion_amount;
    std::array<invasion_warning_t, 101> warnings;
    std::array<invasion_event_pending_t, MAX_PENDING_EVENT_INVASIONS> event_pending;

    void clear();
    void init();

    const enemy_properties_t &get_prop(e_enemy_type type);

    // Allocate free enemy_army slot in 1..MAX_ENEMY_ARMIES-1; returns -1 if full.
    int alloc_invasion_id();
    bool register_event_pending(uint8_t invasion_id, int16_t event_id, uint8_t want_destroy);
    bool has_pending_for_event(int16_t event_id) const;
    void process_event_resolutions(); // month tick: seen → wipe/refuse → chain
};
ANK_CONFIG_STRUCT(invasion_data_t, min_invasion_amount, max_invasion_amount)
extern invasion_data_t g_invasions;

extern const int LOCAL_UPRISING_NUM_ENEMIES[20];


bool scenario_invasion_exists_upcoming();

void scenario_invasion_foreach_warning(std::function<void(vec2i, int)> callback);

int scenario_invasion_count();

bool scenario_invasion_start_from_kingdome(int size);

void scenario_invasion_start(invasion_opts_t opts);

void scenario_invasion_process();

int map_invasion_point(tile2i point);

tile2i scenario_start_invasion_impl(invasion_opts_t opts);
