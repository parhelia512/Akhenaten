// B8: troops ask with defeat leaf → distant battle uses real egyptian strength
// (no equal-abstract win-always). No bataillons → lose; strong enough → win.
// Markers:
//   [test-marker] troops_battle_defeat_ok
//   [test-marker] troops_battle_win_ok
//   [test-marker] troops_no_defeat_immediate_ok

function test179_fail(msg) {
    __log_info_native('[test:179] FAIL: ' + msg)
    __test_signal_ready()
}

function test179_fire_troops(tag, amount, ok_tag, defeat_tag) {
    var request = city.create_good_request({
        tag_id: tag,
        resource: RESOURCE_TROOPS,
        amount: amount,
        months_initial: 12,
        city: "Men-nefer"
    })
    city.create_chain_event({
        tag_id: ok_tag,
        type: EVENT_TYPE_REPUTATION_INCREASE,
        amount: 5
    })
    city.create_chain_event({
        tag_id: defeat_tag,
        type: EVENT_TYPE_REPUTATION_DECREASE,
        amount: 3
    })
    request.set_completed_action_tag(ok_tag)
    request.set_defeat_action_tag(defeat_tag)
    request.execute()
    __test_process_events()
}

function run_test() {
    __log_info_native('[test:179] B8 troops distant-battle win/defeat')

    test_ensure_city_session('data/default.map')
    __test_set_treasury(500000)
    __test_kingdom_set_rating(50)

    // --- defeat: dispatch with no empire-service bataillons ---
    test179_fire_troops(17910, 40, 17911, 17912)
    if (!city.has_active_request(RESOURCE_TROOPS)) {
        test179_fail('defeat: request not active')
        return
    }
    var seq0 = __test_request_cleared_seq()
    var kr0 = __test_kingdom_rating()
    __test_request_dispatch_by_tag(17910)
    if (__test_distant_battle_enemy_strength() !== 40) {
        test179_fail('defeat: enemy strength want 40 got=' + __test_distant_battle_enemy_strength())
        return
    }
    if (__test_distant_battle_egyptian_strength() !== 0) {
        test179_fail('defeat: egyptian want 0 got=' + __test_distant_battle_egyptian_strength())
        return
    }
    __test_process_distant_battle_month()
    __test_process_scenario_events()
    if (__test_request_cleared_seq() <= seq0
        || __test_request_cleared_tag_id() !== 17910
        || __test_request_cleared_fulfilled() !== 0) {
        test179_fail('defeat: cleared tag=' + __test_request_cleared_tag_id()
            + ' ful=' + __test_request_cleared_fulfilled())
        return
    }
    if (__test_kingdom_rating() !== kr0 - 3) {
        test179_fail('defeat: KR want ' + (kr0 - 3) + ' got=' + __test_kingdom_rating())
        return
    }
    __log_marker('troops_battle_defeat_ok')

    // --- win: override egyptian >= enemy after dispatch ---
    test_ensure_city_session('data/default.map')
    __test_set_treasury(500000)
    __test_kingdom_set_rating(50)
    test179_fire_troops(17920, 40, 17921, 17922)
    seq0 = __test_request_cleared_seq()
    kr0 = __test_kingdom_rating()
    __test_request_dispatch_by_tag(17920)
    __test_distant_battle_set_egyptian_strength(40)
    __test_process_distant_battle_month()
    __test_process_scenario_events()
    if (__test_request_cleared_seq() <= seq0
        || __test_request_cleared_tag_id() !== 17920
        || __test_request_cleared_fulfilled() !== 1) {
        test179_fail('win: cleared tag=' + __test_request_cleared_tag_id()
            + ' ful=' + __test_request_cleared_fulfilled())
        return
    }
    if (__test_kingdom_rating() !== kr0 + 5) {
        test179_fail('win: KR want ' + (kr0 + 5) + ' got=' + __test_kingdom_rating())
        return
    }
    __log_marker('troops_battle_win_ok')

    // --- no defeat leaf: immediate fulfill (Selima-style), no battle ---
    test_ensure_city_session('data/default.map')
    __test_set_treasury(500000)
    __test_kingdom_set_rating(50)
    var request = city.create_good_request({
        tag_id: 17930,
        resource: RESOURCE_TROOPS,
        amount: 10,
        months_initial: 8
    })
    city.create_chain_event({
        tag_id: 17931,
        type: EVENT_TYPE_REPUTATION_INCREASE,
        amount: 2
    })
    request.set_completed_action_tag(17931)
    request.execute()
    __test_process_events()
    seq0 = __test_request_cleared_seq()
    __test_request_force_fulfill(17930, 0)
    __test_process_events()
    if (__test_request_cleared_seq() <= seq0
        || __test_request_cleared_tag_id() !== 17930
        || __test_request_cleared_fulfilled() !== 1) {
        test179_fail('immediate: cleared tag=' + __test_request_cleared_tag_id()
            + ' ful=' + __test_request_cleared_fulfilled())
        return
    }
    // No defeat leaf → no distant battle deferral (Selima-style).
    if (city.has_active_request(RESOURCE_TROOPS)) {
        test179_fail('immediate: troops request still active after fulfill')
        return
    }
    __log_marker('troops_no_defeat_immediate_ok')

    __test_signal_ready()
}

function check_valid() {
    return __test_find_inlog('[test-marker] troops_battle_defeat_ok')
        && __test_find_inlog('[test-marker] troops_battle_win_ok')
        && __test_find_inlog('[test-marker] troops_no_defeat_immediate_ok')
}
