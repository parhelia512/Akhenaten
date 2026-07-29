// DP1/DP5: predicted delivery destination for producers (display-only QoL).
//
// Markers:
//   [test-marker] delivery_paths_allowlist_ok
//   [test-marker] delivery_paths_pottery_sy_ok
//   [test-marker] delivery_paths_empty_all_ok
//   [test-marker] delivery_paths_understaffed_ok
//   [test-marker] delivery_paths_clay_workshop_ok
//   [test-marker] delivery_paths_feature_ok

function test95_set_accept(yard, res) {
    var guard = 0
    while (yard.resource_state(res) != STORAGE_STATE_ACCEPT && guard < 8) {
        yard.cycle_resource_state(res)
        guard++
    }
    return yard.resource_state(res) == STORAGE_STATE_ACCEPT
}

function run_test() {
    __log_info_native('[test:95] delivery paths predict')
    test_ensure_city_session('data/default.map')
    __test_set_treasury(500000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0

    // Distinct tiles: for_storing requires distance > 0.
    var pottery = __test_building_create(BUILDING_POTTERY_WORKSHOP, cx - 6, cy)
    if (!pottery) {
        __log_info_native('[test:95] pottery create failed')
        __test_signal_ready()
        return
    }
    if (__test_building_shows_delivery_paths(pottery) != 1) {
        __log_info_native('[test:95] pottery should show delivery paths')
        __test_signal_ready()
        return
    }
    var granary = __test_building_create(BUILDING_GRANARY, cx, cy + 8)
    if (granary && __test_building_shows_delivery_paths(granary) != 0) {
        __log_info_native('[test:95] granary must not show producer delivery paths')
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_allowlist_ok')

    var sy = __test_building_create(BUILDING_STORAGE_YARD, cx + 6, cy)
    if (!sy) {
        __log_info_native('[test:95] storage yard create failed')
        __test_signal_ready()
        return
    }

    var yard = city.get_storage_yard(sy)
    if (!yard) {
        __log_info_native('[test:95] get_storage_yard failed')
        __test_signal_ready()
        return
    }
    // --no-resource leaves Refuse for most goods; force Accept for pottery/clay.
    if (!test95_set_accept(yard, RESOURCE_POTTERY)) {
        __log_info_native('[test:95] set pottery Accept failed')
        __test_signal_ready()
        return
    }
    if (!test95_set_accept(yard, RESOURCE_CLAY)) {
        __log_info_native('[test:95] set clay Accept failed')
        __test_signal_ready()
        return
    }

    __test_link_producer_for_delivery(pottery, sy)
    var dest = __test_predict_delivery(pottery)
    if (dest != sy) {
        __log_info_native('[test:95] pottery should predict sy ' + sy + ' got ' + dest
            + ' reason=' + __test_predict_delivery_reason(pottery))
        __test_signal_ready()
        return
    }
    if (__test_predict_delivery_kind(pottery) != 1) { // e_delivery_dest_kind::storage_yard
        __log_info_native('[test:95] kind want storage_yard(1) got ' + __test_predict_delivery_kind(pottery))
        __test_signal_ready()
        return
    }
    if (__test_predict_delivery_reason(pottery) != 0) { // ok
        __log_info_native('[test:95] reason want ok(0) got ' + __test_predict_delivery_reason(pottery))
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_pottery_sy_ok')

    __test_storage_toggle_empty_all(sy)
    dest = __test_predict_delivery(pottery)
    if (dest) {
        __log_info_native('[test:95] Empty All SY should yield no destination, got ' + dest)
        __test_signal_ready()
        return
    }
    if (__test_predict_delivery_reason(pottery) != 2) { // no_destination
        __log_info_native('[test:95] empty-all reason want no_destination(2) got ' + __test_predict_delivery_reason(pottery))
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_empty_all_ok')

    // Re-accept SY, then understaff → reason understaffed (no workshop for pottery).
    __test_storage_toggle_empty_all(sy)
    if (!test95_set_accept(yard, RESOURCE_POTTERY)) {
        __log_info_native('[test:95] restore pottery Accept failed')
        __test_signal_ready()
        return
    }
    __test_building_set_workers(sy, 0)
    dest = __test_predict_delivery(pottery)
    if (dest) {
        __log_info_native('[test:95] understaffed SY should yield no destination, got ' + dest)
        __test_signal_ready()
        return
    }
    if (__test_predict_delivery_reason(pottery) != 3) { // understaffed
        __log_info_native('[test:95] reason want understaffed(3) got ' + __test_predict_delivery_reason(pottery))
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_understaffed_ok')

    var clay = __test_building_create(BUILDING_CLAY_PIT, cx - 6, cy + 6)
    if (!clay) {
        __log_info_native('[test:95] clay pit create failed')
        __test_signal_ready()
        return
    }
    // Clay → pottery workshop (priority 3); keep SY understaffed so it does not steal clay.
    __test_link_producer_for_delivery(clay, pottery)
    dest = __test_predict_delivery(clay)
    if (dest != pottery) {
        __log_info_native('[test:95] clay should predict pottery ' + pottery + ' got ' + dest
            + ' reason=' + __test_predict_delivery_reason(clay)
            + ' kind=' + __test_predict_delivery_kind(clay))
        __test_signal_ready()
        return
    }
    if (__test_predict_delivery_kind(clay) != 3) { // workshop
        __log_info_native('[test:95] clay kind want workshop(3) got ' + __test_predict_delivery_kind(clay))
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_clay_workshop_ok')

    if (!game_features.gameui_show_delivery_paths) {
        __log_info_native('[test:95] gameui_show_delivery_paths should default ON')
        __test_signal_ready()
        return
    }
    game_features.gameui_show_delivery_paths = false
    if (game_features.gameui_show_delivery_paths) {
        __log_info_native('[test:95] failed to disable delivery paths flag')
        __test_signal_ready()
        return
    }
    game_features.gameui_show_delivery_paths = true
    __log_marker('delivery_paths_feature_ok')

    __test_signal_ready()
}

function check_valid() {
    return __test_find_inlog('delivery_paths_allowlist_ok')
        && __test_find_inlog('delivery_paths_pottery_sy_ok')
        && __test_find_inlog('delivery_paths_empty_all_ok')
        && __test_find_inlog('delivery_paths_understaffed_ok')
        && __test_find_inlog('delivery_paths_clay_workshop_ok')
        && __test_find_inlog('delivery_paths_feature_ok')
}
