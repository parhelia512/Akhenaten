// TE1d / E2 regress: player transport embark → sail → auto-disembark via JS API.
// Markers:
//   [test-marker] player_transport_embark_ok
//   [test-marker] player_transport_sail_ok
//   [test-marker] player_transport_disembark_ok

function run_test() {
    __log_info_native('[test:103] player transport embark / sail / disembark')
    test_reload_city_session('data/default.map')

    var ship = __test_player_transport_spawn_for_embark(4)
    if (!ship) {
        __log_info_native('[test:103] failed to spawn moored transport + company')
        __test_signal_ready()
        return
    }

    var company = __test_player_transport_company_id()
    if (!company) {
        __log_info_native('[test:103] no company id after spawn')
        __test_signal_ready()
        return
    }

    if (__transport_ship_has_troops(ship)) {
        __log_info_native('[test:103] ship already has troops before embark')
        __test_signal_ready()
        return
    }

    __transport_ship_embark(ship, company)
    if (!__transport_ship_has_troops(ship)) {
        __log_info_native('[test:103] embark failed')
        __test_signal_ready()
        return
    }
    __log_marker('player_transport_embark_ok')

    // Finish boarding timer at moored (phase 1 → 2).
    for (var i = 0; i < 60; i++) {
        __test_figure_action_perform(ship)
    }

    var wx = __test_player_transport_water_x()
    var wy = __test_player_transport_water_y()

    // Sail two tiles along the water strip (still has shore access).
    __transport_ship_sail_to(ship, wx + 2, wy)
    __log_marker('player_transport_sail_ok')

    // Avoid long water-path waits in the hermetic map: snap to destination then
    // let the ANCHORED disembark timer complete.
    __test_transport_ship_snap_to_destination(ship)

    var disembarked = false
    for (var j = 0; j < 80; j++) {
        __test_figure_action_perform(ship)
        if (!__transport_ship_has_troops(ship)) {
            disembarked = true
            break
        }
    }

    if (!disembarked) {
        __log_info_native('[test:103] disembark failed; has_troops=' + __transport_ship_has_troops(ship))
        __test_signal_ready()
        return
    }
    __log_marker('player_transport_disembark_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'player_transport_embark_ok',
        'player_transport_sail_ok',
        'player_transport_disembark_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:103] missing marker: ' + marker)
            return false
        }
    }
    return true
}
