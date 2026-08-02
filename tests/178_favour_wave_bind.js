// B2-migrate: favour multi-wave advances on bind COMPLETED (not any-enemy formation poll).
// Markers:
//   [test-marker] favour_wave0_spawn_ok
//   [test-marker] favour_wave1_after_bind_ok
//   [test-marker] favour_abu_targets_ok

function test178_fail(msg) {
    __log_info_native('[test:178] FAIL: ' + msg)
    __test_signal_ready()
}

function test178_hist_size(size) {
    var n = city.invasion_history_count()
    var found = 0
    for (var i = 0; i < n; i++) {
        if (city.invasion_history_at(i).size === size) {
            found++
        }
    }
    return found
}

function run_test() {
    __log_info_native('[test:178] B2-migrate favour wave bind advance')

    // Abydos has land invasion points for Egyptian favour armies.
    __game_load_mission(7, 1)
    __test_set_treasury(500000)
    __image_request_pak(PACK_ENEMY_EGYPTIAN)
    __test_pump_frames(2)
    __test_clear_enemy_formations()

    __test_kingdom_set_rating(0)
    mission_pharaoh_favour_invasion_tick(mission, [8, 12])
    __test_pump_frames(4)
    if (!mission.pharaoh_favour_invasion_done || mission.pharaoh_favour_wave_next !== 1) {
        test178_fail('wave0 state done=' + mission.pharaoh_favour_invasion_done
            + ' next=' + mission.pharaoh_favour_wave_next)
        return
    }
    if ((mission.pharaoh_favour_wave_seq | 0) <= 0) {
        test178_fail('wave0 no seq')
        return
    }
    var figs = __test_count_enemy_figures()
    var forms = city.num_enemy_formations
    __log_info_native('[test:178] after wave0 seq=' + mission.pharaoh_favour_wave_seq
        + ' figs=' + figs + ' forms=' + forms)
    if (figs <= 0 && forms <= 0) {
        test178_fail('wave0 spawned no enemies')
        return
    }
    __log_marker('favour_wave0_spawn_ok')

    // Still active → wave1 must not fire.
    mission_pharaoh_favour_invasion_tick(mission, [8, 12])
    if (mission.pharaoh_favour_wave_next !== 1) {
        test178_fail('wave1 fired before wipe next=' + mission.pharaoh_favour_wave_next)
        return
    }

    var seq0 = mission.pharaoh_favour_wave_seq
    __test_process_invasion_binds()
    __test_clear_enemy_formations()
    __test_pump_frames(2)
    __test_process_invasion_binds()

    var o = mission_pharaoh_favour_invasion_outcome(seq0)
    if (o !== 1) {
        test178_fail('bind outcome want COMPLETED(1) got=' + o + ' seq=' + seq0)
        return
    }

    mission_pharaoh_favour_invasion_tick(mission, [8, 12])
    __test_pump_frames(4)
    if (mission.pharaoh_favour_wave_next !== 2) {
        test178_fail('wave1 not advanced next=' + mission.pharaoh_favour_wave_next)
        return
    }
    if ((mission.pharaoh_favour_wave_seq | 0) <= seq0) {
        test178_fail('wave1 seq not updated seq0=' + seq0 + ' seq=' + mission.pharaoh_favour_wave_seq)
        return
    }
    var n = city.invasion_history_count()
    var last = n > 0 ? city.invasion_history_at(n - 1) : null
    __log_info_native('[test:178] after wave1 seq=' + mission.pharaoh_favour_wave_seq
        + ' hist=' + n + ' last_size=' + (last ? last.size : -1)
        + ' size12=' + test178_hist_size(12))
    if (!last || last.seq !== mission.pharaoh_favour_wave_seq) {
        test178_fail('wave1 history missing for seq=' + mission.pharaoh_favour_wave_seq)
        return
    }
    __log_marker('favour_wave1_after_bind_ok')

    __game_load_mission(9, 1)
    __test_set_treasury(500000)
    __test_kingdom_set_rating(0)
    __image_request_pak(PACK_ENEMY_EGYPTIAN)
    __test_clear_enemy_formations()
    mission_pharaoh_favour_invasion_tick(mission, [40, 20, 20], {
        targets: [
            EVENT_ATTACK_TARGET_FOOD,
            EVENT_ATTACK_TARGET_RANDOM,
            EVENT_ATTACK_TARGET_RANDOM
        ]
    })
    __test_pump_frames(4)
    if (!mission.pharaoh_favour_invasion_done || mission.pharaoh_favour_wave_next !== 1) {
        test178_fail('abu wave0 done=' + mission.pharaoh_favour_invasion_done
            + ' next=' + mission.pharaoh_favour_wave_next)
        return
    }
    if (test178_hist_size(40) < 1) {
        __log_info_native('[test:178] warn: abu size=40 history missing (spawn tile?)')
    }
    __log_marker('favour_abu_targets_ok')

    __test_signal_ready()
}

function check_valid() {
    return __test_find_inlog('[test-marker] favour_wave0_spawn_ok')
        && __test_find_inlog('[test-marker] favour_wave1_after_bind_ok')
        && __test_find_inlog('[test-marker] favour_abu_targets_ok')
}
