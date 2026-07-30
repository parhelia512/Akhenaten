// Egyptian war elephant — register + spawn/action smoke + soldier can target.
// FIGURE_ENEMY_EGYPTIAN_ELEPHANT (47) → figure_egyptian_elephant (figure_enemy_elephant).
// Not in enemy_egyptian.figure_types[]. AoE trampling is a follow-up.
// Markers:
//   [test-marker] egyptian_elephant_control_ok
//   [test-marker] egyptian_elephant_registered_ok
//   [test-marker] egyptian_elephant_spawn_ok
//   [test-marker] egyptian_elephant_soldier_target_ok
//   [test-marker] egyptian_elephant_all_ok

function run_test() {
    __log_info_native('[test:102] egyptian elephant (ES4)')
    test_ensure_city_session('data/default.map')

    if (!__test_enemy_figure_registered(FIGURE_ENEMY_EGYPTIAN_CAMEL)) {
        __log_info_native('[test:102] control failed: camel not registered')
        __test_signal_ready()
        return
    }
    __log_marker('egyptian_elephant_control_ok')

    if (!__test_enemy_figure_registered(FIGURE_ENEMY_EGYPTIAN_ELEPHANT)) {
        __log_info_native('[test:102] FIGURE_ENEMY_EGYPTIAN_ELEPHANT not registered')
        __test_signal_ready()
        return
    }
    __log_marker('egyptian_elephant_registered_ok')

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0

    var eid = test_figure_create(FIGURE_ENEMY_EGYPTIAN_ELEPHANT, cx, cy)
    if (!eid || !__figure_is_valid(eid) || !__test_figure_is_enemy(eid)) {
        __log_info_native('[test:102] elephant spawn/dcast_enemy failed')
        __test_signal_ready()
        return
    }
    if (__figure_get_type(eid) != FIGURE_ENEMY_EGYPTIAN_ELEPHANT) {
        __log_info_native('[test:102] wrong type after create')
        __test_signal_ready()
        return
    }
    __test_figure_action_perform(eid)
    if (!__figure_is_valid(eid)) {
        __log_info_native('[test:102] elephant poofed on first action')
        __test_signal_ready()
        return
    }
    __log_marker('egyptian_elephant_spawn_ok')

    var tile = __figure_get_tile(eid)
    var target = __test_soldier_combat_target(tile.x, tile.y, 12)
    if (!target || __figure_get_type(target) != FIGURE_ENEMY_EGYPTIAN_ELEPHANT) {
        __log_info_native('[test:102] soldier combat target want elephant, got ' + target)
        __test_signal_ready()
        return
    }
    __log_marker('egyptian_elephant_soldier_target_ok')

    __test_figure_kill(eid)
    __log_marker('egyptian_elephant_all_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'egyptian_elephant_control_ok',
        'egyptian_elephant_registered_ok',
        'egyptian_elephant_spawn_ok',
        'egyptian_elephant_soldier_target_ok',
        'egyptian_elephant_all_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:102] missing marker: ' + marker)
            return false
        }
    }
    return true
}
