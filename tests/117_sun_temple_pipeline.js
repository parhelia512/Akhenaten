// Sun Temple C8.6 — pipeline (timber→carve→+160→congrats), stockpile block,
// rating (FINISHED→13; alone unfinished <13), mid-build save/load.
// Place/reject covered by tests/110_sun_temple_place.js.

function find_msg_key(key) {
    var total = __city_message_count()
    for (var i = total - 1; i >= 0; i--) {
        if (__lang_get_message_id(__city_message_mm_text_id(i)) == key) {
            return i
        }
    }
    return -1
}

function find_sun_temple() {
    for (var bi = 1; bi < 800; bi++) {
        if (__building_type(bi) == BUILDING_SUN_TEMPLE) {
            return bi
        }
    }
    return 0
}

function place_st_with_sandstone(amount) {
    if (!__scenario_building_allowed(BUILDING_SUN_TEMPLE)) {
        __scenario_building_allow(BUILDING_SUN_TEMPLE, true)
    }
    if (!__scenario_building_allowed(BUILDING_STORAGE_YARD)) {
        __scenario_building_allow(BUILDING_STORAGE_YARD, true)
    }
    __test_set_treasury(500000)

    var sy = 0
    var yard_spots = [
        {x: 8, y: 8}, {x: 12, y: 12}, {x: 20, y: 8}, {x: 8, y: 40}
    ]
    for (var yi = 0; yi < yard_spots.length && !sy; yi++) {
        sy = test_staffed_yard_with_resource(RESOURCE_SANDSTONE, amount, yard_spots[yi].x, yard_spots[yi].y)
    }
    if (!sy) {
        sy = test_staffed_yard_with_resource(RESOURCE_SANDSTONE, amount, -1, -1)
    }
    if (!sy) {
        return {bid: 0, sy: 0}
    }

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    var bid = test_building_place(BUILDING_SUN_TEMPLE, cx - 5, cy - 5)
    if (!bid) {
        bid = test_building_place(BUILDING_SUN_TEMPLE, 40, 40)
    }
    if (!bid) {
        bid = test_building_place(BUILDING_SUN_TEMPLE, -1, -1)
    }
    __test_process_events()
    __test_pump_frames(2)
    return {bid: bid, sy: sy}
}

function run_test() {
    __log_info_native('[test:117] sun temple pipeline + stockpile + rating + saveload')
    test_reload_city_session('data/default.map')

    var placed = place_st_with_sandstone(220)
    var bid = placed.bid
    var sy = placed.sy
    if (!bid || !sy) {
        __log_info_native('[test:117] place failed bid=' + bid + ' sy=' + sy)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_pipeline_placed:' + bid)

    // --- Stockpile blocks SY→monument task on phase 4 ---
    __test_monument_set_phase(bid, 4)
    __test_storage_yard_add_resource(sy, RESOURCE_SANDSTONE, 160)
    var task_open = __test_storageyard_monument_task_resource(sy)
    if (task_open != RESOURCE_SANDSTONE) {
        __log_info_native('[test:117] expected sandstone monument task, got ' + task_open)
        __log_marker('sun_temple_stockpile_pre_fail:' + task_open)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_deliver_open_ok')

    city.resources.toggle_stockpiled(RESOURCE_SANDSTONE)
    if (!__city_resource_is_stockpiled(RESOURCE_SANDSTONE)) {
        __log_info_native('[test:117] stockpile toggle failed')
        __log_marker('sun_temple_stockpile_toggle_fail')
        __test_signal_ready()
        return
    }
    var task_blocked = __test_storageyard_monument_task_resource(sy)
    if (task_blocked != 0) {
        __log_info_native('[test:117] stockpile should block monument task, got ' + task_blocked)
        __log_marker('sun_temple_stockpile_block_fail:' + task_blocked)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_stockpile_block_ok')
    // Clear stockpile for the rest of the test.
    city.resources.toggle_stockpiled(RESOURCE_SANDSTONE)

    // --- Rating: unfinished body < 13 ---
    city_update_monthly_monument_rating({})
    var rating_mid = city.rating.monument | 0
    if (rating_mid >= 13) {
        __log_info_native('[test:117] unfinished rating want <13 got ' + rating_mid)
        __log_marker('sun_temple_rating_mid_fail:' + rating_mid)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_rating_mid_ok:' + rating_mid)

    // --- Mid-build save/load at phase 4 ---
    var save_name = 'test_117_sun_temple_mid.svx'
    if (!__game_write_savegame(save_name)) {
        __log_marker('sun_temple_saveload_skipped')
    } else if (!__game_load_savegame(save_name)) {
        __game_delete_savegame(save_name)
        __log_marker('sun_temple_saveload_skipped')
    } else {
        __game_delete_savegame(save_name)
        bid = find_sun_temple()
        var ph = bid ? __test_monument_phase(bid) : -999
        if (!bid || ph != 4) {
            __log_info_native('[test:117] after load bid=' + bid + ' phase=' + ph)
            __log_marker('sun_temple_saveload_fail:' + ph)
            __test_signal_ready()
            return
        }
        __log_marker('sun_temple_saveload_ok:' + bid)
    }
    if (!bid) {
        bid = find_sun_temple()
    }
    if (!bid) {
        __log_info_native('[test:117] lost sun temple after saveload branch')
        __test_signal_ready()
        return
    }

    // --- Pipeline: phase 2 timber → progress → 3 → 4 sand → terminal → FINISHED + congrats ---
    // Restart clean for deterministic phases (saveload may have left stockpile/yard odd).
    test_reload_city_session('data/default.map')
    placed = place_st_with_sandstone(220 + 160)
    bid = placed.bid
    if (!bid) {
        __log_info_native('[test:117] re-place for pipeline failed')
        __test_signal_ready()
        return
    }

    // Skip leveling: jump to phase 2 (timber).
    __test_monument_set_phase(bid, 2)
    if (!__test_monument_add_resource(bid, RESOURCE_TIMBER, 100)) {
        __log_info_native('[test:117] timber add_resource failed')
        __log_marker('sun_temple_timber_fail')
        __test_signal_ready()
        return
    }
    if (__test_monument_resource_pct(bid, RESOURCE_TIMBER) < 100) {
        __log_info_native('[test:117] timber pct not full')
        __log_marker('sun_temple_timber_pct_fail')
        __test_signal_ready()
        return
    }
    __test_building_update_day(bid)
    var ph3 = __test_monument_phase(bid)
    if (ph3 != 3) {
        __log_info_native('[test:117] after timber want phase 3 got ' + ph3)
        __log_marker('sun_temple_phase3_fail:' + ph3)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_timber_advance_ok')

    // Phase 3 carve (no resource) → advances on next update_day.
    __test_building_update_day(bid)
    var ph4 = __test_monument_phase(bid)
    if (ph4 != 4) {
        __log_info_native('[test:117] after carve want phase 4 got ' + ph4)
        __log_marker('sun_temple_phase4_fail:' + ph4)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_carve_advance_ok')

    if (!__test_monument_add_resource(bid, RESOURCE_SANDSTONE, 160)) {
        __log_info_native('[test:117] sandstone build add_resource failed')
        __log_marker('sun_temple_sand_fail')
        __test_signal_ready()
        return
    }
    __test_building_update_day(bid)
    var ph5 = __test_monument_phase(bid)
    if (ph5 != 5) {
        __log_info_native('[test:117] after sand want terminal 5 got ' + ph5)
        __log_marker('sun_temple_terminal_fail:' + ph5)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_sand_advance_ok')

    // Terminal → FINISHED + congratulations.
    var msg_before = __city_message_count()
    __test_building_update_day(bid)
    var ph_fin = __test_monument_phase(bid)
    var msg_idx = find_msg_key('sun_temple_congratulations')
    if ((ph_fin == -1 || ph_fin == 255) && msg_idx >= 0) {
        __log_marker('sun_temple_finish_congrats_ok')
    } else {
        __log_info_native('[test:117] finish want FINISHED+congrats got ph=' + ph_fin
            + ' msg_idx=' + msg_idx + ' msgs=' + msg_before + '→' + __city_message_count())
        __log_marker('sun_temple_finish_congrats_fail')
        __test_signal_ready()
        return
    }

    city_update_monthly_monument_rating({})
    var rating_done = city.rating.monument | 0
    // weight 4 → trunc(2.25*4+4.5)=13
    if (rating_done < 13) {
        __log_info_native('[test:117] finished rating want >=13 got ' + rating_done)
        __log_marker('sun_temple_rating_done_fail:' + rating_done)
        __test_signal_ready()
        return
    }
    __log_marker('sun_temple_rating_done_ok:' + rating_done)

    // Waset soft-check: ST alone = 13 < 20 goal (no small pyramid here).
    if (rating_done < 20) {
        __log_marker('sun_temple_waset_alone_below_20_ok')
    } else {
        __log_info_native('[test:117] ST alone should be <20 for Waset soft-check, got ' + rating_done)
        __log_marker('sun_temple_waset_alone_fail:' + rating_done)
        __test_signal_ready()
        return
    }

    __log_info_native('[test:117] PASS')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'sun_temple_pipeline_placed',
        'sun_temple_deliver_open_ok',
        'sun_temple_stockpile_block_ok',
        'sun_temple_rating_mid_ok',
        'sun_temple_timber_advance_ok',
        'sun_temple_carve_advance_ok',
        'sun_temple_sand_advance_ok',
        'sun_temple_finish_congrats_ok',
        'sun_temple_rating_done_ok',
        'sun_temple_waset_alone_below_20_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog('[test-marker] ' + required[i])
            && !__test_find_inlog(required[i])) {
            __log_info_native('[test:117] missing marker ' + required[i])
            return false
        }
    }
    if (__test_find_inlog('sun_temple_saveload_fail')) {
        return false
    }
    if (!__test_find_inlog('sun_temple_saveload_skipped')
        && !__test_find_inlog('sun_temple_saveload_ok')) {
        __log_info_native('[test:117] missing saveload ok/skip marker')
        return false
    }
    return true
}
