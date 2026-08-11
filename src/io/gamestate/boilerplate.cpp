#include "boilerplate.h"

#include "building/construction/build_planner.h"
#include "building/building_granary.h"
#include "building/building_storage.h"
#include "building/building_wall.h"
#include "building/monuments.h"
#include "game/game_config.h"
#include "game/autosave_module.h"
#include "city/city.h"
#include "city/map.h"
#include "city/city_message.h"
#include "city/city_hotkeys_handler.h"
#include "city/campaign_carry.h"
#include "city/military.h"
#include "city/city_resource.h"
#include "core/bstring.h"
#include "content/vfs.h"
#include "empire/empire.h"
#include "empire/empire_map.h"
#include "empire/type.h"
#include "empire/trade_prices.h"
#include "figure/enemy_army.h"
#include "figure/figure_names.h"
#include "figure/route.h"
#include "empire/trader_handler.h"
#include "game/mission.h"
#include "game/undo.h"
#include "graphics/image.h"
#include "grid/canals.h"
#include "grid/trees.h"
#include "grid/bookmark.h"
#include "grid/building.h"
#include "grid/desirability.h"
#include "grid/elevation.h"
#include "grid/figure.h"
#include "grid/image.h"
#include "grid/gardens.h"
#include "grid/image_context.h"
#include "grid/orientation.h"
#include "grid/property.h"
#include "grid/random.h"
#include "grid/road_network.h"
#include "grid/routing/routing_terrain.h"
#include "grid/soldier_strength.h"
#include "grid/sprite.h"
#include "grid/bridge_grid.h"
#include "grid/terrain.h"
#include "grid/tiles.h"
#include "grid/wall_material.h"
#include "grid/moisture.h"
#include "grid/floodplain.h"
#include "grid/basin.h"
#include "grid/water.h"
#include "grid/sandstone.h"
#include "grid/stone.h"
#include "grid/limestone.h"
#include "grid/granite.h"
#include "grid/golden.h"
#include "grid/copper.h"
#include "grid/clay.h"
#include "grid/gems.h"
#include "game/game.h"
#include "editor/editor.h"
#include "scenario/criteria.h"
#include "scenario/demand_change.h"
#include "scenario/distant_battle.h"
#include "scenario/earthquake.h"
#include "scenario/editor_map_meta.h"
#include "scenario/farao_change.h"
#include "scenario/scenario_revolt.h"
#include "scenario/scenario_invasion.h"
#include "scenario/invasion_auto_resolve.h"
#include "scenario/price_change.h"
#include "scenario/request.h"
#include "scenario/scenario.h"
#include "sound/sound_city.h"
#include "sound/sound.h"
#include "js/js_game.h"
#include "widget/widget_figure_follow.h"
#include "window/window_city.h"
#include "window/file_dialog_common.h"
#include "game/game_events.h"
#include "empire/empire.h"
#include "city/city_warnings.h"
#include "city/city_recorded_paths.h"
#include "empire/empire_traders.h"

#include "chunks.h"
#include "file_schemas.h"
#include "city/coverage.h"
#include "city/city_floods.h"
#include "core/log.h"
#include "core/profiler.h"
#include "io/io.h"
#include "io/chunk_serializer.h"
#include "js/js_game.h"
#include "dev/debug.h"
#include "window/popup_dialog.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <vector>
#include <cstdio>

#ifdef _MSC_VER
// not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#include <direct.h>
#else
#include <sys/stat.h>
#endif

static pcstr MISSION_PACK_FILE = "mission1.pak";

uint32_t save_data_version() {
    return latest_save_version;
}

uint32_t svx_container_version() {
    return svx::CONTAINER_REV;
}

vfs::path fullpath_saves(vfs::path filename) {
    if (strncasecmp(filename, "Save/", 5) == 0 || strncasecmp(filename, "Save\\", 5) == 0) {
        return vfs::path(filename);
    }

    return vfs::path(vfs::SAVE_FOLDER, "/", game_features::gameopt_player_name.to_string().c_str(), "/", filename);
}

vfs::path fullpath_maps(vfs::path filename) {
    if (strncasecmp(filename, "Maps/", 5) == 0 || strncasecmp(filename, "Maps\\", 5) == 0) {
        return filename;
    }
    return vfs::path(vfs::MAPS_FOLDER, "/", filename.c_str());
}

static buffer* small_buffer = new buffer(4);
int GamestateIO::get_campaign_scenario_offset(int scenario_id) {
    // init 4-byte buffer and read from file header corresponding to scenario index (i.e. mission 20 = offset 20*4 = 80)
    small_buffer->clear();
    if (!io_read_file_part_into_buffer(MISSION_PACK_FILE, NOT_LOCALIZED, small_buffer, 4, 4 * scenario_id))
        return 0;
    return small_buffer->read_i32();
}
const int GamestateIO::read_file_version(const char* filename, int offset) {
    small_buffer->clear();
    if (!io_read_file_part_into_buffer(filename, NOT_LOCALIZED, small_buffer, 4, offset + 4))
        return -1;
    return small_buffer->read_i32();
}

static void pre_load() { // do we NEED this...?
    OZZY_PROFILER_FUNCTION();
    g_scenario.campaign_scenario_id = -1;
    grid_xx::init_all_grids();

    // clear data
    g_city.victory.reset();
    g_city.migration.reset();
    g_city_planner.reset();
    g_city.init();
    g_message_manager.init();
    game.init_state();
    game.animation_timers_init();
    sound_city_init();
    building_clear_all();
    building_storage_clear_all();
    g_city.figures.init_figures();
    g_enemy_armies.clear();
    figure_name_init();
    g_formations.clear_all();
    figure_route_clear_all();
    g_recorded_paths.clear();
    map_clear_floodplain_growth();
    // Troop carry lives outside city_data; clear so old saves (no chunk) and map
    // loads cannot inherit a stale snapshot from the previous session.
    g_campaign_carry.clear();

    game.time_init(2098);
    map_monuments_clear();
    // clear grids
    map_image_clear();
    map_building_clear();
    map_terrain_clear();
    map_canal_clear();
    map_figure_clear();
    map_property_clear();
    map_sprite_clear();
    map_bridge_grids_clear();
    map_random_clear();
    g_desirability.clear_map();
    map_elevation_clear();
    map_soldier_strength_clear();
    map_road_network_clear();

    map_image_context_init();
    map_random_init();
    map_tiles_gardens_clear_all();

    events::emit(event_game_mission_pre_load{});
    events::process();

    figure_follow_stop(false);
}

static void post_load() {
    city_set_player_name(game_features::gameopt_player_name.to_string());
    mission_id_t missionid(g_scenario.campaign_scenario_id);

    bool is_new_mission = (game.session.last_loaded == e_session_mission);
    if (is_new_mission) {
        g_scenario.vars.clear();
    }

    g_scenario.load_metadata(missionid, is_new_mission);
    js_register_mission_vars(g_scenario.vars);

    g_empire.load_mission_metadata(missionid, /*preserve_runtime=*/!is_new_mission);
    js_register_game_handlers(missionid.value());

    // camera
    //    g_camera.refresh_camera_position();
    //    city_view_refresh_viewport();

    // problems / overlays
    g_message_manager.init_problem_areas();
    g_city.reset_overlay();
    game_undo_disable();

    g_distant_battle.determine_distant_battle_city();

    int karmy = g_empire.init_distant_battle_travel_months(EMPIRE_OBJECT_KINGDOME_ARMY);
    g_scenario.distant_battle_set_kingdome_travel_months(karmy);

    int kenemy = g_empire.init_distant_battle_travel_months(EMPIRE_OBJECT_ENEMY_ARMY);
    g_scenario.distant_battle_set_enemy_travel_months(kenemy);

    //map
    map_image_fix_icorrect_tiles();
    map_bridge_migrate_from_sprite();
    map_wall_material_migrate_from_terrain();

    // Enhanced auto-resolve: honor feature after load, drop ghost pending, reopen UI.
    g_invasion_auto_resolve.on_after_load();

    // building counts / storage
    city_resource_determine_available();
    g_city.update_allowed_foods();
    g_city.buildings.update_counters();
    g_city.on_post_load();
    g_city.resource.calculate_stocks();
    city_resource_calculate_storageyard_stocks();
    building_storage_reset_building_ids();
    g_city.avg_coverage.update();
    g_empire_traders.purge_dead();

    g_city.religion.ra_no_traders_months_left = std::clamp<int>(g_city.religion.ra_no_traders_months_left, 0, 12);
    g_city.religion.ra_harshly_reduced_trading_months_left = std::clamp<int>(g_city.religion.ra_harshly_reduced_trading_months_left, 0, 12);

    // Save/map chunks already hold prices; reset only for a fresh mission start.
    if (is_new_mission) {
        trade_prices_reset();
    }

    // city data special cases
    switch (game.session.last_loaded) {
    default:
        assert(false);
        break;

    case e_session_mission:
        game_features::gameopt_game_speed.set( 80 );
        g_city.init_campaign_mission();
        g_city.init_mission_resources(g_scenario.init_resources);
        // Pass JS player_rank from load_metadata � not campaign_scenario_id.
        g_city.kingdome.load_scenario(g_city.kingdome.player_rank, game.session.last_loaded);
        events::emit(event_building_menu_update{ "stage_normal" });
        break;

    case e_session_save:
        events::emit(event_building_menu_update{ "stage_normal" });
        break;

    case e_session_custom_map:
        g_city.init_custom_map();
        g_city.init_mission_resources(g_scenario.init_resources);
        g_city.kingdome.load_scenario(g_city.kingdome.player_rank, game.session.last_loaded);
        events::emit(event_building_menu_update{ "stage_normal" });
        break;
    }

    // city messages
    city_message_clear_scroll();

    // city sounds
    sound_city_init();
    map_tiles_gardens_update_all();

    events::emit(event_level_post_load{
        static_cast<int>(game.session.last_loaded),
        g_scenario.campaign_scenario_id});
}



bool GamestateIO::write_mission(const int scenario_id) {
    // TODO?
    return false;
}

bool GamestateIO::write_family_marker(pcstr filename_short) {
    vfs::path full = fullpath_saves(filename_short);
    vfs::path folders = vfs::content_path(fullpath_saves("").c_str());

    logs::info("Save player data: writing %s (dir %s)", full.c_str(), folders.c_str());
    vfs::create_folders(folders);

    // FILE_FORMAT_SAVE_FILE (.sav) stub — not a city save, so no Ironwill / last_save update.
    const bool save_ok = g_chunk_io.serialize(full, 0, FILE_FORMAT_SAVE_FILE, save_data_version(),
                                              file_schema_family_marker);
    if (save_ok) {
        logs::info("Save player data: OK %s", full.c_str());
    } else {
        logs::error("Save player data: FAILED %s", full.c_str());
    }
    return save_ok;
}

bool GamestateIO::write_savegame(pcstr filename_short) {
    // Gate by flag + whitelist only (session.active is not cleared on main menu — E4/R6).
    if (game_features::gameopt_ironwill.to_bool()
        && !autosave_module_t::is_ironwill_exempt_save(filename_short)) {
        logs::info("Save game: blocked by Ironwill (%s)", filename_short ? filename_short : "");
        return false;
    }

    vfs::path full = fullpath_saves(filename_short);

    logs::info("Save game: writing %s", full.c_str());

    e_file_format format = get_format_from_file(filename_short);
    assert(format == FILE_FORMAT_SAVE_FILE_EXT);
    bool save_ok = g_chunk_io.serialize(full, 0, format, save_data_version(), file_schema);
    if (save_ok) {
        if (!autosave_module_t::is_monthly_filename(filename_short)) {
            game_features::gameopt_last_save_filename = full.c_str();
            game_features::gameopt_last_player = game_features::gameopt_player_name.to_string();
            game_features::save();
        }
        logs::info("Save game: OK %s", full.c_str());
    } else {
        logs::error("Save game: FAILED %s", full.c_str());
    }

    return save_ok;
}

bool GamestateIO::write_map(pcstr filename_short) {
    vfs::path full = fullpath_maps(filename_short);

    // write file
    return g_chunk_io.serialize(full, 0, FILE_FORMAT_MAP_FILE, 160, file_schema);
}

bool GamestateIO::write_map_path(pcstr path) {
    if (!path || !path[0]) {
        return false;
    }
    return g_chunk_io.serialize(path, 0, FILE_FORMAT_MAP_FILE, 160, file_schema);
}

bool GamestateIO::export_mission_map(const int scenario_id, pcstr path) {
    if (!load_mission_pak_raw(scenario_id)) {
        return false;
    }
    return write_map_path(path);
}

bool GamestateIO::load_mission_pak_raw(const int scenario_id) {
    const int offset = get_campaign_scenario_offset(scenario_id);
    if (offset <= 0) {
        return false;
    }

    // mission pack files do not store carry savings / troops / campaign rank; preserve across pre_load().
    const uint16_t saved_carry = g_city.kingdome.campaign_carry_personal_savings;
    const int32_t saved_rank = g_scenario.campaign_mission_rank;
    const campaign_carry_t saved_troops = g_campaign_carry;

    pre_load();
    vfs::path mission_pak_path = vfs::path(MISSION_PACK_FILE).resolve();
    auto mission_pak = vfs::file_open(mission_pak_path);
    if (!g_chunk_io.unserialize(mission_pak, offset, FILE_FORMAT_MISSION_PAK, GamestateIO::read_file_version, file_schema)) {
        return false;
    }

    g_city.kingdome.campaign_carry_personal_savings = saved_carry;
    g_scenario.campaign_mission_rank = saved_rank;
    g_campaign_carry = saved_troops;

    game.session.last_loaded = e_session_mission;
    game.session.last_loaded_mission = MISSION_PACK_FILE;
    g_scenario.campaign_scenario_id = scenario_id;

    g_scenario.scmode = e_scenario_normal;
    return true;
}

bool GamestateIO::load_mission_map_raw(const int scenario_id, pcstr map_path) {
    if (!map_path || !map_path[0]) {
        return false;
    }

    // Same carry/rank/troops preservation as load_mission_pak_raw.
    const uint16_t saved_carry = g_city.kingdome.campaign_carry_personal_savings;
    const int32_t saved_rank = g_scenario.campaign_mission_rank;
    const campaign_carry_t saved_troops = g_campaign_carry;

    pre_load();
    vfs::path full = vfs::path(map_path).resolve();
    auto mapfile = vfs::file_open(full);
    if (!g_chunk_io.unserialize(mapfile, 0, FILE_FORMAT_MAP_FILE, GamestateIO::read_file_version, file_schema)) {
        return false;
    }

    g_city.kingdome.campaign_carry_personal_savings = saved_carry;
    g_scenario.campaign_mission_rank = saved_rank;
    g_campaign_carry = saved_troops;

    // Campaign session so post_load() applies JS metadata as a new mission (not custom-map hacks).
    game.session.last_loaded = e_session_mission;
    game.session.last_loaded_mission = map_path;
    g_scenario.campaign_scenario_id = scenario_id;
    g_scenario.scmode = e_scenario_normal;
    return true;
}

bool GamestateIO::load_mission(const int scenario_id, bool start_immediately) {
    xstring map_file;
    g_config_arch.r_section(mission_id_t(scenario_id), [&](archive arch) {
        map_file = arch.r_string("map_file");
    });

    bool loaded = false;
    bool map_file_missing = false;
    if (!map_file.empty()) {
        const vfs::path map_full = vfs::path(map_file.c_str()).resolve();
        if (!vfs::file_exists(map_full)) {
            map_file_missing = true;
            logs::warn("Mission %d: map file missing: %s (falling back to %s)",
                       scenario_id, map_file.c_str(), MISSION_PACK_FILE);
        } else {
            loaded = load_mission_map_raw(scenario_id, map_file.c_str());
            if (!loaded) {
                logs::warn("Mission %d: map_file '%s' failed to load, falling back to %s",
                           scenario_id, map_file.c_str(), MISSION_PACK_FILE);
            }
        }
    }
    if (!loaded && !load_mission_pak_raw(scenario_id)) {
        return false;
    }

    // Scenario selection loads with start_immediately=false, so start_loaded_file()
    // (which also calls init_cities) never runs. post_load() needs EMPIRE_CITY_OURS via
    // update_allowed_foods() � rebuild cities from empire objects first.
    g_empire.init_cities();

    post_load();

    g_empire.fix_trade_routes();

    // finish loading and start
    if (start_immediately) {
        start_loaded_file();

        // replay mission autosave file
        bstring256 filename("autosave_replay.", saved_game_data_expanded.extension);
        GamestateIO::write_savegame(filename);
    }

    if (map_file_missing) {
        bstring512 body;
        body.printf("Map file not found:\n%s\n\nFalling back to %s.",
                    map_file.c_str(), MISSION_PACK_FILE);
        popup_dialog::show_ok(lang_text_from_key("#popup_dialog_map_file_missing"), body.c_str());
    }

    return true;
}

bool GamestateIO::load_savegame(pcstr filename_short, bool start_immediately) {
    // concatenate string
    bstring256 full = fullpath_saves(filename_short).resolve();

    if (!vfs::file_exists(full.c_str())) {
        logs::error("Load game: file not found %s", full.c_str());
        return false;
    }

    // read file
    pre_load();
    e_file_format file_format = get_format_from_file(filename_short);
    auto savegame = vfs::file_open(full);
    if (!g_chunk_io.unserialize(savegame, 0, file_format, GamestateIO::read_file_version, file_schema)) {
        return false;
    }

    game.session.last_loaded = e_session_save;
    game.session.last_loaded_mission = filename_short;
    post_load();

    // finish loading and start
    if (start_immediately) {
        start_loaded_file();
    }

    return true;
}

bool GamestateIO::load_map(pcstr filename, bool relative, bool start_immediately) {
    OZZY_PROFILER_FUNCTION();
    vfs::path fullpath;
    if (relative) {
        fullpath = fullpath_maps(filename).resolve();
    } else {
        fullpath = vfs::path(filename).resolve();
    }

    // read file
    pre_load();
    auto mapfile = vfs::file_open(fullpath);
    if (!g_chunk_io.unserialize(mapfile, 0, FILE_FORMAT_MAP_FILE, GamestateIO::read_file_version, file_schema)) {
        return false;
    }

    game.session.last_loaded = e_session_custom_map;
    game.session.last_loaded_mission = filename;
    g_scenario.campaign_scenario_id = get_custom_mission_id(filename);
    // temp hack, custom map missions have no cities
    auto cities = g_empire.get_cities();
    cities[0].type = EMPIRE_CITY_OURS;
    cities[0].in_use = true;
    // temp hack

    // ED5: custom-map play loads *.meta.js (requests/invasions/price/demand)
    // before start_loaded_file inits. Editor load applies meta itself after strip.
    if (!editor_is_active()) {
        editor_invasions_clear();
        editor_price_changes_clear();
        editor_demand_changes_clear();
        if (vfs::file_exists(editor_map_meta_path(fullpath.c_str()))) {
            g_scenario.events.clear_for_editor();
            editor_map_meta_load(fullpath.c_str());
        }
    }

    post_load();

    if (start_immediately) {
        start_loaded_file();
        bstring256 filename("autosave_replay.", saved_game_data_expanded.extension);
        GamestateIO::write_savegame(filename);
    }

    return true;
}

void GamestateIO::start_loaded_file() {
    // build the map grids when loading MAP files
    if (game.session.last_loaded != e_session_save) {
        // initialize grids
        map_tiles_update_all_elevation();
        map_tiles_river_refresh_entire();
        map_tiles_update_all_earthquake();
        map_tiles_update_all_rocks();
        map_sandstone_init();
        map_stone_init();
        map_limestone_init();
        map_granite_init();
        map_golden_init();
        map_copper_init();
        map_gems_init();
        map_clay_init();
        map_tiles_add_entry_exit_flags();
        map_tiles_update_all_cleared_land();
        map_tiles_update_all_empty_land();
        map_tiles_update_all_meadow();
        map_tiles_update_all_roads();
        map_tiles_update_all_plazas();
        map_canal_update_all_tiles(0);

        //        map_natives_init();
        g_city.fishing_points.create();
        g_city.animals.create_herds();

        g_city.map.entry_point = scenario_map_entry();
        g_city.map.exit_point = scenario_map_exit();

        game.time_init(g_scenario.start_year);

        // traders / empire
        g_empire_map.init_scenario();
        g_empire.init_cities();
        g_empire_traders.init();

        // set up events
        scenario_earthquake_init();
        scenario_revolt_init();
        scenario_kingdome_change_init();
        scenario_criteria_init_max_year();
        g_invasions.init();
        g_distant_battle.determine_distant_battle_city();
        scenario_request_init();
        scenario_demand_change_init();
        scenario_price_change_init();
    }

    map_moisture_recompute_profile();

    g_camera.init();
    map_orientation_update_buildings();
    map_terrain_init_outside_map();
    map_normalize_outside_diamond_all();

    // river / garden tiles refresh
    build_terrain_caches();
    map_basin_rebuild();
    canals_reset_decay_phase();

    // routing
    map_routing_update_all();
    figure_route_clean();
    g_city.map.update_road_network();
    map_routing_update_ferry_routes();
    g_city.maintenance.check_kingdome_access();

    // tiles
    g_floods.init();
    map_water_rebuild_shores();
    map_tiles_update_floodplain_images();
    map_tiles_river_refresh_entire();
    map_tiles_gardens_determine();
    map_tiles_upadte_all_marshland_tiles();
    map_tree_update_all_tiles();
    map_building_update_all_tiles();

    autoconfig_window::before_mission_start();
    events::emit(event_mission_start{ g_scenario.campaign_scenario_id });

    if (game.session.last_loaded == e_session_mission) {
        g_campaign_carry.activate_for_mission(g_scenario.carry_troops_mask);
        g_campaign_carry.post_notice_if_needed();
        // Monument store survives missions without carry_monuments (e.g. Maritis 50).
        if (g_scenario.carry_monuments) {
            g_campaign_carry.apply_monuments();
        }
    }

    g_city.before_start_simulation();
    game.before_start_simulation();
    game.session.active = true;

    const bool show_briefing = (game.session.last_loaded == e_session_mission);
    if (show_briefing) {
        events::emit(event_mission_briefing_show_after_load{ g_scenario.campaign_scenario_id });
        events::process();
    } else {
        game.paused = false;
        window_city_show();
        g_sound.music_update(true);
    }
    game.session.last_loaded = e_session_none;
}

bool GamestateIO::delete_mission(const int scenario_id) {
    // TODO?
    return false;
}

bool GamestateIO::delete_savegame(vfs::path filename_short) {
    vfs::path full = fullpath_saves(filename_short);
    return vfs::file_remove(full);
}

bool GamestateIO::delete_map(const char* filename_short) {
    vfs::path full = fullpath_maps(filename_short);
    // ED4b sidecar next to map (Maps/foo.map ? Maps/foo.meta.js).
    vfs::path meta = full;
    char *dot = std::strrchr(meta.data(), '.');
    if (dot) {
        std::snprintf(dot, (size_t)(vfs::path::capacity - (dot - meta.data())), ".meta.js");
        if (vfs::file_exists(meta)) {
            vfs::file_remove(meta.c_str());
        }
    }
    return vfs::file_remove(full);
}

declare_console_command_p(save_map) {
    std::string filename;

    // Read filename from input stream
    if (!(is >> filename)) {
        os << "Error: Please provide a filename.\n";
        os << "Usage: save_map <filename>\n";
        os << "Example: save_map my_custom_map\n";
        return;
    }

    // Check if we're in a valid game state
    if (!game.session.active) {
        os << "Error: No active game session. Please load a map or start a mission first.\n";
        return;
    }

    bool success = GamestateIO::write_map(filename.c_str());

    os << (success
           ? "Map saved successfully!\n"
           : "Error: Failed to save map.\n");
}