// E3d + CC52.N smoke: mission via_sea fleet spawn on Rowarty (36) and Actium (52).
// Markers:
//   [test-marker] rowarty_sea_invasion_ok
//   [test-marker] actium_sea_invasion_ok

function run_test() {
    __log_info_native('[test:139] naval mission via_sea (Rowarty + Actium)')

    // --- E3d: Rowarty loc>=9 via_sea ---
    __game_load_mission(36, 1)
    if (typeof mission36_enemy_raid !== 'function') {
        __log_info_native('[test:139] mission36_enemy_raid missing after load')
        __test_signal_ready()
        return
    }
    mission36_enemy_raid(990, 8, 9, 1)
    var r_transports = __test_count_enemy_transports()
    var r_warships = __test_count_enemy_warships()
    if (r_transports < 1 || r_warships < 1) {
        __log_info_native('[test:139] rowarty fleet incomplete: transports='
            + r_transports + ' warships=' + r_warships)
        __test_signal_ready()
        return
    }
    __log_marker('rowarty_sea_invasion_ok')

    // --- CC52.N: Actium Persian via_sea ---
    __game_load_mission(52, 1)
    if (typeof mission52_enemy_raid !== 'function') {
        __log_info_native('[test:139] mission52_enemy_raid missing after load')
        __test_signal_ready()
        return
    }
    mission52_enemy_raid(991, 8, 0)
    var a_transports = __test_count_enemy_transports()
    var a_warships = __test_count_enemy_warships()
    if (a_transports < 1 || a_warships < 1) {
        __log_info_native('[test:139] actium fleet incomplete: transports='
            + a_transports + ' warships=' + a_warships)
        __test_signal_ready()
        return
    }
    __log_marker('actium_sea_invasion_ok')

    __test_signal_ready()
}

function check_valid() {
    var markers = [
        'rowarty_sea_invasion_ok',
        'actium_sea_invasion_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        var marker = '[test-marker] ' + markers[i]
        if (!__test_find_inlog(marker)) {
            __log_info_native('[test:139] missing marker: ' + marker)
            return false
        }
    }
    return true
}
