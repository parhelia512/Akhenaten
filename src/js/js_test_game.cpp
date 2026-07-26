// Native JS bindings used only by the integraltests driver (see
// src/platform/integral_tests.cpp and tests/*.js). Split out from js_game.cpp
// to keep the test-only surface in one place and make it easy to audit /
// disable later without touching the main game bindings.

#include "js_game.h"

#include "core/app.h"
#include "core/log.h"
#include "js.h"
#include "js_defines.h"
#include "mujs/mujs.h"
#include "platform/arguments.h"
#include "platform/integral_tests.h"
#include "widget/debug_console.h"

#include "building/building.h"
#include "building/building_static_params.h"
#include "building/building_storage_yard.h"
#include "building/monument_mastaba.h"
#include "building/monuments.h"
#include "grid/grid.h"
#include "graphics/view/view.h"
#include "figure/figure.h"
#include "figure/figure_impl.h"
#include "figuretype/figure_missile.h"
#include "figuretype/figure_ostrich_hunter.h"
#include "graphics/color.h"
#include "city/city.h"
#include "city/city_buildings.h"
#include "game/autosave_module.h"
#include "core/bstring.h"
#include "game/game.h"
#include "io/gamestate/boilerplate.h"
#include "game/game_events.h"
#include "scenario/scenario.h"
#include "scenario/scenario_event_manager.h"
#include "scenario/scenario_invasion.h"
#include "scenario/request.h"
#include "figure/enemy_army.h"
#include "figure/formation.h"
#include "window/window_info.h"
#include "empire/empire.h"

#include <SDL.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include "figuretype/figure_kingdome_trader.h"
#include "figuretype/figure_trader_ship.h"
#include "empire/trader_handler.h"
#include "empire/empire_traders.h"

// SDL_main.h does `#define main SDL_main`; undo it here so building::main() etc.
// are not macro-mangled (this TU is not the program entry point).
#ifdef main
#undef main
#endif

static bool file_contains_marker(pcstr path, pcstr marker, const size_t marker_len) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }

    std::string chunk;
    chunk.reserve(8192 + marker_len);
    std::string tail;
    char buf[8192];
    bool strip_bom = true;

    while (in) {
        in.read(buf, sizeof(buf));
        const std::streamsize got = in.gcount();
        if (got <= 0) {
            break;
        }

        chunk = tail;
        chunk.append(buf, static_cast<size_t>(got));

        if (strip_bom && chunk.size() >= 3
            && (unsigned char) chunk[0] == 0xEF
            && (unsigned char) chunk[1] == 0xBB
            && (unsigned char) chunk[2] == 0xBF) {
            chunk.erase(0, 3);
        }
        strip_bom = false;

        if (chunk.find(marker) != std::string::npos) {
            return true;
        }

        if (marker_len <= 1) {
            tail.clear();
        } else if (chunk.size() >= marker_len - 1) {
            tail = chunk.substr(chunk.size() - (marker_len - 1));
        } else {
            tail = chunk;
        }
    }

    return tail.find(marker) != std::string::npos;
}

bool test_log_contains(pcstr marker) {
    if (!marker || !*marker) {
        return false;
    }

    logs::flush();

    const size_t marker_len = std::strlen(marker);
    return file_contains_marker(logs::output_path(), marker, marker_len);
}

static bool __test_find_inlog(pcstr marker) {
    return test_log_contains(marker);
}
ANK_FUNCTION_1(__test_find_inlog);

static void test_pump(int n) {
    for (int i = 0; i < n && !g_app.quit; ++i) {
        g_app.pump_one_frame();
    }
}

void __log_marker(pcstr message) {
    if (g_args.is_integral_tests()) {
        logs::info("[test-marker] %s", message);
    }

    logs::info("%s", message);
}
ANK_FUNCTION_1(__log_marker);

void __test_pump_frames(int n) {
    n = std::clamp(n, 0, 240); // safety bound: ~4s at 60fps
    test_pump(n);
}
ANK_FUNCTION_1(__test_pump_frames);

static void push_mouse_motion(int x, int y) {
    SDL_Event e{};
    e.type = SDL_MOUSEMOTION;
    e.motion.which = 0; // not SDL_TOUCH_MOUSEID
    e.motion.x = x;
    e.motion.y = y;
    e.motion.xrel = 0;
    e.motion.yrel = 0;
    SDL_PushEvent(&e);
}

static void push_mouse_button(Uint32 type, Uint8 button, int x, int y) {
    SDL_Event e{};
    e.type = type;
    e.button.which = 0;
    e.button.button = button;
    e.button.state = (type == SDL_MOUSEBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
    e.button.clicks = 1;
    e.button.x = x;
    e.button.y = y;
    SDL_PushEvent(&e);
}

static void do_synthetic_click(Uint8 sdl_button, int x, int y) {
    push_mouse_motion(x, y);
    push_mouse_button(SDL_MOUSEBUTTONDOWN, sdl_button, x, y);
    test_pump(2);
    push_mouse_button(SDL_MOUSEBUTTONUP, sdl_button, x, y);
    test_pump(8);
}

void __test_mouse_click(int x, int y) {
    do_synthetic_click(SDL_BUTTON_LEFT, x, y);
}
ANK_FUNCTION_2(__test_mouse_click);

void __test_mouse_right_click(int x, int y) {
    do_synthetic_click(SDL_BUTTON_RIGHT, x, y);
}
ANK_FUNCTION_2(__test_mouse_right_click);

void __test_signal_ready() {
    g_test_signal_ready = true;
}
ANK_FUNCTION(__test_signal_ready);

void __test_run_console_command(pcstr command) {
    run_debug_command(command);
}
ANK_FUNCTION_1(__test_run_console_command);

bool __test_start_city_session(pcstr map_path) {
    if (game.session.active) {
        return true;
    }

    if (!GamestateIO::load_map(map_path, false, true)) {
        logs::error("test_start_city_session: load_map(%s) failed", map_path);
        return false;
    }

    GamestateIO::start_loaded_file();
    auto cities = g_empire.get_cities();
    cities[0].type = EMPIRE_CITY_OURS;
    cities[0].in_use = true;

    return game.session.active;
}
ANK_FUNCTION_1(__test_start_city_session);

void __test_end_city_session() {
    game.session.active = false;
}
ANK_FUNCTION(__test_end_city_session);

void __test_set_treasury(int amount) {
    g_city.finance.treasury = amount;
}
ANK_FUNCTION_1(__test_set_treasury);

void __test_process_events() {
    events::process();
}
ANK_FUNCTION(__test_process_events);

void __test_process_scenario_events() {
    g_scenario.events.process_events();
}
ANK_FUNCTION(__test_process_scenario_events);

int __test_request_cleared_seq() {
    return scenario_request_last_cleared().seq;
}
ANK_FUNCTION(__test_request_cleared_seq);

int __test_request_cleared_tag_id() {
    return scenario_request_last_cleared().tag_id;
}
ANK_FUNCTION(__test_request_cleared_tag_id);

int __test_request_cleared_resource() {
    return scenario_request_last_cleared().resource;
}
ANK_FUNCTION(__test_request_cleared_resource);

int __test_request_cleared_fulfilled() {
    return scenario_request_last_cleared().fulfilled;
}
ANK_FUNCTION(__test_request_cleared_fulfilled);

int __test_request_cleared_was_overdue() {
    return scenario_request_last_cleared().was_overdue;
}
ANK_FUNCTION(__test_request_cleared_was_overdue);

static event_ph_t *__test_find_request_by_tag(int tag) {
    for (int i = 0; i < g_scenario.events.events_count(); ++i) {
        event_ph_t *e = g_scenario.events.at(i);
        if (e && e->type == EVENT_TYPE_REQUEST && e->tag_id == tag) {
            return e;
        }
    }
    return nullptr;
}

// Force overdue grace with months_left months remaining (for late-fulfill tests).
void __test_request_force_overdue(int tag, int months_left) {
    event_ph_t *e = __test_find_request_by_tag(tag);
    if (!e || !e->is_active) {
        return;
    }
    e->event_state = e_event_state_overdue;
    e->is_overdue = true;
    e->quest_months_left = (uint8_t)std::max(0, months_left);
}
ANK_FUNCTION_2(__test_request_force_overdue);

// Expire grace immediately → refuse path on next process_active_request.
void __test_request_force_refuse_now(int tag) {
    event_ph_t *e = __test_find_request_by_tag(tag);
    if (!e || !e->is_active) {
        return;
    }
    e->event_state = e_event_state_overdue;
    e->is_overdue = true;
    e->quest_months_left = 0;
    g_scenario.events.process_active_request(e->event_id);
}
ANK_FUNCTION_1(__test_request_force_refuse_now);

void __test_process_invasion_binds() {
    g_invasions.process_bind_resolutions();
}
ANK_FUNCTION(__test_process_invasion_binds);

void __test_clear_enemy_formations() {
    for (int fi = 1; fi < MAX_FORMATIONS; ++fi) {
        formation *m = formation_get(fi);
        if (!m || !m->in_use || m->is_herd || m->own_batalion) {
            continue;
        }
        for (int fig = 0; fig < m->num_figures; ++fig) {
            if (m->figures[fig] > 0) {
                figure *f = figure_get(m->figures[fig]);
                if (f) {
                    f->poof();
                }
            }
        }
        g_formations.clear(fi);
    }
    // Also poof enemy figures that still reference a formation (pre-calculate state).
    for (int i = 1; i < MAX_FIGURES; ++i) {
        figure *f = figure_get(i);
        if (!f || !f->is_alive() || f->formation_id <= 0) {
            continue;
        }
        formation *m = formation_get(f->formation_id);
        if (m && !m->own_batalion && !m->is_herd) {
            f->poof();
        }
    }
    g_formations.calculate_figures();
}
ANK_FUNCTION(__test_clear_enemy_formations);

int __test_count_enemy_figures() {
    g_formations.calculate_figures();
    int n = 0;
    for (int i = 1; i < MAX_FIGURES; ++i) {
        figure *f = figure_get(i);
        if (!f || !f->is_alive() || f->formation_id <= 0) {
            continue;
        }
        formation *m = formation_get(f->formation_id);
        if (m && m->in_use && !m->own_batalion && !m->is_herd) {
            ++n;
        }
    }
    return n;
}
ANK_FUNCTION(__test_count_enemy_figures);

void __test_set_scenario_enemy_id(int enemy_id) {
    g_scenario.enemy_id = (e_enemy_type)enemy_id;
}
ANK_FUNCTION_1(__test_set_scenario_enemy_id);

void __test_set_army_buildings_destroyed(int invasion_id, int count) {
    if (invasion_id <= 0 || invasion_id >= enemy_armies_t::MAX_ENEMY_ARMIES) {
        return;
    }
    enemy_army *army = enemy_army_get_editable((uint8_t)invasion_id);
    army->buildings_destroyed = (uint8_t)std::clamp(count, 0, 255);
}
ANK_FUNCTION_2(__test_set_army_buildings_destroyed);

int __building_static_building_size(int type) {
    if (type <= BUILDING_NONE || type >= BUILDING_MAX) {
        return 0;
    }
    return std::max(1, (int)building_static_params::get((e_building_type)type).building_size);
}
ANK_FUNCTION_1(__building_static_building_size);

static int __test_building_create(int type, int x, int y) {
    if (type <= BUILDING_NONE || type >= BUILDING_MAX) {
        return 0;
    }

    tile2i place = (x < 0 || y < 0) ? tile2i::invalid : tile2i(x, y);
    if (building *existing = building_first((e_building_type)type)) {
        return existing->id;
    }

    if (!place.valid()) {
        place.set(g_scenario.map.width / 2, g_scenario.map.height / 2);
    }

    building *b = building_create((e_building_type)type, place, 0);
    if (!b || b->id <= 0) {
        return 0;
    }

    add_building(b, 0, 0);
    b->state = BUILDING_STATE_VALID;
    return b->id;
}
ANK_FUNCTION_3(__test_building_create);

color __test_color_roundtrip(color c) {
    return c;
}
ANK_FUNCTION_1(__test_color_roundtrip);

static int __test_figure_create(int type, int x, int y) {
    if (type <= FIGURE_NONE || type >= FIGURE_MAX) {
        return 0;
    }

    tile2i place = (x < 0 || y < 0) ? tile2i::invalid : tile2i(x, y);
    if (!place.valid()) {
        place.set(g_scenario.map.width / 2, g_scenario.map.height / 2);
    }

    figure *f = figure_create((e_figure_type)type, place, DIR_0_TOP_RIGHT);
    if (!f || f->id <= 0 || !f->is_alive()) {
        return 0;
    }

    return f->id;
}
ANK_FUNCTION_3(__test_figure_create);

static void __test_figure_set_home(int fid, int bid) {
    figure *f = figure_get(fid);
    building *b = building_get(bid);
    if (!f || !f->is_alive() || !b || !b->is_valid()) {
        return;
    }
    f->set_home(bid);
}
ANK_FUNCTION_2(__test_figure_set_home);

static void __test_figure_set_speed(int fid, int speed) {
    figure *f = figure_get(fid);
    if (!f || !f->is_alive()) {
        return;
    }
    f->speed_multiplier = (uint8_t)std::clamp(speed, 0, 255);
}
ANK_FUNCTION_2(__test_figure_set_speed);

static void __test_figure_set_force_valid_animation(int fid, int enabled) {
    figure *f = figure_get(fid);
    if (!f || !f->is_valid()) {
        return;
    }
    f->set_force_valid_animation(enabled != 0);
}
ANK_FUNCTION_2(__test_figure_set_force_valid_animation);

static int __test_figure_get_damage(int fid) {
    figure *f = figure_get(fid);
    if (!f) {
        return -1;
    }
    return f->damage;
}
ANK_FUNCTION_1(__test_figure_get_damage);

static int __test_count_figures(int type) {
    int count = 0;
    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (f && f->is_valid() && f->type == (e_figure_type)type) {
            count++;
        }
    }
    return count;
}
ANK_FUNCTION_1(__test_count_figures);

// Bypass hunt-animation callback: spawn a hunter arrow at the hunter aimed at its target.
static int __test_hunter_force_shot(int hunter_fid) {
    figure *hunter = figure_get(hunter_fid);
    if (!hunter || !hunter->is_alive() || hunter->type != FIGURE_OSTRICH_HUNTER) {
        return 0;
    }
    if (!hunter->target_figure_id) {
        return 0;
    }
    figure *prey = figure_get(hunter->target_figure_id);
    if (!prey || !prey->is_alive()) {
        return 0;
    }

    const auto &params = figure_ostrich_hunter::current_params();
    auto missile = figure_missile::create(hunter->id, hunter->tile, prey->tile, FIGURE_HUNTER_ARROW);
    if (!missile) {
        return 0;
    }
    missile->runtime_data().missile_attack_value = params.animal_attack_value;
    return missile->id();
}
ANK_FUNCTION_1(__test_hunter_force_shot);

static void __test_figure_set_action(int fid, int action) {
    figure *f = figure_get(fid);
    if (!f || !f->is_alive()) {
        return;
    }
    f->advance_action((short)action);
}
ANK_FUNCTION_2(__test_figure_set_action);

static void __test_figure_update_animation(int fid) {
    figure *f = figure_get(fid);
    if (!f || !f->is_alive()) {
        return;
    }
    figure_impl *impl = f->dcast();
    if (impl) {
        impl->update_animation();
    }
}
ANK_FUNCTION_1(__test_figure_update_animation);

bool __test_enemy_figure_registered(int type) {
    tile2i tile(g_scenario.map.width / 2, g_scenario.map.height / 2);
    figure *f = figure_create((e_figure_type)type, tile, 0);
    if (!f) {
        return false;
    }

    figure_impl *impl = f->dcast();
    bool is_enemy = impl && impl->dcast_enemy();
    f->poof();
    return is_enemy;
}
ANK_FUNCTION_1(__test_enemy_figure_registered);

void __test_show_tile_info(int bid) {
    building *b = building_get(bid);
    if (!b || !b->is_valid()) {
        return;
    }

    events::emit(event_show_tile_info{ b->tile, true, "test" });
}
ANK_FUNCTION_1(__test_show_tile_info);

static e_building_type test_mastaba_params_type(building *head) {
    switch (head->type) {
    case BUILDING_MEDIUM_MASTABA:
    case BUILDING_MEDIUM_MASTABA_SIDE:
    case BUILDING_MEDIUM_MASTABA_WALL:
    case BUILDING_MEDIUM_MASTABA_ENTRANCE:
    case BUILDING_MEDIUM_MASTABA_RESERVED:
        return BUILDING_MEDIUM_MASTABA;
    default:
        return BUILDING_SMALL_MASTABA;
    }
}

static void test_monument_apply_phase(building *head, int phase) {
    auto mm = head->dcast_monument();
    if (!mm) {
        return;
    }
    mm->set_phase(phase);
    for (building *p = head->has_next() ? head->next() : nullptr; p; p = p->has_next() ? p->next() : nullptr) {
        if (auto pm = p->dcast_monument()) {
            pm->set_phase(phase);
        }
    }
}

// Force a monument to `phase`. When advancing forward, walks one phase at a time so
// per-phase hooks (pyramid setup_phase_6, layer type changes, …) all fire — a direct
// jump used to skip them and left the first brick ring with missing edge textures.
static void __test_monument_set_phase(int bid, int phase) {
    building *b = building_get(bid);
    building *head = b ? b->main() : nullptr;
    auto mm = head ? head->dcast_monument() : nullptr;
    if (!mm) {
        return;
    }

    const int cur = (int)mm->runtime_data().phase;
    if (cur >= 0 && phase > cur) {
        for (int p = cur + 1; p <= phase; ++p) {
            test_monument_apply_phase(head, p);
        }
    } else {
        test_monument_apply_phase(head, phase);
    }

    const bool is_mastaba = head->dcast_mastaba();
    const bool is_pyramid = head->dcast_pyramid();
    if (!is_mastaba && !is_pyramid) {
        return;
    }

    // Fill every part's tiles so height ornaments use the current layer (progress >= 200).
    for (building *p = head; p; p = p->has_next() ? p->next() : nullptr) {
        grid_tiles part_tiles = map_grid_get_tiles(p, 0);
        for (auto &t : part_tiles) {
            map_monuments_set_progress(t, 200);
        }
    }

    if (is_mastaba) {
        const vec2i tiles = get_mastaba_params(test_mastaba_params_type(head)).init_tiles;
        if (mm->is_finished() || mm->phase() >= 8 || phase >= 8) {
            building_mastaba::finalize(head, tiles);
        } else if (phase >= 2) {
            building_mastaba::update_images(head, phase, tiles);
        }
    }
}
ANK_FUNCTION_2(__test_monument_set_phase);

static int __test_monument_phase(int bid) {
    building *b = building_get(bid);
    building *head = b ? b->main() : nullptr;
    auto mm = head ? head->dcast_monument() : nullptr;
    // phase is int8_t (MONUMENT_FINISHED = -1); phase() returns uint8_t and would yield 255.
    return mm ? (int)mm->runtime_data().phase : -999;
}
ANK_FUNCTION_1(__test_monument_phase);

// Deliver resource loads into a monument (updates resources_pct like cart delivery).
// amount is resource units (same as deliver_resource); returns false if full / not a monument.
static bool __test_monument_add_resource(int bid, int resource, int amount) {
    building *b = building_get(bid);
    building *head = b ? b->main() : nullptr;
    auto mm = head ? head->dcast_monument() : nullptr;
    if (!mm || resource <= RESOURCE_NONE || resource >= RESOURCES_MAX || amount <= 0) {
        return false;
    }
    return mm->deliver_resource(static_cast<e_resource>(resource), amount);
}
ANK_FUNCTION_3(__test_monument_add_resource);

// Current delivery progress for a resource on the monument (0..100+).
static int __test_monument_resource_pct(int bid, int resource) {
    building *b = building_get(bid);
    building *head = b ? b->main() : nullptr;
    auto mm = head ? head->dcast_monument() : nullptr;
    if (!mm || resource <= RESOURCE_NONE || resource >= RESOURCES_MAX) {
        return -1;
    }
    return mm->runtime_data().resources_pct[resource];
}
ANK_FUNCTION_2(__test_monument_resource_pct);

// Force-stock a storage yard (bypasses accepting/getting rules). Yard must already exist.
static bool __test_storage_yard_add_resource(int bid, int resource, int amount) {
    building *b = building_get(bid);
    auto *yard = b ? b->dcast_storage_yard() : nullptr;
    if (!yard || !b->is_valid() || resource <= RESOURCE_NONE || resource >= RESOURCES_MAX || amount <= 0) {
        return false;
    }
    return yard->add_resource(static_cast<e_resource>(resource), amount, /*force*/true) >= 0;
}
ANK_FUNCTION_3(__test_storage_yard_add_resource);

static int __test_yards_stored(int resource) {
    if (resource <= RESOURCE_NONE || resource >= RESOURCES_MAX) {
        return 0;
    }
    return g_city.resource.yards_stored(static_cast<e_resource>(resource));
}
ANK_FUNCTION_1(__test_yards_stored);

static int __test_yards_stored_staffed(int resource) {
    if (resource <= RESOURCE_NONE || resource >= RESOURCES_MAX) {
        return 0;
    }
    return g_city.resource.yards_stored_staffed(static_cast<e_resource>(resource));
}
ANK_FUNCTION_1(__test_yards_stored_staffed);

static void __test_burial_provisions_clear() {
    for (int r = 0; r < RESOURCES_MAX; r++) {
        g_scenario.monuments.burial_provisions[r].required = 0;
        g_scenario.monuments.burial_provisions[r].dispatched = 0;
    }
}
ANK_FUNCTION(__test_burial_provisions_clear);

static bool __test_burial_provisions_set(int resource, int required) {
    if (resource <= RESOURCE_NONE || resource >= RESOURCES_MAX || required < 0) {
        return false;
    }
    g_scenario.monuments.burial_provisions[resource].required = required;
    g_scenario.monuments.burial_provisions[resource].dispatched = 0;
    return true;
}
ANK_FUNCTION_2(__test_burial_provisions_set);

// Return the current resolved image id for a monument building (per phase + variant + camera).
static int __test_building_current_image(int bid) {
    building *b = building_get(bid);
    auto m = b ? b->dcast_monument() : nullptr;
    return m ? m->building_image_get() : 0;
}
ANK_FUNCTION_1(__test_building_current_image);

// Center the camera on a building: monuments use center_point() (footprint centre),
// other buildings use the footprint middle tile.
static void __test_camera_center_building(int bid) {
    building *b = building_get(bid);
    if (!b || !b->is_valid()) {
        return;
    }
    tile2i c = b->tile.shifted(b->size / 2, b->size / 2);
    if (auto m = b->dcast_monument()) {
        c = m->center_point();
    }
    if (!c.valid()) {
        return;
    }
    vec2i screen = g_camera.tile_to_screen(c);
    g_camera.go_to_screen_tile(screen, true);
}
ANK_FUNCTION_1(__test_camera_center_building);

// Hot-reload stack regression: each js_vm_sync used to leave ~1 stack slot per
// [console_command=…] handler (~30). JS_STACKSIZE is 256, so ~10 reloads then
// crashed in config::refresh / UI archive load (stackoverflow → fatal exit).
static bool __test_js_hotreload_handlers_stack_ok(int iterations) {
    js_State *J = js_vm_state();
    if (!J || iterations < 1) {
        return false;
    }

    const int baseline = js_gettop(J);
    for (int i = 0; i < iterations; ++i) {
        js_register_game_handlers({});
    }
    const int after = js_gettop(J);
    logs::info("[test-marker] hotreload_handlers_stack baseline=%d after=%d iterations=%d",
               baseline, after, iterations);
    if (after != baseline) {
        logs::info("[hotreload] handlers stack leak delta=%d", after - baseline);
        return false;
    }
    return true;
}
ANK_FUNCTION_1(__test_js_hotreload_handlers_stack_ok);

static bool __test_js_hotreload_file_stack_ok(pcstr path, int iterations) {
    js_State *J = js_vm_state();
    if (!J || !path || !*path || iterations < 1) {
        return false;
    }

    const int baseline = js_gettop(J);
    for (int i = 0; i < iterations; ++i) {
        js_vm_reload_file(path);
        js_vm_sync({});
    }
    const int after = js_gettop(J);
    logs::info("[test-marker] hotreload_file_stack path=%s baseline=%d after=%d iterations=%d",
               path, baseline, after, iterations);
    if (after != baseline) {
        logs::info("[hotreload] file reload stack leak delta=%d", after - baseline);
        return false;
    }
    return true;
}
ANK_FUNCTION_2(__test_js_hotreload_file_stack_ok);

static pcstr __test_autosave_format(int slots, int slot) {
    static bstring256 buf;
    buf = autosave_module_t::format_monthly_filename(slots, slot, "svx");
    return buf.c_str();
}
ANK_FUNCTION_2(__test_autosave_format);

static int __test_autosave_pick(int slots, int exists_mask, int m0, int m1, int m2) {
    bool exists[10] = {};
    uint64_t mtime[10] = {};
    if (slots < 1) {
        slots = 1;
    }
    if (slots > 10) {
        slots = 10;
    }
    const int mtimes_in[] = {m0, m1, m2};
    for (int i = 0; i < slots; ++i) {
        exists[i] = (exists_mask & (1 << i)) != 0;
        mtime[i] = exists[i] ? (uint64_t)(i < 3 ? mtimes_in[i] : (100 + i)) : UINT64_MAX;
    }
    return autosave_module_t::pick_slot(slots, exists, mtime);
}
ANK_FUNCTION_5(__test_autosave_pick);

static int __test_trader_capacity(int fid) {
    figure *f = figure_get(fid);
    if (!f || !f->is_valid()) {
        return -1;
    }
    if (auto *caravan = f->dcast<figure_trade_caravan>()) {
        return caravan->max_capacity();
    }
    if (auto *ship = f->dcast<figure_trade_ship>()) {
        return ship->max_capacity();
    }
    return -1;
}
ANK_FUNCTION_1(__test_trader_capacity);

static int __test_trader_static_max_capacity(int type) {
    if (type == FIGURE_TRADE_CARAVAN) {
        const auto &p = (const figure_trade_caravan::static_params &)figure_static_params::get(FIGURE_TRADE_CARAVAN);
        return p.max_capacity;
    }
    if (type == FIGURE_TRADE_SHIP) {
        const auto &p = (const figure_trade_ship::static_params &)figure_static_params::get(FIGURE_TRADE_SHIP);
        return p.max_capacity;
    }
    return -1;
}
ANK_FUNCTION_1(__test_trader_static_max_capacity);

static int __test_trader_static_capacity_random(int type) {
    if (type != FIGURE_TRADE_CARAVAN) {
        return -1;
    }
    const auto &p = (const figure_trade_caravan::static_params &)figure_static_params::get(FIGURE_TRADE_CARAVAN);
    return p.capacity_random;
}
ANK_FUNCTION_1(__test_trader_static_capacity_random);

static void __test_trader_set_bought(int fid, int amount) {
    figure *f = figure_get(fid);
    if (!f || !f->is_valid()) {
        return;
    }
    if (auto *caravan = f->dcast<figure_trade_caravan>()) {
        caravan->runtime_data().amount_bought = (uint16_t)std::max(0, amount);
        return;
    }
    if (auto *ship = f->dcast<figure_trade_ship>()) {
        ship->runtime_data().amount_bought = (uint16_t)std::max(0, amount);
    }
}
ANK_FUNCTION_2(__test_trader_set_bought);

static int __test_trader_buy_under_capacity(int fid) {
    figure *f = figure_get(fid);
    if (!f || !f->is_valid()) {
        return -1;
    }
    if (auto *caravan = f->dcast<figure_trade_caravan>()) {
        return caravan->total_bought() < caravan->max_capacity() ? 1 : 0;
    }
    if (auto *ship = f->dcast<figure_trade_ship>()) {
        return ship->total_bought() < ship->max_capacity() ? 1 : 0;
    }
    return -1;
}
ANK_FUNCTION_1(__test_trader_buy_under_capacity);

static int __test_empire_trader_has_traded_max(int bought, int sold, int capacity) {
    empire_trader_handle h = empire_create_trader();
    if (!h.valid()) {
        return -1;
    }
    auto &t = g_empire_traders.traders[h.handle];
    t.bought_amount = (uint16_t)std::max(0, bought);
    t.sold_amount = (uint16_t)std::max(0, sold);
    const int result = h.has_traded_max(capacity) ? 1 : 0;
    t.is_active = false;
    return result;
}
ANK_FUNCTION_3(__test_empire_trader_has_traded_max);

ANK_DECLARE_JSFUNCTION_ITERATOR(register_test_js_functions);
inline void register_test_js_functions(js_State *J) {
}
