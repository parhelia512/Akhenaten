// Low Bridge: synthetic 3-wide channel → place via map_bridge_add; floodplain reject;
// span longer than max_length reject; citizen road routing on bridge tiles.
// Markers: test_bridge_placed / test_bridge_floodplain_reject / test_bridge_max_reject / test_bridge_citizen_road

var __test46_ok = false

function test46_place_bridge(start) {
    if (!__map_bridge_calculate_valid(start, false)) {
        return 0
    }
    return __map_bridge_add(start, false)
}

function run_test() {
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)
    __scenario_building_allow(BUILDING_LOW_BRIDGE, true)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0

    // --- happy path: length 4 channel ---
    var start = test_prepare_bridge_channel(cx, cy, 3, 4)
    var length = test46_place_bridge(start)
    if (length < 2) {
        __log_info_native('[test:46] place failed length=' + length
            + ' valid=' + __map_bridge_calculate_valid(start, false)
            + ' calc_len=' + __map_bridge_calculate_length(start, false))
        __test_signal_ready()
        return
    }

    if (!__map_is_bridge(start)) {
        __log_info_native('[test:46] start tile not bridge after place')
        __test_signal_ready()
        return
    }

    if (__map_bridge_part_at(start) < 1 || __map_bridge_part_at(start) > 6) {
        __log_info_native('[test:46] bad bridge_part=' + __map_bridge_part_at(start))
        __test_signal_ready()
        return
    }

    if (__map_bridge_type_at(start) !== 0) {
        __log_info_native('[test:46] expected bridge_type 0 got ' + __map_bridge_type_at(start))
        __test_signal_ready()
        return
    }

    if (!terrain.is(start, TERRAIN_ROAD) || !terrain.is(start, TERRAIN_WATER)) {
        __log_info_native('[test:46] missing WATER|ROAD on bridge tile')
        __test_signal_ready()
        return
    }

    if (!__map_routing_citizen_is_road(start)) {
        __log_info_native('[test:46] citizen routing not ROAD on bridge')
        __test_signal_ready()
        return
    }

    __log_marker('test_bridge_placed:len=' + length + ':' + start.x + ',' + start.y)

    // --- floodplain reject ---
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)
    start = test_prepare_bridge_channel(cx, cy + 8, 3, 4)
    for (var dy = 0; dy < 4; dy++) {
        for (var dx = -1; dx <= 1; dx++) {
            terrain.add({ x: cx + dx, y: cy + 8 + dy }, TERRAIN_FLOODPLAIN)
        }
    }
    if (__map_bridge_calculate_valid(start, false) || test46_place_bridge(start) > 0) {
        __log_info_native('[test:46] floodplain span should be rejected')
        __test_signal_ready()
        return
    }
    __log_marker('test_bridge_floodplain_reject')

    // --- max_length reject (10 water rows > config max 8) ---
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)
    start = test_prepare_bridge_channel(cx, cy, 3, 10)
    var calc_len = __map_bridge_calculate_length(start, false)
    var valid = __map_bridge_calculate_valid(start, false)
    if (valid || test46_place_bridge(start) > 0) {
        __log_info_native('[test:46] overlong span should be rejected calc_len='
            + calc_len + ' valid=' + valid)
        __test_signal_ready()
        return
    }
    __log_marker('test_bridge_max_reject:calc_len=' + calc_len)

    __log_marker('test_bridge_citizen_road')
    __test46_ok = true
    __test_signal_ready()
}

function check_valid() {
    if (!__test46_ok) {
        return false
    }
    var required = [
        '[test-marker] test_bridge_placed:',
        '[test-marker] test_bridge_floodplain_reject',
        '[test-marker] test_bridge_max_reject:',
        '[test-marker] test_bridge_citizen_road'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog(required[i])) {
            __log_info_native('[test:46] missing ' + required[i])
            return false
        }
    }
    return true
}
