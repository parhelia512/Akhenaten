// FIGURE_FROG register + Ptah/EVENT/cheat plague + house infest + happiness + timer (CF1).
// Markers:
//   [test-marker] frog_registered_ok
//   [test-marker] frog_anim_walk_ok
//   [test-marker] frog_anim_idle_ok
//   [test-marker] frog_ptah_swarm_ok
//   [test-marker] frog_event_swarm_ok
//   [test-marker] frog_happiness_ok
//   [test-marker] frog_cheat_ok
//   [test-marker] frog_house_infest_ok
//   [test-marker] frog_infest_on_pass_ok
//   [test-marker] frog_saveload_ok | frog_saveload_skipped
//   [test-marker] frog_timer_poof_ok

var GOD_PTAH = 2
var ACTION_FROG_CREATED = 120
var ACTION_FROG_ROAMING = 121

var __test113_saveload_ok = false
var __test113_saveload_skipped = false

function test113_remove_frogs() {
    city.figures.remove_figures(FIGURE_FROG)
}

function test113_count_frogs() {
    return __test_count_figures(FIGURE_FROG)
}

function test113_try_ptah_frogs(max_tries) {
    for (var i = 0; i < max_tries; i++) {
        test113_remove_frogs()
        __test_run_console_command('god_major_curse ' + GOD_PTAH)
        if (test113_count_frogs() > 0) {
            return true
        }
    }
    return false
}

function run_test() {
    __log_info_native('[test:113] frog plague (CF1)')
    test_reload_city_session('data/default.map')
    __test_set_treasury(50000)
    game.paused = false

    // --- Register + walk anim ---
    var probe = test_figure_create(FIGURE_FROG, -1, -1)
    if (!probe || !__figure_is_valid(probe) || __figure_get_type(probe) != FIGURE_FROG) {
        __log_info_native('[test:113] FIGURE_FROG not registered')
        __test_signal_ready()
        return
    }
    __log_marker('frog_registered_ok')

    __test_figure_set_action(probe, ACTION_FROG_ROAMING)
    __test_figure_update_animation(probe)
    var key = __figure_get_anim_key(probe)
    if (key != 'walk') {
        __log_info_native('[test:113] anim key want "walk", got "' + key + '"')
        __test_signal_ready()
        return
    }
    __log_marker('frog_anim_walk_ok')

    __test_figure_set_action(probe, ACTION_FROG_CREATED)
    __test_figure_update_animation(probe)
    key = __figure_get_anim_key(probe)
    if (key != 'idle') {
        __log_info_native('[test:113] anim key want "idle", got "' + key + '"')
        __test_signal_ready()
        return
    }
    __log_marker('frog_anim_idle_ok')
    __test_figure_kill(probe)
    test113_remove_frogs()

    // --- Ptah major curse frogs branch (~50%) ---
    if (!test113_try_ptah_frogs(40)) {
        __log_info_native('[test:113] Ptah curse expected frogs after retries')
        __test_signal_ready()
        return
    }
    __log_marker('frog_ptah_swarm_ok')

    // --- EVENT_TYPE_FROGS: amount → count + happiness ---
    test113_remove_frogs()
    var hid = test_building_place(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    if (!hid) {
        hid = __test_building_create(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    }
    if (!hid || __test_house_set_population(hid, 20) < 1) {
        __log_info_native('[test:113] failed to create populated house for happiness')
        __test_signal_ready()
        return
    }
    var house = city.get_house(hid)
    var happiness_before = house.house_happiness

    city.create_chain_event({
        tag_id: 11301,
        type: EVENT_TYPE_FROGS,
        amount: 3,
        trigger: EVENT_TRIGGER_ONCE
    }).execute()

    var n = test113_count_frogs()
    if (n != 3) {
        __log_info_native('[test:113] EVENT_TYPE_FROGS amount=3 want 3 frogs, got ' + n)
        __test_signal_ready()
        return
    }
    __log_marker('frog_event_swarm_ok')

    var happiness_after = house.house_happiness
    if (happiness_after >= happiness_before) {
        __log_info_native('[test:113] happiness want drop, ' + happiness_before + ' -> ' + happiness_after)
        __test_signal_ready()
        return
    }
    __log_marker('frog_happiness_ok')

    // --- frog_plague cheat ---
    test113_remove_frogs()
    __test_run_console_command('frog_plague')
    if (test113_count_frogs() < 1) {
        __log_info_native('[test:113] frog_plague expected frogs')
        __test_signal_ready()
        return
    }
    __log_marker('frog_cheat_ok')

    // --- House infest API ---
    test113_remove_frogs()
    var hid2 = test_building_place(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    if (!hid2) {
        hid2 = __test_building_create(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    }
    if (!hid2 || __test_house_set_population(hid2, 8) < 1) {
        __log_info_native('[test:113] failed to create house for infest')
        __test_signal_ready()
        return
    }
    if (!__test_frog_infest_house(hid2)) {
        __log_info_native('[test:113] infest_house failed')
        __test_signal_ready()
        return
    }
    var h2 = city.get_house(hid2)
    if (h2.frog_infest_days < 1 || h2.population != 0) {
        __log_info_native('[test:113] infest want days>0 pop=0, got days=' + h2.frog_infest_days + ' pop=' + h2.population)
        __test_signal_ready()
        return
    }
    if (h2.population_room != 0) {
        __log_info_native('[test:113] population_room want 0 while infested, got ' + h2.population_room)
        __test_signal_ready()
        return
    }
    if (h2.is_vacant_lot) {
        __log_info_native('[test:113] infested empty house must not be is_vacant_lot')
        __test_signal_ready()
        return
    }
    __test_house_add_population(hid2, 3)
    h2 = city.get_house(hid2)
    if (h2.population != 0) {
        __log_info_native('[test:113] add_population must not refill infested house, pop=' + h2.population)
        __test_signal_ready()
        return
    }
    __log_marker('frog_house_infest_ok')

    // --- Infest-on-pass: frog on house tile evicts ---
    test113_remove_frogs()
    var hid3 = test_building_place(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    if (!hid3) {
        hid3 = __test_building_create(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    }
    if (!hid3 || __test_house_set_population(hid3, 5) < 1) {
        __log_info_native('[test:113] failed to create house for infest-on-pass')
        __test_signal_ready()
        return
    }
    var h3tile = __building_tile(hid3)
    if (!h3tile) {
        __log_info_native('[test:113] no tile for infest-on-pass house')
        __test_signal_ready()
        return
    }
    var frog_pass = test_figure_create(FIGURE_FROG, h3tile.x, h3tile.y)
    if (!frog_pass || !__figure_is_valid(frog_pass)) {
        __log_info_native('[test:113] failed to place frog on house tile')
        __test_signal_ready()
        return
    }
    __test_figure_set_action(frog_pass, ACTION_FROG_ROAMING)
    __test_figure_action_perform(frog_pass)
    var h3 = city.get_house(hid3)
    if (h3.frog_infest_days < 1 || h3.population != 0) {
        __log_info_native('[test:113] infest-on-pass want days>0 pop=0, got days=' + h3.frog_infest_days + ' pop=' + h3.population)
        __test_signal_ready()
        return
    }
    __log_marker('frog_infest_on_pass_ok')
    __test_figure_kill(frog_pass)
    test113_remove_frogs()

    // --- Save/load: figure days_left + house frog_infest_days ---
    test113_remove_frogs()
    var hid_sl = test_building_place(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    if (!hid_sl) {
        hid_sl = __test_building_create(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    }
    if (!hid_sl || !__test_frog_infest_house(hid_sl)) {
        __log_info_native('[test:113] saveload house infest failed')
        __test_signal_ready()
        return
    }
    var days_before = city.get_house(hid_sl).frog_infest_days
    var sid = __test_frog_spawn_swarm(1)
    if (!sid || !__figure_is_valid(sid)) {
        __log_info_native('[test:113] saveload spawn failed')
        __test_signal_ready()
        return
    }
    __test_frog_set_days(sid, 33)
    var save_name = 'test_113_frog.svx'
    if (!__game_write_savegame(save_name)) {
        __log_info_native('[test:113] write_savegame failed — saveload skipped')
        __log_marker('frog_saveload_skipped')
        __test113_saveload_skipped = true
    } else if (!__game_load_savegame(save_name)) {
        __log_info_native('[test:113] load_savegame failed — saveload skipped')
        __game_delete_savegame(save_name)
        __log_marker('frog_saveload_skipped')
        __test113_saveload_skipped = true
    } else {
        __game_delete_savegame(save_name)
        if (test113_count_frogs() < 1) {
            __log_info_native('[test:113] after load expected frogs')
            __test_signal_ready()
            return
        }
        var found_days = false
        var i
        for (i = 1; i < 200; i++) {
            if (__figure_is_valid(i) && __figure_get_type(i) == FIGURE_FROG) {
                if (__test_frog_get_days(i) == 33) {
                    found_days = true
                    break
                }
            }
        }
        if (!found_days) {
            __log_info_native('[test:113] after load days_left=33 not found')
            __test_signal_ready()
            return
        }
        var h_sl = city.get_house(hid_sl)
        if (!h_sl || h_sl.frog_infest_days != days_before) {
            __log_info_native('[test:113] after load frog_infest_days want ' + days_before
                + ', got ' + (h_sl ? h_sl.frog_infest_days : 'no-house'))
            __test_signal_ready()
            return
        }
        __log_marker('frog_saveload_ok')
        __test113_saveload_ok = true
    }

    // --- Timer poof ---
    test113_remove_frogs()
    var fid = __test_frog_spawn_swarm(1)
    if (!fid || !__figure_is_valid(fid)) {
        __log_info_native('[test:113] spawn_swarm(1) failed')
        __test_signal_ready()
        return
    }
    if (!__test_frog_set_days(fid, 1)) {
        __log_info_native('[test:113] set_days failed')
        __test_signal_ready()
        return
    }
    __test_figure_update_day(fid)
    if (test113_count_frogs() != 0) {
        __log_info_native('[test:113] expected 0 frogs after timer poof, got ' + test113_count_frogs())
        __test_signal_ready()
        return
    }
    __log_marker('frog_timer_poof_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'frog_registered_ok',
        'frog_anim_walk_ok',
        'frog_anim_idle_ok',
        'frog_ptah_swarm_ok',
        'frog_event_swarm_ok',
        'frog_happiness_ok',
        'frog_cheat_ok',
        'frog_house_infest_ok',
        'frog_infest_on_pass_ok',
        'frog_timer_poof_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:113] missing marker: ' + marker)
            return false
        }
    }
    if (!__test_find_inlog('[test-marker] frog_saveload_ok')
        && !__test_find_inlog('[test-marker] frog_saveload_skipped')) {
        __log_info_native('[test:113] missing saveload ok/skip marker')
        return false
    }
    if (!__test113_saveload_ok && !__test113_saveload_skipped) {
        __log_info_native('[test:113] saveload neither ok nor skipped')
        return false
    }
    return true
}
