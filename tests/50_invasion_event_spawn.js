// B2 Phase 1–5: native EVENT_TYPE_INVASION spawn + deferred on_completed +
// favour BY_FAVOUR + chain-only child.
//
// Markers:
//   [test-marker] invasion_event_spawn_ok
//   [test-marker] invasion_event_no_sync_chain
//   [test-marker] invasion_event_resolve_ok
//   [test-marker] invasion_event_gate_off
//   [test-marker] invasion_event_favour_ok
//   [test-marker] invasion_event_chain_only_ok

function run_test() {
    __log_info_native('[test:50] invasion event spawn + deferred resolve + favour + chain')
    test_ensure_city_session('data/default.map')

    __image_request_pak(PACK_ENEMY_BARBARIAN)
    __image_request_pak(PACK_ENEMY_EGYPTIAN)
    __test_pump_frames(2)
    __test_clear_enemy_formations()

    var mid_y = ((__scenario_map.height / 2) | 0)

    // --- Gate on: spawn + no sync chain + deferred resolve ---
    __test_set_use_native_invasion_events(1)
    __test_set_scenario_enemy_id(ENEMY_0_BARBARIAN)
    var kr_before = city.rating_kingdom

    city.create_chain_event({
        tag_id: 5001,
        type: EVENT_TYPE_REPUTATION_INCREASE,
        amount: 3,
        trigger: EVENT_TRIGGER_ONLY_VIA_EVENT
    })
    var inv = city.create_invasion_event({
        tag_id: 5002,
        invader: 1, // EVENT_INVADER_ENEMY
        amount: 10,
        invasion_attack_target: 4, // RANDOM
        trigger: EVENT_TRIGGER_ONCE,
        tilex: 2,
        tiley: mid_y
    })
    inv.set_completed_action_tag(5001)
    inv.execute()
    __test_pump_frames(4)

    var enemy_figs = __test_count_enemy_figures()
    var formations = city.num_enemy_formations
    __log_info_native('[test:50] after spawn figures=' + enemy_figs + ' formations=' + formations)
    if (enemy_figs > 0 || formations > 0) {
        __log_marker('invasion_event_spawn_ok')
    } else {
        __log_info_native('[test:50] expected enemy figures/formations > 0 after spawn')
        __log_marker('invasion_event_spawn_fail')
    }

    if (city.rating_kingdom == kr_before) {
        __log_marker('invasion_event_no_sync_chain')
    } else {
        __log_info_native('[test:50] KR changed on spawn tick: ' + kr_before + ' -> ' + city.rating_kingdom)
        __log_marker('invasion_event_no_sync_chain_fail')
    }

    var pending_id = __test_pending_invasion_id_for_tag(5002)
    __log_info_native('[test:50] pending invasion_id=' + pending_id)

    __test_process_scenario_events()
    __test_clear_enemy_formations()
    __test_pump_frames(2)
    __test_process_scenario_events()
    __test_process_scenario_events()

    if (city.rating_kingdom == kr_before + 3) {
        __log_marker('invasion_event_resolve_ok')
    } else {
        __log_info_native('[test:50] expected KR ' + (kr_before + 3) + ' after resolve, got ' + city.rating_kingdom)
        __log_marker('invasion_event_resolve_fail:' + city.rating_kingdom)
    }

    // --- B2c: ONLY_VIA invasion child fires via parent chain ---
    __test_clear_enemy_formations()
    city.create_invasion_event({
        tag_id: 5010,
        invader: 1,
        amount: 8,
        invasion_attack_target: 4,
        trigger: EVENT_TRIGGER_ONLY_VIA_EVENT,
        tilex: 2,
        tiley: mid_y
    })
    // Fire chain master as if parent on_completed pointed here.
    __city_event_fire_chain(5010)
    __test_process_scenario_events() // ACTIVATED_12 child → spawn
    __test_pump_frames(4)
    var chain_figs = __test_count_enemy_figures()
    if (chain_figs > 0) {
        __log_marker('invasion_event_chain_only_ok')
    } else {
        __log_info_native('[test:50] chain-only invasion spawned 0 figures')
        __log_marker('invasion_event_chain_only_fail')
    }
    __test_clear_enemy_formations()

    // --- B2b: favour BY_FAVOUR at KR=0 ---
    city.create_invasion_event({
        tag_id: 5020,
        invader: 3, // EVENT_INVADER_PHARAOH
        amount: 40,
        invasion_attack_target: 4,
        trigger: 16, // EVENT_TRIGGER_BY_FAVOUR
        tilex: 2,
        tiley: mid_y
    })
    city.kingdome.rating = 0
    __test_process_scenario_events()
    __test_pump_frames(4)
    var favour_figs = __test_count_enemy_figures()
    if (favour_figs > 0) {
        __log_marker('invasion_event_favour_ok')
    } else {
        __log_info_native('[test:50] favour KR=0 spawned 0 figures')
        __log_marker('invasion_event_favour_fail')
    }
    __test_clear_enemy_formations()

    // --- Gate off: must not spawn ---
    __test_set_use_native_invasion_events(0)
    var before = city.num_enemy_formations
    city.create_invasion_event({
        tag_id: 5003,
        invader: 1,
        amount: 12,
        invasion_attack_target: 4,
        trigger: EVENT_TRIGGER_ONCE,
        tilex: 2,
        tiley: mid_y
    }).execute()
    __test_process_scenario_events()
    if (city.num_enemy_formations == before) {
        __log_marker('invasion_event_gate_off')
    } else {
        __log_info_native('[test:50] gate off but formations grew to ' + city.num_enemy_formations)
        __log_marker('invasion_event_gate_off_fail')
    }

    __test_set_use_native_invasion_events(0)
    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'invasion_event_spawn_ok',
        'invasion_event_no_sync_chain',
        'invasion_event_resolve_ok',
        'invasion_event_chain_only_ok',
        'invasion_event_favour_ok',
        'invasion_event_gate_off'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:50] missing marker: ' + marker)
            return false
        }
    }
    return true
}
