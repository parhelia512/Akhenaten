// Caesareum — place 15×15 clear land; reject second unfinished; finish → rating.

function run_test() {
    __log_info_native('[test:119] caesareum place + finish rating')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_CAESAREUM)) {
        __scenario_building_allow(BUILDING_CAESAREUM, true)
    }

    __test_set_treasury(500000)

    var cx = 40
    var cy = 40

    var bid = test_building_place(BUILDING_CAESAREUM, cx, cy)
    if (!bid) {
        bid = test_building_place(BUILDING_CAESAREUM, 30, 30)
    }
    if (!bid) {
        __log_info_native('[test:119] failed to place BUILDING_CAESAREUM')
        __test_signal_ready()
        return
    }

    __test_process_events()
    __test_pump_frames(2)

    var b = city.get_building(bid)
    if (!b || b.type != BUILDING_CAESAREUM) {
        __log_info_native('[test:119] placed building type mismatch')
        __test_signal_ready()
        return
    }
    __log_marker('caesareum_placed_ok:' + bid)

    // Second unfinished must be rejected.
    var second = test_building_place(BUILDING_CAESAREUM, cx + 20, cy)
    if (second) {
        __log_info_native('[test:119] unexpected second unfinished place bid=' + second)
        __test_signal_ready()
        return
    }
    __log_marker('caesareum_reject_second_ok')

    // Engine: phases 0–5 + sentinel 6 → size 7; set_phase(7) → FINISHED
    __test_monument_set_phase(bid, 7)
    __test_process_events()
    __test_pump_frames(2)

    var ph = __test_monument_phase(bid)
    if (ph != 255 && ph != -1) {
        __log_info_native('[test:119] not finished after set_phase(7) ph=' + ph)
        __test_signal_ready()
        return
    }
    __log_marker('caesareum_finished_ok')

    city_update_monthly_monument_rating({})
    var rating_done = city.rating.monument | 0
    // weight 8 → trunc(2.25*8+4.5)=22; require > 0 and sensible floor
    if (rating_done < 15) {
        __log_info_native('[test:119] finished rating want >=15 got ' + rating_done)
        __log_marker('caesareum_rating_fail:' + rating_done)
        __test_signal_ready()
        return
    }
    __log_marker('caesareum_rating_ok:' + rating_done)

    __log_info_native('[test:119] PASS')
    __log_marker('caesareum_ok')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'caesareum_placed_ok',
        'caesareum_reject_second_ok',
        'caesareum_finished_ok',
        'caesareum_rating_ok',
        'caesareum_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:119] missing marker ' + required[i])
            return false
        }
    }
    return true
}
