// FIGURE_FROG register + Ptah/EVENT/cheat plague + house infest + happiness + timer (CF1).
// Markers:
//   [test-marker] frog_registered_ok
//   [test-marker] frog_anim_walk_ok
//   [test-marker] frog_ptah_swarm_ok
//   [test-marker] frog_event_swarm_ok
//   [test-marker] frog_happiness_ok
//   [test-marker] frog_cheat_ok
//   [test-marker] frog_house_infest_ok
//   [test-marker] frog_timer_poof_ok

var GOD_PTAH = 2
var ACTION_FROG_ROAMING = 121

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
    // Evicted house must stay on house info window (not vacant-lot) while lockout runs.
    if (h2.is_vacant_lot) {
        __log_info_native('[test:113] infested empty house must not be is_vacant_lot')
        __test_signal_ready()
        return
    }
    // Lockout must also block add_population (in-flight immigrant/homeless path).
    __test_house_add_population(hid2, 3)
    h2 = city.get_house(hid2)
    if (h2.population != 0) {
        __log_info_native('[test:113] add_population must not refill infested house, pop=' + h2.population)
        __test_signal_ready()
        return
    }
    __log_marker('frog_house_infest_ok')

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
        'frog_ptah_swarm_ok',
        'frog_event_swarm_ok',
        'frog_happiness_ok',
        'frog_cheat_ok',
        'frog_house_infest_ok',
        'frog_timer_poof_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:113] missing marker: ' + marker)
            return false
        }
    }
    return true
}
