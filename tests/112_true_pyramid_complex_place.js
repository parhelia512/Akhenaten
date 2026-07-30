// True Pyramid Complex (20×20 on-land) — place, parts, height→polish schedule.
// Causeway/temples not implemented; same cadence as large true pyramid.

function run_test() {
    __log_info_native('[test:112] true pyramid complex place + phase walk')
    test_ensure_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_PYRAMID_COMPLEX)) {
        __scenario_building_allow(BUILDING_PYRAMID_COMPLEX, true)
    }

    __test_set_treasury(500000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    var bid = 0
    var candidates = [
        {x: cx - 10, y: cy - 10}, {x: cx, y: cy}, {x: 30, y: 30}, {x: 24, y: 24}
    ]
    for (var i = 0; i < candidates.length && !bid; i++) {
        bid = test_building_place(BUILDING_PYRAMID_COMPLEX, candidates[i].x, candidates[i].y)
    }
    if (!bid) {
        bid = test_building_place(BUILDING_PYRAMID_COMPLEX, -1, -1)
    }
    if (!bid) {
        __log_info_native('[test:112] failed to place BUILDING_PYRAMID_COMPLEX')
        __test_signal_ready()
        return
    }

    var tile = __building_tile(bid)
    __log_marker('true_pyramid_complex_placed_ok:' + bid + ':' + tile.x + ',' + tile.y)

    if (__building_type(bid) != BUILDING_PYRAMID_COMPLEX) {
        __log_info_native('[test:112] unexpected type ' + __building_type(bid))
        __test_signal_ready()
        return
    }

    var parts = [bid]
    var cur = city.get_building(bid)
    while (cur && cur.next_part_building_id > 0 && parts.length < 256) {
        parts.push(cur.next_part_building_id)
        cur = city.get_building(cur.next_part_building_id)
    }
    if (parts.length >= 80) {
        __log_marker('true_pyramid_complex_parts_ok:' + parts.length)
    } else {
        __log_marker('true_pyramid_complex_parts_fail:' + parts.length)
    }

    var mon0 = city.get_monument(bid)
    if (!mon0) {
        __log_info_native('[test:112] city.get_monument failed')
        __test_signal_ready()
        return
    }
    var total = mon0.phases_total()
    if (total < 41) {
        __log_info_native('[test:112] phases_total=' + total + ' want >=41')
        __log_marker('true_pyramid_complex_schedule_fail:' + total)
        __test_signal_ready()
        return
    }
    __log_marker('true_pyramid_complex_schedule_ok:' + total)

    var all_ok = true
    var check_phases = [0, 2, 24, 35, 36, 40]
    for (var ci = 0; ci < check_phases.length; ci++) {
        var want = check_phases[ci]
        __test_monument_set_phase(bid, want)
        __test_pump_frames(2)
        var got = __test_monument_phase(bid)
        if (got != want) {
            __log_info_native('[test:112] phase want ' + want + ' got ' + got)
            all_ok = false
        }
    }

    __test_monument_set_phase(bid, total)
    __test_pump_frames(2)
    var fin = __test_monument_phase(bid)
    if (fin != -1 && fin != 255) {
        __log_info_native('[test:112] finish want -1 after set_phase(total), got ' + fin)
        all_ok = false
    }

    __test_monument_set_phase(bid, 36)
    __test_pump_frames(2)
    var mon36 = city.get_monument(bid)
    if (mon36 && mon36.phase() == 36 && mon36.material_pct_min() == 100) {
        __log_marker('true_pyramid_complex_polish_no_lime_ok')
    } else {
        __log_marker('true_pyramid_complex_polish_fail')
        all_ok = false
    }

    if (all_ok) {
        __log_marker('true_pyramid_complex_phases_ok')
    } else {
        __log_marker('true_pyramid_complex_phases_fail')
    }

    __log_info_native('[test:112] done')
    __test_signal_ready()
}
