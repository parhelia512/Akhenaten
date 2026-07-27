// WR1 / WR4: fixed worker ratio — % of plebs via gameplay_fixed_worker_percent.
// Markers:
//   [test-marker] fixed_workers_calc_ok
//   [test-marker] fixed_workers_on_pool_ok
//   [test-marker] fixed_workers_advisor_pct_ok
//   [test-marker] fixed_workers_percent_feature_ok

function run_test() {
    __log_info_native('[test:67] fixed workers ratio')
    test_ensure_city_session('data/default.map')

    game_features.set('gameplay_change_fixed_workers', true)
    game_features.set('gameplay_fixed_worker_percent', 38)

    var calc = city.labor.calc_fixed_workers_available(1000)
    if (calc != 380) {
        __log_info_native('[test:67] calc want 380 got ' + calc)
        __test_signal_ready()
        return
    }
    __log_marker('fixed_workers_calc_ok')

    city.labor.calculate_workers(1000, 250)
    var avail = city.labor.workers_available
    if (avail != 380) {
        __log_info_native('[test:67] pool want 380 got ' + avail)
        __test_signal_ready()
        return
    }
    __log_marker('fixed_workers_on_pool_ok')

    // Advisor must use workers_available / total — never hardcode 38.
    if (typeof advisor_population_percent_in_workforce_value !== 'function') {
        __log_info_native('[test:67] advisor helper missing')
        __test_signal_ready()
        return
    }
    var cur = city.population_stats.current
    if (!cur) {
        var pct_empty = advisor_population_percent_in_workforce_value()
        if (pct_empty != 0) {
            __log_info_native('[test:67] advisor empty want 0 got ' + pct_empty)
            __test_signal_ready()
            return
        }
    } else {
        var pct = advisor_population_percent_in_workforce_value()
        var expect = (100 * avail / cur) | 0
        if (pct != expect) {
            __log_info_native('[test:67] advisor want ' + expect + ' got ' + pct
                + ' (avail=' + avail + ' cur=' + cur + ')')
            __test_signal_ready()
            return
        }
    }
    __log_marker('fixed_workers_advisor_pct_ok')

    game_features.set('gameplay_fixed_worker_percent', 40)
    calc = city.labor.calc_fixed_workers_available(1000)
    city.labor.calculate_workers(1000, 0)
    if (calc != 400 || city.labor.workers_available != 400) {
        __log_info_native('[test:67] percent feature want 400 got calc='
            + calc + ' avail=' + city.labor.workers_available)
        game_features.set('gameplay_fixed_worker_percent', 38)
        game_features.set('gameplay_change_fixed_workers', false)
        __test_signal_ready()
        return
    }
    __log_marker('fixed_workers_percent_feature_ok')

    game_features.set('gameplay_fixed_worker_percent', 38)
    game_features.set('gameplay_change_fixed_workers', false)
    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'fixed_workers_calc_ok',
        'fixed_workers_on_pool_ok',
        'fixed_workers_advisor_pct_ok',
        'fixed_workers_percent_feature_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:67] missing marker: ' + marker)
            return false
        }
    }
    return true
}

run_test()
