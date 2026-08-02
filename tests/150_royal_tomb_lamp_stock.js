// C10 RT1: Small royal tomb lamp stock — phase-0 delivery, then stock ≤700 + consume.

function run_test() {
    __log_info_native('[test:150] royal tomb lamp stock pipeline')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_SMALL_ROYAL_TOMB)) {
        __scenario_building_allow(BUILDING_SMALL_ROYAL_TOMB, true)
    }
    __test_set_treasury(500000)

    var cx = 40
    var cy = 40
    var w = 11
    var h = 20
    for (var dy = 0; dy < h; dy++) {
        for (var dx = 0; dx < w; dx++) {
            terrain.add({ x: cx + dx, y: cy + dy }, TERRAIN_ELEVATION)
        }
    }

    var bid = test_building_place(BUILDING_SMALL_ROYAL_TOMB, cx, cy)
    if (!bid) {
        __log_info_native('[test:150] place failed')
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_placed_ok:' + bid)

    // Phase 0: no stock room until carving begins.
    if (__test_royal_tomb_lamp_stock_room(bid) != 0) {
        __log_info_native('[test:150] phase0 stock_room want 0 got ' + __test_royal_tomb_lamp_stock_room(bid))
        __test_signal_ready()
        return
    }

    // Deliver phase lamps (400) + leftover into stock once phase need is full.
    if (!__test_monument_add_resource(bid, RESOURCE_LAMPS, 400)) {
        __log_info_native('[test:150] phase lamp deliver failed')
        __test_signal_ready()
        return
    }
    if (__test_monument_resource_pct(bid, RESOURCE_LAMPS) < 100) {
        __log_info_native('[test:150] phase lamp pct want 100 got ' + __test_monument_resource_pct(bid, RESOURCE_LAMPS))
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_phase0_full')

    // Advance to phase 1 → stock top-up allowed (room=700).
    __test_monument_set_phase(bid, 1)
    var room = __test_royal_tomb_lamp_stock_room(bid)
    if (room != 700) {
        __log_info_native('[test:150] phase1 stock_room want 700 got ' + room)
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_stock_room_ok:' + room)

    if (!__test_monument_add_resource(bid, RESOURCE_LAMPS, 250)) {
        __log_info_native('[test:150] stock deliver failed')
        __test_signal_ready()
        return
    }
    var stock = __test_royal_tomb_lamp_stock(bid)
    if (stock != 250) {
        __log_info_native('[test:150] stock want 250 got ' + stock)
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_stock_ok:' + stock)

    // Cap at 700.
    __test_monument_add_resource(bid, RESOURCE_LAMPS, 1000)
    stock = __test_royal_tomb_lamp_stock(bid)
    if (stock != 700) {
        __log_info_native('[test:150] stock cap want 700 got ' + stock)
        __test_signal_ready()
        return
    }
    if (__test_royal_tomb_lamp_stock_room(bid) != 0) {
        __log_info_native('[test:150] full stock_room want 0')
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_stock_cap_ok')

    // Without workers, update_day must not burn lamps / advance.
    __test_building_update_day(bid)
    if (__test_royal_tomb_lamp_stock(bid) != 700) {
        __log_info_native('[test:150] no-worker burn unexpected stock=' + __test_royal_tomb_lamp_stock(bid))
        __test_signal_ready()
        return
    }
    __log_marker('rt_lamp_no_worker_ok')
    __log_marker('rt_lamp_stock_pipeline_ok')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'rt_lamp_placed_ok',
        'rt_lamp_phase0_full',
        'rt_lamp_stock_room_ok',
        'rt_lamp_stock_ok',
        'rt_lamp_stock_cap_ok',
        'rt_lamp_no_worker_ok',
        'rt_lamp_stock_pipeline_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:150] missing marker ' + required[i])
            return false
        }
    }
    return true
}
