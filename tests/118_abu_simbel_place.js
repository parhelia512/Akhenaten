// Abu Simbel — reject clear land; place cliff bulk 9×21 + external 3×3 entrance; finish → rating.

function run_test() {
    __log_info_native('[test:118] abu simbel cliff place + finish rating')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_ABU_SIMBEL)) {
        __scenario_building_allow(BUILDING_ABU_SIMBEL, true)
    }

    __test_set_treasury(500000)

    var cx = 40
    var cy = 40
    var w = 9
    var h = 21
    // Entrance protrudes past far depth (outside bulk); rows y=h..h+2 stay clear.

    function paint_cliff_site(ox, oy) {
        for (var dy = 0; dy < h; dy++) {
            for (var dx = 0; dx < w; dx++) {
                terrain.add({ x: ox + dx, y: oy + dy }, TERRAIN_ELEVATION)
            }
        }
        // Entrance rows stay clear land (no elevation).
    }

    // Reject on clear land (no cliffs).
    var rejected = test_building_place(BUILDING_ABU_SIMBEL, cx, cy)
    if (rejected) {
        __log_info_native('[test:118] unexpected place without cliffs bid=' + rejected)
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_reject_no_cliff_ok')

    paint_cliff_site(cx, cy)
    __log_marker('abu_simbel_cliff_painted')

    var bid = test_building_place(BUILDING_ABU_SIMBEL, cx, cy)
    if (!bid) {
        __log_info_native('[test:118] failed to place BUILDING_ABU_SIMBEL on cliffs')
        __test_signal_ready()
        return
    }

    __test_process_events()
    __test_pump_frames(2)

    var b = city.get_building(bid)
    if (!b || b.type != BUILDING_ABU_SIMBEL) {
        __log_info_native('[test:118] placed building type mismatch')
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_placed_ok:' + bid)

    // Second (while unfinished) must be rejected — only one ever.
    paint_cliff_site(cx + 30, cy)
    var second = test_building_place(BUILDING_ABU_SIMBEL, cx + 30, cy)
    if (second) {
        __log_info_native('[test:118] unexpected second unfinished place bid=' + second)
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_reject_second_ok')

    // 8 art stages + terminal → phases().size()==9; set_phase(9) → FINISHED
    __test_monument_set_phase(bid, 9)
    __test_process_events()
    __test_pump_frames(2)

    var ph = __test_monument_phase(bid)
    // FINISHED is -1 stored as uint8 255
    if (ph != 255 && ph != -1) {
        __log_info_native('[test:118] not finished after set_phase(9) ph=' + ph)
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_finished_ok')

    city_update_monthly_monument_rating({})
    var rating_done = city.rating.monument | 0
    // weight 44 → trunc(2.25*44+4.5)=103 → clamp 100
    if (rating_done < 90) {
        __log_info_native('[test:118] finished rating want >=90 got ' + rating_done)
        __log_marker('abu_simbel_rating_fail:' + rating_done)
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_rating_ok:' + rating_done)

    // Finished still blocks another Abu Simbel on the map.
    paint_cliff_site(cx + 30, cy)
    var third = test_building_place(BUILDING_ABU_SIMBEL, cx + 30, cy)
    if (third) {
        __log_info_native('[test:118] unexpected place after finish bid=' + third)
        __test_signal_ready()
        return
    }
    __log_marker('abu_simbel_reject_after_finish_ok')

    __log_info_native('[test:118] PASS')
    __log_marker('abu_simbel_ok')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'abu_simbel_reject_no_cliff_ok',
        'abu_simbel_placed_ok',
        'abu_simbel_reject_second_ok',
        'abu_simbel_finished_ok',
        'abu_simbel_rating_ok',
        'abu_simbel_reject_after_finish_ok',
        'abu_simbel_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:118] missing marker ' + required[i])
            return false
        }
    }
    return true
}
