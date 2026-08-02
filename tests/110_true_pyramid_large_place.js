// Large true (smooth) pyramid — place, height→polish→finish.
// Height courses through 35 (like large stepped); polish 36–40 (5 layers);
// terminal 41; finish = set_phase(phases_total).

function find_large_true_pyramid_main() {
    for (var bi = 1; bi < 500; bi++) {
        if (__building_type(bi) != BUILDING_LARGE_PYRAMID) continue
        var b = city.get_building(bi)
        if (b && !b.prev_part_building_id) return bi
    }
    return 0
}

function run_test() {
    __log_info_native('[test:110] large true pyramid place + phase walk (height→polish→finish)')
    test_ensure_city_session('data/default.map')

    if (!__scenario_building_allowed(BUILDING_LARGE_PYRAMID)) {
        __scenario_building_allow(BUILDING_LARGE_PYRAMID, true)
    }

    __test_set_treasury(500000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    var bid = 0
    var candidates = [
        {x: cx - 10, y: cy - 10}, {x: cx, y: cy}, {x: 30, y: 30}, {x: 24, y: 24}
    ]
    for (var i = 0; i < candidates.length && !bid; i++) {
        bid = test_building_place(BUILDING_LARGE_PYRAMID, candidates[i].x, candidates[i].y)
    }
    if (!bid) {
        bid = test_building_place(BUILDING_LARGE_PYRAMID, -1, -1)
    }
    if (!bid) {
        __log_info_native('[test:110] failed to place BUILDING_LARGE_PYRAMID')
        __test_signal_ready()
        return
    }

    var tile = __building_tile(bid)
    __log_marker('true_pyramid_large_placed_ok:' + bid + ':' + tile.x + ',' + tile.y)

    if (__building_type(bid) != BUILDING_LARGE_PYRAMID) {
        __log_info_native('[test:110] unexpected type ' + __building_type(bid))
        __test_signal_ready()
        return
    }

    var parts = [bid]
    var cur = city.get_building(bid)
    while (cur && cur.next_part_building_id > 0 && parts.length < 256) {
        parts.push(cur.next_part_building_id)
        cur = city.get_building(cur.next_part_building_id)
    }
    // 20×20 / 2×2 = 100 parts
    if (parts.length >= 80) {
        __log_marker('true_pyramid_large_parts_ok:' + parts.length)
    } else {
        __log_marker('true_pyramid_large_parts_fail:' + parts.length)
    }

    var mon0 = city.get_monument(bid)
    if (!mon0) {
        __log_info_native('[test:110] city.get_monument failed')
        __test_signal_ready()
        return
    }
    var total = mon0.phases_total()
    // height 0–35 + polish 36–40 + terminal → phases_total >= 41
    if (total < 41) {
        __log_info_native('[test:110] phases_total=' + total + ' want >=41')
        __log_marker('true_pyramid_large_schedule_fail:' + total)
        __test_signal_ready()
        return
    }
    __log_marker('true_pyramid_large_schedule_ok:' + total)

    var all_ok = true
    var check_phases = [0, 2, 12, 24, 35, 36, 40]
    for (var ci = 0; ci < check_phases.length; ci++) {
        var want = check_phases[ci]
        __test_monument_set_phase(bid, want)
        __test_pump_frames(2)
        var got = __test_monument_phase(bid)
        if (got != want) {
            __log_info_native('[test:110] phase want ' + want + ' got ' + got)
            all_ok = false
        }
    }

    __test_monument_set_phase(bid, total)
    __test_pump_frames(2)
    var fin = __test_monument_phase(bid)
    if (fin != -1 && fin != 255) {
        __log_info_native('[test:110] finish want -1 after set_phase(total), got ' + fin)
        all_ok = false
    }
    __log_marker('true_pyramid_large_finish_phase:' + fin)

    city_update_monthly_monument_rating({})
    var rating = city.rating.monument | 0
    if (rating >= 32) {
        __log_marker('true_pyramid_large_rating_ok:' + rating)
    } else {
        __log_info_native('[test:110] monument rating want >=32 got ' + rating)
        __log_marker('true_pyramid_large_rating_fail:' + rating)
        all_ok = false
    }

    if (all_ok) {
        __log_marker('true_pyramid_large_phases_ok')
    } else {
        __log_marker('true_pyramid_large_phases_fail')
    }

    // Polish mid-band @36: no limestone/stone; guild must still request stonemasons.
    // set_phase fills tiles to 200 → park mid-progress before any day tick / pump.
    __test_monument_set_phase(bid, 36)
    __test_monument_set_all_progress(bid, 50)
    __test_pump_frames(2)
    var mon36 = city.get_monument(bid)
    if (mon36 && mon36.phase() == 36) {
        var pct = mon36.material_pct_min()
        if (pct == 100) {
            __log_marker('true_pyramid_large_polish_no_lime_ok')
        } else {
            __log_marker('true_pyramid_large_polish_pct_fail:' + pct)
        }
        if (mon36.need_stonemason()) {
            __log_marker('true_pyramid_large_polish_mason_ok')
        } else {
            __log_marker('true_pyramid_large_polish_mason_fail')
        }
    } else {
        __log_marker('true_pyramid_large_polish_phase_fail')
    }

    var save_name = 'test_110_true_pyramid_large_polish.svx'
    if (!__game_write_savegame(save_name)) {
        __log_marker('true_pyramid_large_saveload_skipped')
    } else if (!__game_load_savegame(save_name)) {
        __game_delete_savegame(save_name)
        __log_marker('true_pyramid_large_saveload_skipped')
    } else {
        __game_delete_savegame(save_name)
        var found = find_large_true_pyramid_main()
        var ph = found ? __test_monument_phase(found) : -999
        if (!found || ph != 36) {
            __log_info_native('[test:110] after load found=' + found + ' phase=' + ph)
            __log_marker('true_pyramid_large_saveload_fail:' + ph)
        } else {
            __log_marker('true_pyramid_large_saveload_ok:' + found)
            bid = found
        }
    }

    // Natural finish: 35→36 clears; 40→41 keeps; 41→FINISHED
    __test_monument_set_phase(bid, 35)
    var prog35 = __test_monument_min_progress(bid)
    __test_building_update_day(bid)
    var ph36 = __test_monument_phase(bid)
    var prog36 = __test_monument_min_progress(bid)
    if (ph36 == 36 && prog35 >= 200 && prog36 == 0) {
        __log_marker('true_pyramid_large_polish_clear_ok')
    } else {
        __log_info_native('[test:110] polish clear want phase=36 progress=0 got ph=' + ph36
            + ' prog35=' + prog35 + ' prog36=' + prog36)
        __log_marker('true_pyramid_large_polish_clear_fail')
    }

    __test_monument_set_phase(bid, 40)
    var prog40 = __test_monument_min_progress(bid)
    __test_building_update_day(bid)
    var ph41 = __test_monument_phase(bid)
    var prog41 = __test_monument_min_progress(bid)
    if (ph41 == 41 && prog40 >= 200 && prog41 >= 200) {
        __log_marker('true_pyramid_large_terminal_keep_ok')
    } else {
        __log_info_native('[test:110] terminal keep want phase=41 progress>=200 got ph=' + ph41
            + ' prog40=' + prog40 + ' prog41=' + prog41)
        __log_marker('true_pyramid_large_terminal_keep_fail')
    }

    function find_msg_key(key) {
        var n = __city_message_count()
        for (var i = n - 1; i >= 0; i--) {
            if (__lang_get_message_id(__city_message_mm_text_id(i)) == key) {
                return i
            }
        }
        return -1
    }

    var before = __city_message_count()
    __test_building_update_day(bid)
    var ph_fin = __test_monument_phase(bid)
    var msg = find_msg_key('pyramid_congratulations')
    if ((ph_fin == -1 || ph_fin == 255) && msg >= 0) {
        __log_marker('true_pyramid_large_congrats_ok')
    } else {
        __log_info_native('[test:110] congrats want finished+msg got ph=' + ph_fin
            + ' msg=' + msg + ' before_msgs=' + before)
        __log_marker('true_pyramid_large_congrats_fail')
    }

    __log_info_native('[test:110] done')
    __test_signal_ready()
}

function check_valid() {
    var required = [
        'true_pyramid_large_placed_ok',
        'true_pyramid_large_schedule_ok',
        'true_pyramid_large_phases_ok',
        'true_pyramid_large_polish_no_lime_ok',
        'true_pyramid_large_polish_mason_ok',
        'true_pyramid_large_polish_clear_ok',
        'true_pyramid_large_terminal_keep_ok',
        'true_pyramid_large_congrats_ok',
        'true_pyramid_large_rating_ok'
    ]
    for (var i = 0; i < required.length; i++) {
        if (!__test_find_inlog(required[i])) {
            __log_info_native('[test:110] missing: ' + required[i])
            return false
        }
    }
    if (__test_find_inlog('true_pyramid_large_phases_fail')
        || __test_find_inlog('true_pyramid_large_polish_pct_fail')
        || __test_find_inlog('true_pyramid_large_polish_mason_fail')
        || __test_find_inlog('true_pyramid_large_saveload_fail')
        || __test_find_inlog('true_pyramid_large_polish_clear_fail')
        || __test_find_inlog('true_pyramid_large_terminal_keep_fail')
        || __test_find_inlog('true_pyramid_large_congrats_fail')
        || __test_find_inlog('true_pyramid_large_rating_fail')) {
        return false
    }
    if (!__test_find_inlog('true_pyramid_large_saveload_skipped')
        && !__test_find_inlog('true_pyramid_large_saveload_ok')) {
        __log_info_native('[test:110] missing saveload marker')
        return false
    }
    return true
}
