// C10 RT3: Large royal tomb — cliff bulk 17x33 + 1x1 entrance; finish → rating weight 13.

function run_test() {
    __log_info_native('[test:145] large royal tomb cliff place + finish rating')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_LARGE_ROYAL_TOMB)) {
        __scenario_building_allow(BUILDING_LARGE_ROYAL_TOMB, true)
    }

    __test_set_treasury(500000)

    var cx = 40
    var cy = 40
    var w = 17
    var h = 33

    function paint_cliff_site(ox, oy) {
        for (var dy = 0; dy < h; dy++) {
            for (var dx = 0; dx < w; dx++) {
                terrain.add({ x: ox + dx, y: oy + dy }, TERRAIN_ELEVATION)
            }
        }
    }

    var rejected = test_building_place(BUILDING_LARGE_ROYAL_TOMB, cx, cy)
    if (rejected) {
        __log_info_native('[test:145] unexpected place without cliffs bid=' + rejected)
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_reject_no_cliff_ok')

    paint_cliff_site(cx, cy)
    __log_marker('large_royal_tomb_cliff_painted')

    var bid = test_building_place(BUILDING_LARGE_ROYAL_TOMB, cx, cy)
    if (!bid) {
        __log_info_native('[test:145] failed to place BUILDING_LARGE_ROYAL_TOMB on cliffs')
        __test_signal_ready()
        return
    }

    __test_process_events()
    __test_pump_frames(2)

    var b = city.get_building(bid)
    if (!b || b.type != BUILDING_LARGE_ROYAL_TOMB) {
        __log_info_native('[test:145] placed building type mismatch')
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_placed_ok:' + bid)

    paint_cliff_site(cx + 40, cy)
    var second = test_building_place(BUILDING_LARGE_ROYAL_TOMB, cx + 40, cy)
    if (second) {
        __log_info_native('[test:145] unexpected second unfinished place bid=' + second)
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_reject_second_ok')

    var mon = city.get_monument(bid)
    if (!mon) {
        __log_info_native('[test:145] city.get_monument failed')
        __test_signal_ready()
        return
    }
    var total = mon.phases_total()
    if (total < 2) {
        __log_info_native('[test:145] phases_total=' + total + ' want >=2')
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_schedule_ok:' + total)

    __test_monument_set_phase(bid, total)
    __test_process_events()
    __test_pump_frames(2)

    var ph = __test_monument_phase(bid)
    if (ph != 255 && ph != -1) {
        __log_info_native('[test:145] not finished after set_phase(' + total + ') ph=' + ph)
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_finished_ok')

    city_update_monthly_monument_rating({})
    var rating_done = city.rating.monument | 0
    // weight 13 → trunc(2.25*13+4.5)=33
    if (rating_done < 33) {
        __log_info_native('[test:145] finished rating want >=33 got ' + rating_done)
        __log_marker('large_royal_tomb_rating_fail:' + rating_done)
        __test_signal_ready()
        return
    }
    __log_marker('large_royal_tomb_rating_ok:' + rating_done)
    __log_marker('large_royal_tomb_ok')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'large_royal_tomb_reject_no_cliff_ok',
        'large_royal_tomb_placed_ok',
        'large_royal_tomb_reject_second_ok',
        'large_royal_tomb_schedule_ok',
        'large_royal_tomb_finished_ok',
        'large_royal_tomb_rating_ok',
        'large_royal_tomb_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:145] missing marker ' + required[i])
            return false
        }
    }
    return true
}
