// Sun Temple place — 220 sandstone (staffed SY), reject without stock /
// second unfinished, consume place sandstone.

function run_test() {
    __log_info_native('[test:110] sun temple reject + place + sandstone consume')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_SUN_TEMPLE)) {
        __scenario_building_allow(BUILDING_SUN_TEMPLE, true)
    }
    if (!__scenario_building_allowed(BUILDING_STORAGE_YARD)) {
        __scenario_building_allow(BUILDING_STORAGE_YARD, true)
    }

    __test_set_treasury(500000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    var sandstone_need = 220

    var rejected = test_building_place(BUILDING_SUN_TEMPLE, cx - 5, cy - 5)
    if (!rejected) {
        rejected = test_building_place(BUILDING_SUN_TEMPLE, -1, -1)
    }
    if (rejected) {
        __log_info_native('[test:110] unexpected place without sandstone bid=' + rejected)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_reject_no_sandstone_ok')

    var sy = 0
    var yard_spots = [
        {x: 8, y: 8}, {x: 12, y: 12}, {x: 20, y: 8}, {x: 8, y: 40}
    ]
    for (var yi = 0; yi < yard_spots.length && !sy; yi++) {
        sy = test_staffed_yard_with_resource(RESOURCE_SANDSTONE, sandstone_need, yard_spots[yi].x, yard_spots[yi].y)
    }
    if (!sy) {
        sy = test_staffed_yard_with_resource(RESOURCE_SANDSTONE, sandstone_need, -1, -1)
    }
    if (!sy) {
        __log_info_native('[test:110] staffed yard setup failed')
        __test_signal_ready()
        return
    }

    var staffed_before = __test_yards_stored_staffed(RESOURCE_SANDSTONE)
    if (staffed_before < sandstone_need) {
        __log_info_native('[test:110] staffed sandstone ' + staffed_before + ' < need ' + sandstone_need)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_sandstone_ready:' + staffed_before)

    // 10×10 footprint — keep clear of the yard.
    var bid = test_building_place(BUILDING_SUN_TEMPLE, cx - 5, cy - 5)
    if (!bid) {
        bid = test_building_place(BUILDING_SUN_TEMPLE, 40, 40)
    }
    if (!bid) {
        bid = test_building_place(BUILDING_SUN_TEMPLE, -1, -1)
    }
    if (!bid) {
        __log_info_native('[test:110] failed to place BUILDING_SUN_TEMPLE')
        __test_signal_ready()
        return
    }

    __test_process_events()
    __test_pump_frames(2)

    var tile = __building_tile(bid)
    __log_marker('sun_temple_placed_ok:' + bid + ':' + tile.x + ',' + tile.y)

    var b = city.get_building(bid)
    if (!b || b.type != BUILDING_SUN_TEMPLE) {
        __log_info_native('[test:110] placed building type mismatch')
        __test_signal_ready()
        return
    }

    var staffed_after = __test_yards_stored_staffed(RESOURCE_SANDSTONE)
    if (staffed_after > staffed_before - sandstone_need) {
        __log_info_native('[test:110] sandstone not consumed: before=' + staffed_before + ' after=' + staffed_after)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_sandstone_consumed:' + staffed_after)

    // Top up yard so reject is from "one unfinished", not stock.
    test_staffed_yard_with_resource(RESOURCE_SANDSTONE, sandstone_need, 60, 60)
    var second = test_building_place(BUILDING_SUN_TEMPLE, 55, 55)
    if (!second) {
        second = test_building_place(BUILDING_SUN_TEMPLE, 70, 70)
    }
    if (second) {
        __log_info_native('[test:110] unexpected second unfinished sun temple bid=' + second)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_reject_second_ok')

    __log_info_native('[test:110] PASS')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'sun_temple_reject_no_sandstone_ok',
        'sun_temple_sandstone_ready',
        'sun_temple_placed_ok',
        'sun_temple_sandstone_consumed',
        'sun_temple_reject_second_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:110] missing marker ' + required[i])
            return false
        }
    }
    return true
}
