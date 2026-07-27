// Bast lion raid (CF5 TEMP Enhanced): game_feature ON, no houses → spawn lions at TEMPLE_BAST/ZOO.
// Markers:
//   [test-marker] bast_lion_feature_on_spawn_ok
//   [test-marker] bast_lion_feature_off_no_spawn_ok
//   [test-marker] bast_lion_houses_smash_no_lions_ok

var GOD_BAST = 4
var __test75_feat_prev = false
var __test75_on_ok = false
var __test75_off_ok = false
var __test75_smash_ok = false

function test75_remove_lions() {
    city.figures.remove_figures(FIGURE_LION)
}

function test75_count_lions() {
    return __test_count_figures(FIGURE_LION)
}

function test75_place_temple() {
    if (!__scenario_building_allowed(BUILDING_TEMPLE_BAST)) {
        __scenario_building_allow(BUILDING_TEMPLE_BAST, true)
    }
    var bid = test_building_place(BUILDING_TEMPLE_BAST, -1, -1)
    if (!bid) {
        bid = __test_building_create(BUILDING_TEMPLE_BAST, -1, -1)
    }
    return bid
}

function run_test() {
    __log_info_native('[test:75] bast lion curse (CF5)')
    test_reload_city_session('data/default.map')
    __test_set_treasury(50000)
    game.paused = false

    __test75_feat_prev = game_features.get('gameplay_bast_lion_raid') === true

    // Clear any pre-existing houses via OFF smash (or wrath_2 if none).
    game_features.set('gameplay_bast_lion_raid', false)
    __test_run_console_command('god_major_curse ' + GOD_BAST)
    test75_remove_lions()

    // --- Feature ON, no houses: temple spawn ---
    game_features.set('gameplay_bast_lion_raid', true)
    var temple = test75_place_temple()
    if (!temple) {
        __log_info_native('[test:75] failed to place BUILDING_TEMPLE_BAST')
        __test_signal_ready()
        return
    }

    __test_run_console_command('god_major_curse ' + GOD_BAST)
    var lions = test75_count_lions()
    if (lions < 1) {
        __log_info_native('[test:75] feature ON expected lions, got ' + lions)
        __test_signal_ready()
        return
    }
    __log_marker('bast_lion_feature_on_spawn_ok')
    __test75_on_ok = true

    // --- Feature OFF: no lions when no houses ---
    test75_remove_lions()
    game_features.set('gameplay_bast_lion_raid', false)
    __test_run_console_command('god_major_curse ' + GOD_BAST)
    lions = test75_count_lions()
    if (lions != 0) {
        __log_info_native('[test:75] feature OFF expected 0 lions, got ' + lions)
        __test_signal_ready()
        return
    }
    __log_marker('bast_lion_feature_off_no_spawn_ok')
    __test75_off_ok = true

    // --- Feature ON + house present → smash, not lions ---
    game_features.set('gameplay_bast_lion_raid', true)
    test75_remove_lions()
    var house = __test_building_create(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    if (!house) {
        house = test_building_place(BUILDING_HOUSE_CRUDE_HUT, -1, -1)
    }
    if (!house) {
        __log_info_native('[test:75] failed to create house for smash path')
        __test_signal_ready()
        return
    }

    __test_run_console_command('god_major_curse ' + GOD_BAST)
    lions = test75_count_lions()
    if (lions != 0) {
        __log_info_native('[test:75] smash path expected 0 lions, got ' + lions)
        __test_signal_ready()
        return
    }
    __log_marker('bast_lion_houses_smash_no_lions_ok')
    __test75_smash_ok = true

    __test_signal_ready()
}

function check_valid() {
    if (!__test75_on_ok || !__test75_off_ok || !__test75_smash_ok) {
        __log_info_native('[test:75] one or more phases failed')
        return false
    }

    var markers = [
        'bast_lion_feature_on_spawn_ok',
        'bast_lion_feature_off_no_spawn_ok',
        'bast_lion_houses_smash_no_lions_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:75] missing marker: ' + marker)
            return false
        }
    }
    return true
}

function done() {
    game_features.set('gameplay_bast_lion_raid', __test75_feat_prev)
    test75_remove_lions()
}
