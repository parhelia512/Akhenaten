// FIGURE_MUMMY register + spawn_wave → 1× msg 496 + soldier can target + kill.
// Markers:
//   [test-marker] mummy_registered_ok
//   [test-marker] mummy_spawn_wave_msg_ok
//   [test-marker] mummy_soldier_target_ok
//   [test-marker] mummy_soldier_kill_ok
//   [test-marker] mummy_city_cap_ok
//   [test-marker] mummy_event_type_ok

var MESSAGE_MUMMY_ATTACKS = 496
var MUMMY_CITY_CAP = 4

function count_messages_with_id(mm_id) {
    var n = 0
    var total = __city_message_count()
    for (var i = 0; i < total; i++) {
        if (__city_message_mm_text_id(i) == mm_id) {
            n++
        }
    }
    return n
}

function kill_all_mummies() {
    for (var i = 1; i < 2000; i++) {
        if (__figure_is_valid(i) && __figure_get_type(i) == FIGURE_MUMMY) {
            __test_figure_kill(i)
        }
    }
}

function run_test() {
    __log_info_native('[test:85] mummy attack spawn_wave + soldier kill')
    test_ensure_city_session('data/default.map')

    var probe = test_figure_create(FIGURE_MUMMY, -1, -1)
    if (!probe || !__figure_is_valid(probe) || __figure_get_type(probe) != FIGURE_MUMMY) {
        __log_info_native('[test:85] FIGURE_MUMMY not registered')
        __test_signal_ready()
        return
    }
    if (!__test_figure_is_enemy(probe)) {
        __log_info_native('[test:85] mummy should be is_enemy for soldier targeting')
        __test_signal_ready()
        return
    }
    __test_figure_kill(probe)
    __log_marker('mummy_registered_ok')

    kill_all_mummies()
    var before_msg = count_messages_with_id(MESSAGE_MUMMY_ATTACKS)

    var fid = __test_mummy_spawn_wave(1)
    __test_process_events()
    if (!fid || !__figure_is_valid(fid) || __figure_get_type(fid) != FIGURE_MUMMY) {
        __log_info_native('[test:85] spawn_wave failed, fid=' + fid)
        __test_signal_ready()
        return
    }
    if (__test_count_figures(FIGURE_MUMMY) < 1) {
        __log_info_native('[test:85] no live mummy after spawn_wave')
        __test_signal_ready()
        return
    }

    var after_msg = count_messages_with_id(MESSAGE_MUMMY_ATTACKS)
    if (after_msg != before_msg + 1) {
        __log_info_native('[test:85] want +1 message 496, before=' + before_msg + ' after=' + after_msg)
        __test_signal_ready()
        return
    }

    // Wave of 2 still adds exactly one popup.
    var fid2 = __test_mummy_spawn_wave(2)
    __test_process_events()
    if (!fid2) {
        __log_info_native('[test:85] spawn_wave(2) failed')
        __test_signal_ready()
        return
    }
    var msg2 = count_messages_with_id(MESSAGE_MUMMY_ATTACKS)
    if (msg2 != after_msg + 1) {
        __log_info_native('[test:85] wave(2) should add exactly 1 msg, got delta ' + (msg2 - after_msg))
        __test_signal_ready()
        return
    }
    __log_marker('mummy_spawn_wave_msg_ok')

    var tile = __figure_get_tile(fid)
    var target = __test_soldier_combat_target(tile.x, tile.y, 12)
    if (!target || __figure_get_type(target) != FIGURE_MUMMY) {
        __log_info_native('[test:85] soldier combat target want mummy, got ' + target)
        __test_signal_ready()
        return
    }
    __log_marker('mummy_soldier_target_ok')

    kill_all_mummies()
    if (__test_count_figures(FIGURE_MUMMY) != 0) {
        __log_info_native('[test:85] after kill, want 0 live mummies, got ' + __test_count_figures(FIGURE_MUMMY))
        __test_signal_ready()
        return
    }
    __log_marker('mummy_soldier_kill_ok')

    // City live cap: spawn_wave(10) must clamp to 4.
    kill_all_mummies()
    var over = __test_mummy_spawn_wave(10)
    __test_process_events()
    var live_cap = __test_count_figures(FIGURE_MUMMY)
    if (!over || live_cap != MUMMY_CITY_CAP) {
        __log_info_native('[test:85] city cap want ' + MUMMY_CITY_CAP + ' live, got ' + live_cap)
        __test_signal_ready()
        return
    }
    // Second wave while at cap → no new spawn.
    var blocked = __test_mummy_spawn_wave(2)
    if (blocked) {
        __log_info_native('[test:85] spawn at cap should return 0, got ' + blocked)
        __test_signal_ready()
        return
    }
    if (__test_count_figures(FIGURE_MUMMY) != MUMMY_CITY_CAP) {
        __log_info_native('[test:85] live count changed while at cap')
        __test_signal_ready()
        return
    }
    __log_marker('mummy_city_cap_ok')

    // EVENT_TYPE_MUMMY path → spawn + 1× msg 496.
    kill_all_mummies()
    var before_ev = count_messages_with_id(MESSAGE_MUMMY_ATTACKS)
    city.create_chain_event({
        tag_id: 8501,
        type: EVENT_TYPE_MUMMY,
        amount: 2,
        trigger: EVENT_TRIGGER_ONCE
    }).execute()
    __test_process_events()
    var after_ev = count_messages_with_id(MESSAGE_MUMMY_ATTACKS)
    var live_ev = __test_count_figures(FIGURE_MUMMY)
    if (live_ev < 1) {
        __log_info_native('[test:85] EVENT_TYPE_MUMMY spawned 0 mummies')
        __test_signal_ready()
        return
    }
    if (after_ev != before_ev + 1) {
        __log_info_native('[test:85] EVENT_TYPE_MUMMY want +1 msg 496, before='
            + before_ev + ' after=' + after_ev)
        __test_signal_ready()
        return
    }
    __log_marker('mummy_event_type_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'mummy_registered_ok',
        'mummy_spawn_wave_msg_ok',
        'mummy_soldier_target_ok',
        'mummy_soldier_kill_ok',
        'mummy_city_cap_ok',
        'mummy_event_type_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + markers[i])) {
            return false
        }
    }
    return true
}
