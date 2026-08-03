// Recorded cart paths: granary/lodge keep last 3 handoff trails for hover UI.
//
// Markers:
//   [test-marker] delivery_paths_granary_push_ok
//   [test-marker] delivery_paths_ring3_ok
//   [test-marker] delivery_paths_evict_ok
//   [test-marker] delivery_paths_lodge_ok
//   [test-marker] delivery_paths_feature_ok

function test95_push_path(bid, tile_x, tile_y) {
    var pid = __test_recorded_path_acquire()
    if (!pid) {
        return 0
    }
    __test_recorded_path_append(pid, tile_x, tile_y)
    __test_building_push_recorded_path(bid, pid)
    return pid
}

function run_test() {
    __log_info_native('[test:95] delivery paths recorded')
    test_ensure_city_session('data/default.map')
    __test_set_treasury(500000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0

    var granary = __test_building_create(BUILDING_GRANARY, cx, cy)
    if (!granary) {
        __log_info_native('[test:95] granary create failed')
        __test_signal_ready()
        return
    }

    var p1 = test95_push_path(granary, cx, cy)
    if (!p1) {
        __log_info_native('[test:95] path acquire failed')
        __test_signal_ready()
        return
    }
    if (__test_building_recorded_path_at(granary, 0) != p1) {
        __log_info_native('[test:95] granary newest path want ' + p1
            + ' got ' + __test_building_recorded_path_at(granary, 0))
        __test_signal_ready()
        return
    }
    if (__test_recorded_path_tile_count(p1) < 1) {
        __log_info_native('[test:95] path should have tiles')
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_granary_push_ok')

    var p2 = test95_push_path(granary, cx + 1, cy)
    var p3 = test95_push_path(granary, cx + 2, cy)
    if (__test_building_recorded_path_at(granary, 0) != p3
        || __test_building_recorded_path_at(granary, 1) != p2
        || __test_building_recorded_path_at(granary, 2) != p1) {
        __log_info_native('[test:95] ring order want newest-first ' + p3 + ',' + p2 + ',' + p1
            + ' got ' + __test_building_recorded_path_at(granary, 0)
            + ',' + __test_building_recorded_path_at(granary, 1)
            + ',' + __test_building_recorded_path_at(granary, 2))
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_ring3_ok')

    var p4 = test95_push_path(granary, cx + 3, cy)
    if (__test_building_recorded_path_at(granary, 0) != p4
        || __test_building_recorded_path_at(granary, 1) != p3
        || __test_building_recorded_path_at(granary, 2) != p2) {
        __log_info_native('[test:95] after 4th push ring want ' + p4 + ',' + p3 + ',' + p2)
        __test_signal_ready()
        return
    }
    if (__test_recorded_path_used(p1) != 0) {
        __log_info_native('[test:95] oldest path should be released after eviction')
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_evict_ok')

    var lodge = __test_building_create(BUILDING_HUNTING_LODGE, cx + 8, cy)
    if (!lodge) {
        __log_info_native('[test:95] hunting lodge create failed')
        __test_signal_ready()
        return
    }
    var lp = test95_push_path(lodge, cx + 8, cy)
    if (__test_building_recorded_path_at(lodge, 0) != lp) {
        __log_info_native('[test:95] lodge should keep recorded path')
        __test_signal_ready()
        return
    }
    if (__test_building_recorded_path_at(granary, 0) != p4) {
        __log_info_native('[test:95] lodge push must not clear granary ring')
        __test_signal_ready()
        return
    }
    __log_marker('delivery_paths_lodge_ok')

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
    return __test_find_inlog('delivery_paths_granary_push_ok')
        && __test_find_inlog('delivery_paths_ring3_ok')
        && __test_find_inlog('delivery_paths_evict_ok')
        && __test_find_inlog('delivery_paths_lodge_ok')
        && __test_find_inlog('delivery_paths_feature_ok')
}
