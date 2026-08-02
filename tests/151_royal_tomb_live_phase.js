// C10 RT1: Small royal tomb live phase walk —
// lamps → phase1 (mason+artisan+clay/paint+stock) → phase2.

function run_test() {
    __log_info_native('[test:151] royal tomb live phase walk')
    test_reload_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_SMALL_ROYAL_TOMB)) {
        __scenario_building_allow(BUILDING_SMALL_ROYAL_TOMB, true)
    }
    __test_set_treasury(500000)

    var cx = 40
    var cy = 40
    for (var dy = 0; dy < 20; dy++) {
        for (var dx = 0; dx < 11; dx++) {
            terrain.add({ x: cx + dx, y: cy + dy }, TERRAIN_ELEVATION)
        }
    }

    var bid = test_building_place(BUILDING_SMALL_ROYAL_TOMB, cx, cy)
    if (!bid) {
        __log_info_native('[test:151] place failed')
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_placed_ok:' + bid)

    // --- phase 0 → 1: deliver 400 lamps, no guilds required ---
    if (!__test_monument_add_resource(bid, RESOURCE_LAMPS, 400)) {
        __log_info_native('[test:151] phase0 lamp deliver failed')
        __test_signal_ready()
        return
    }
    __test_building_update_day(bid)
    if (__test_monument_phase(bid) != 1) {
        __log_info_native('[test:151] after lamps want phase 1 got ' + __test_monument_phase(bid))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_phase1_ok')

    // Without mason / artisan / stock, phase must not advance.
    __test_building_update_day(bid)
    if (__test_monument_phase(bid) != 1) {
        __log_info_native('[test:151] phase advanced without mason/artisan/stock')
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_gate_no_mason_ok')

    // Phase 1 needs clay+paint (My Palace Stairway) — mason alone is not enough.
    __test_monument_add_resource(bid, RESOURCE_LAMPS, 50)
    if (__test_royal_tomb_lamp_stock(bid) < 1) {
        __log_info_native('[test:151] stock empty after top-up')
        __test_signal_ready()
        return
    }
    var mason = __test_royal_tomb_attach_worker(bid, FIGURE_STONEMASON)
    if (!mason) {
        __log_info_native('[test:151] attach mason failed')
        __test_signal_ready()
        return
    }
    __test_building_update_day(bid)
    if (__test_monument_phase(bid) != 1) {
        __log_info_native('[test:151] phase advanced without artisan/clay got '
            + __test_monument_phase(bid))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_gate_no_artisan_ok')

    // --- phase 1 → 2: artisan delivers clay+paint; mason + stock required ---
    if (__test_monument_resource_pct(bid, RESOURCE_CLAY) > 0
            || __test_monument_resource_pct(bid, RESOURCE_PAINT) > 0) {
        __log_info_native('[test:151] phase1 clay/paint should start empty')
        __test_signal_ready()
        return
    }

    var artisan = __test_royal_tomb_attach_worker(bid, FIGURE_TOMB_ARTISAN)
    if (!artisan) {
        __log_info_native('[test:151] attach artisan failed')
        __test_signal_ready()
        return
    }
    __test_figure_action_perform(artisan)
    if (__test_monument_resource_pct(bid, RESOURCE_CLAY) < 100
            || __test_monument_resource_pct(bid, RESOURCE_PAINT) < 100) {
        __log_info_native('[test:151] artisan deliver clay='
            + __test_monument_resource_pct(bid, RESOURCE_CLAY)
            + ' paint=' + __test_monument_resource_pct(bid, RESOURCE_PAINT))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_clay_paint_ok')

    // max_artisans=2 — second slot still open.
    var artisan2 = __test_royal_tomb_attach_worker(bid, FIGURE_TOMB_ARTISAN)
    if (!artisan2) {
        __log_info_native('[test:151] attach second artisan failed (max_artisans)')
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_two_artisans_ok')

    if (!__test_figure_is_alive(mason)) {
        mason = __test_royal_tomb_attach_worker(bid, FIGURE_STONEMASON)
        if (!mason) {
            __log_info_native('[test:151] reattach mason failed')
            __test_signal_ready()
            return
        }
    }

    var stock_before = __test_royal_tomb_lamp_stock(bid)
    __test_building_update_day(bid)
    if (__test_monument_phase(bid) != 2) {
        __log_info_native('[test:151] after artisan want phase 2 got ' + __test_monument_phase(bid)
            + ' clay=' + __test_monument_resource_pct(bid, RESOURCE_CLAY)
            + ' paint=' + __test_monument_resource_pct(bid, RESOURCE_PAINT)
            + ' stock=' + __test_royal_tomb_lamp_stock(bid))
        __test_signal_ready()
        return
    }
    if (__test_royal_tomb_lamp_stock(bid) != stock_before - 1) {
        __log_info_native('[test:151] lamp not consumed stock=' + __test_royal_tomb_lamp_stock(bid))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_phase2_ok')

    // Phase advance resets resources. Spent artisan (delivered_phase=1) leaves on next tick;
    // artisan2 never delivered — credits phase 2 (one guild-load per trip, no free refill).
    if (__test_monument_resource_pct(bid, RESOURCE_CLAY) != 0
            || __test_monument_resource_pct(bid, RESOURCE_PAINT) != 0) {
        __log_info_native('[test:151] phase2 should reset clay/paint')
        __test_signal_ready()
        return
    }
    if (__test_royal_tomb_lamp_stock(bid) < 1) {
        __test_monument_add_resource(bid, RESOURCE_LAMPS, 20)
    }

    __test_figure_action_perform(artisan) // spent → leave WORK
    __test_figure_action_perform(artisan2) // fresh load → fill phase 2
    if (__test_monument_resource_pct(bid, RESOURCE_CLAY) < 100
            || __test_monument_resource_pct(bid, RESOURCE_PAINT) < 100) {
        __log_info_native('[test:151] artisan2 deliver clay='
            + __test_monument_resource_pct(bid, RESOURCE_CLAY)
            + ' paint=' + __test_monument_resource_pct(bid, RESOURCE_PAINT))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_new_trip_ok')

    if (!__test_figure_is_alive(mason)) {
        mason = __test_royal_tomb_attach_worker(bid, FIGURE_STONEMASON)
        if (!mason) {
            __log_info_native('[test:151] reattach mason for phase3 failed')
            __test_signal_ready()
            return
        }
    }
    __test_building_update_day(bid)
    if (__test_monument_phase(bid) != 3) {
        __log_info_native('[test:151] after artisan2 want phase 3 got ' + __test_monument_phase(bid)
            + ' clay=' + __test_monument_resource_pct(bid, RESOURCE_CLAY)
            + ' paint=' + __test_monument_resource_pct(bid, RESOURCE_PAINT)
            + ' stock=' + __test_royal_tomb_lamp_stock(bid))
        __test_signal_ready()
        return
    }
    __log_marker('rt_live_phase3_ok')
    __log_marker('rt_live_phase_walk_ok')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'rt_live_placed_ok',
        'rt_live_phase1_ok',
        'rt_live_gate_no_mason_ok',
        'rt_live_gate_no_artisan_ok',
        'rt_live_clay_paint_ok',
        'rt_live_two_artisans_ok',
        'rt_live_phase2_ok',
        'rt_live_new_trip_ok',
        'rt_live_phase3_ok',
        'rt_live_phase_walk_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:151] missing marker ' + required[i])
            return false
        }
    }
    return true
}
