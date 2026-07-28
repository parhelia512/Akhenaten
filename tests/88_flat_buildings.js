// Flat buildings view (FM*): session flag, flatten predicate, raise set, feature sync.
// Markers:
//   [test-marker] flat_view_on_ok
//   [test-marker] flat_should_flatten_ok
//   [test-marker] flat_raise_ok
//   [test-marker] flat_unraise_ok
//   [test-marker] flat_view_off_clears_raise_ok

function run_test() {
    __log_info_native('[test:88] flat buildings view')
    test_ensure_city_session('data/default.map')

    __test_set_treasury(500000)
    if (!__scenario_building_allowed(BUILDING_VILLAGE_PALACE)) {
        __scenario_building_allow(BUILDING_VILLAGE_PALACE, true)
    }

    var bid = __test_building_create(BUILDING_VILLAGE_PALACE, -1, -1)
    if (!bid) {
        __log_info_native('[test:88] palace create failed')
        __test_signal_ready()
        return
    }

    __city_flat_buildings_set(0)
    if (__city_flat_should_flatten(bid)) {
        __log_info_native('[test:88] should not flatten when view Off')
        __test_signal_ready()
        return
    }

    __city_flat_buildings_set(1)
    if (!__city_flat_buildings_active()) {
        __log_info_native('[test:88] view On failed (feature off?)')
        __test_signal_ready()
        return
    }
    __log_marker('flat_view_on_ok')

    if (!__city_flat_should_flatten(bid)) {
        __log_info_native('[test:88] expected flatten for palace while On')
        __test_signal_ready()
        return
    }
    __log_marker('flat_should_flatten_ok')

    __city_flat_toggle_raised(bid)
    if (!__city_flat_is_raised(bid) || __city_flat_should_flatten(bid)) {
        __log_info_native('[test:88] raise should exclude from flatten')
        __test_signal_ready()
        return
    }
    __log_marker('flat_raise_ok')

    __city_flat_toggle_raised(bid)
    if (__city_flat_is_raised(bid) || !__city_flat_should_flatten(bid)) {
        __log_info_native('[test:88] unraise should flatten again')
        __test_signal_ready()
        return
    }
    __log_marker('flat_unraise_ok')

    __city_flat_toggle_raised(bid)
    __city_flat_buildings_set(0)
    if (__city_flat_is_raised(bid) || __city_flat_buildings_active()) {
        __log_info_native('[test:88] Off should clear raise + view')
        __test_signal_ready()
        return
    }
    __log_marker('flat_view_off_clears_raise_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'flat_view_on_ok',
        'flat_should_flatten_ok',
        'flat_raise_ok',
        'flat_unraise_ok',
        'flat_view_off_clears_raise_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:88] missing marker: ' + marker)
            return false
        }
    }
    return true
}
