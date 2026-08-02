// B9: pak_event dump names resources/cities and flags orphan_inbound / oob.
// Markers:
//   [test-marker] pak_dump_named_item_ok
//   [test-marker] pak_dump_orphan_inbound_ok
//   [test-marker] pak_dump_linked_not_orphan_ok
//   [test-marker] pak_dump_oob_ok

function test180_fail(msg) {
    __log_info_native('[test:180] FAIL: ' + msg)
    __test_signal_ready()
}

function run_test() {
    __log_info_native('[test:180] B9 pak dump triage UX')
    test_ensure_city_session('data/default.map')

    // Linked leaf: request ok → chain KR leaf.
    var leaf = city.create_chain_event({
        tag_id: 18002,
        type: EVENT_TYPE_REPUTATION_INCREASE,
        amount: 4
    })
    var request = city.create_good_request({
        tag_id: 18001,
        resource: RESOURCE_POTTERY,
        amount: 8,
        months_initial: 6,
        city: "Men-nefer"
    })
    request.set_completed_action_tag(18002)
    request.execute()

    // Orphan chain_only leaf — never referenced by any parent.
    city.create_chain_event({
        tag_id: 18003,
        type: EVENT_TYPE_REPUTATION_DECREASE,
        amount: 2,
        trigger: EVENT_TRIGGER_ONLY_VIA_EVENT
    })

    __test_dump_scenario_events()

    if (!__test_find_inlog('item=pottery(')) {
        test180_fail('named pottery item missing from pak_event dump')
        return
    }
    __log_marker('pak_dump_named_item_ok')

    if (!__test_find_inlog('orphan_inbound=1')) {
        test180_fail('orphan_inbound=1 missing')
        return
    }
    __log_marker('pak_dump_orphan_inbound_ok')

    if (!__test_find_inlog('pak_orphan_inbound_count:')) {
        test180_fail('pak_orphan_inbound_count missing')
        return
    }
    __log_marker('pak_dump_linked_not_orphan_ok')

    if (!__test_find_inlog('pak_oob_count:')) {
        test180_fail('pak_oob_count missing')
        return
    }
    __log_marker('pak_dump_oob_ok')

    if (!leaf) {
        test180_fail('setup helpers missing')
        return
    }

    __test_signal_ready()
}

function check_valid() {
    return __test_find_inlog('[test-marker] pak_dump_named_item_ok')
        && __test_find_inlog('[test-marker] pak_dump_orphan_inbound_ok')
        && __test_find_inlog('[test-marker] pak_dump_linked_not_orphan_ok')
        && __test_find_inlog('[test-marker] pak_dump_oob_ok')
}
