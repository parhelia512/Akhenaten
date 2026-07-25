// Stationary ostrich + hunter with lodge home.
// Polls for: hunter enters HUNT → force a shot (anim unreliable under --no-resource)
// → ostrich dies → gamemeat appears on the lodge.
//
// Markers:
//   [test-marker] ostrich_hunt_setup_ok
//   [test-marker] ostrich_hunt_hunting_ok
//   [test-marker] ostrich_hunt_killed_ok
//   [test-marker] ostrich_hunt_meat_ok
//
// Action/state constants come from C++ token_holder binds
// (e_ostrich_hunter_action, e_ostrich_action, e_figure_state, e_common_action).

var __test49_ok = false

// TEMP debug — remove after hunt test is green
function test49_dbg(msg) {
    __log_info_native('[test:49][dbg] ' + msg)
}

function test49_action_name(a) {
    if (a == ACTION_8_RECALCULATE) return 'RECALC'
    if (a == ACTION_9_OSTRICH_HUNTER_CHASE_PREY) return 'CHASE'
    if (a == ACTION_10_OSTRICH_HUNTER_PICKUP_ANIMAL) return 'PICKUP'
    if (a == ACTION_11_OSTRICH_HUNTER_GOING_TO_PICKUP_POINT) return 'GO_PICKUP'
    if (a == ACTION_12_OSTRICH_HUNTER_MOVE_PACKED) return 'MOVE_PACKED'
    if (a == ACTION_13_OSTRICH_HUNTER_WAIT_FOR_ACTION) return 'WAIT'
    if (a == ACTION_14_OSTRICH_HUNTER_UNLOADING) return 'UNLOAD'
    if (a == ACTION_15_OSTRICH_HUNTER_HUNT) return 'HUNT'
    if (a == ACTION_16_OSTRICH_HUNTER_INVESTIGATE) return 'INVESTIGATE'
    if (a == ACTION_12_OSTRICH_HUNTER_MOVE_RANDOM_PACKED) return 'MOVE_RANDOM_PACKED'
    if (a == ACTION_12_OSTRICH_HUNTER_LOOK_RANDOM_PACKED) return 'LOOK_RANDOM_PACKED'
    return '?' + a
}

function test49_state_name(s) {
    if (s == FIGURE_STATE_ALIVE) return 'ALIVE'
    if (s == FIGURE_STATE_DEAD) return 'DEAD'
    if (s == FIGURE_STATE_DYING) return 'DYING'
    return '?' + s
}

function test49_snapshot(tag, bid, hunter, ostrich, shots, meat0) {
    var ht = __figure_is_valid(hunter) ? __figure_get_tile(hunter) : null
    var ot = __figure_is_valid(ostrich) ? __figure_get_tile(ostrich) : null
    var ha = __figure_is_valid(hunter) ? __figure_get_action_state(hunter) : -1
    var oa = __figure_is_valid(ostrich) ? __figure_get_action_state(ostrich) : -1
    var hs = __figure_is_valid(hunter) ? __figure_get_state(hunter) : -1
    var os = __figure_is_valid(ostrich) ? __figure_get_state(ostrich) : -1
    var dmg = test_figure_get_damage(ostrich)
    var meat = city.get_building(bid).stored_resource(RESOURCE_GAMEMEAT)
    var arrows = __test_count_figures(FIGURE_HUNTER_ARROW)
    var hank = __figure_is_valid(hunter) ? __figure_get_anim_key(hunter) : '-'
    var oank = __figure_is_valid(ostrich) ? __figure_get_anim_key(ostrich) : '-'
    test49_dbg(tag
        + ' hunter=' + hunter + '@' + (ht ? (ht.x + ',' + ht.y) : 'gone')
        + ' act=' + test49_action_name(ha) + '(' + ha + ')'
        + ' st=' + test49_state_name(hs)
        + ' anim=' + hank
        + ' | ostrich=' + ostrich + '@' + (ot ? (ot.x + ',' + ot.y) : 'gone')
        + ' act=' + oa
        + ' st=' + test49_state_name(os)
        + ' dmg=' + dmg
        + ' anim=' + oank
        + ' | meat=' + meat + '(+' + (meat - meat0) + ')'
        + ' shots=' + shots
        + ' arrows=' + arrows)
}

function run_test() {
    __log_info_native('[test:49] ostrich hunt (stationary prey, poll meat)')
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)

    var bid = test_building_place(BUILDING_HUNTING_LODGE, -1, -1)
    if (!bid) {
        __log_info_native('[test:49] FAIL: lodge place failed')
        __test_signal_ready()
        return
    }

    var tile = __building_tile(bid)
    if (!tile) {
        __log_info_native('[test:49] FAIL: lodge tile missing')
        __test_signal_ready()
        return
    }

    // Prey next to lodge; hunter within attack_distance (config: 5).
    var ostrich = test_figure_create(FIGURE_OSTRICH, tile.x + 2, tile.y)
    var hunter = test_figure_create(FIGURE_OSTRICH_HUNTER, tile.x + 4, tile.y)
    if (!ostrich || !hunter) {
        __log_info_native('[test:49] FAIL: spawn ostrich/hunter failed')
        __test_signal_ready()
        return
    }

    test_figure_set_home(hunter, bid)
    test_figure_set_speed(ostrich, 0)
    // Under --no-resource hunt/pack/unload need animctx to tick without atlas.
    test_figure_set_force_valid_animation(hunter, true)
    test_figure_set_force_valid_animation(ostrich, true)
    __test_figure_set_action(ostrich, ACTION_19_OSTRICH_IDLE)
    __test_figure_set_action(hunter, ACTION_8_RECALCULATE)

    var meat0 = city.get_building(bid).stored_resource(RESOURCE_GAMEMEAT)
    test49_dbg('setup lodge=' + bid + '@' + tile.x + ',' + tile.y
        + ' ostrich=' + ostrich + ' hunter=' + hunter + ' meat0=' + meat0)
    test49_snapshot('after_setup', bid, hunter, ostrich, 0, meat0)
    __log_marker('ostrich_hunt_setup_ok')

    var hunting = false
    var killed = false
    var meat_ok = false
    var shots = 0
    var last_hunter_act = -1
    var last_ostrich_st = -1

    for (var i = 0; i < 80; i++) {
        test_figure_set_speed(ostrich, 0)
        __test_pump_frames(30)

        var ha = __figure_is_valid(hunter) ? __figure_get_action_state(hunter) : -1
        var os = __figure_is_valid(ostrich) ? __figure_get_state(ostrich) : -1

        // TEMP: log every tick + on action/state change
        if (ha != last_hunter_act || os != last_ostrich_st || (i % 5) == 0) {
            test49_snapshot('i=' + i, bid, hunter, ostrich, shots, meat0)
            last_hunter_act = ha
            last_ostrich_st = os
        }

        if (!hunting && ha == ACTION_15_OSTRICH_HUNTER_HUNT) {
            __log_marker('ostrich_hunt_hunting_ok')
            hunting = true
            test49_dbg('ENTERED HUNT at i=' + i)
        }

        // Under --no-resource the hunt anim may never finish, so force shots
        // once the hunter has acquired the prey and entered HUNT.
        if (hunting && !killed && shots < 4) {
            var mid = __test_hunter_force_shot(hunter)
            test49_dbg('force_shot i=' + i + ' mid=' + mid
                + ' arrows_now=' + __test_count_figures(FIGURE_HUNTER_ARROW))
            if (mid) {
                shots++
            }
            __test_pump_frames(20)
            test49_snapshot('after_shot', bid, hunter, ostrich, shots, meat0)
        }

        if (!killed) {
            var dead = !__figure_is_valid(ostrich)
                || __figure_get_state(ostrich) == FIGURE_STATE_DYING
                || test_figure_get_damage(ostrich) >= 100
            if (dead) {
                __log_marker('ostrich_hunt_killed_ok')
                killed = true
                test49_dbg('OSTRICH KILLED at i=' + i
                    + ' dmg=' + test_figure_get_damage(ostrich)
                    + ' st=' + test49_state_name(__figure_is_valid(ostrich) ? __figure_get_state(ostrich) : -1))
            }
        }

        var meat = city.get_building(bid).stored_resource(RESOURCE_GAMEMEAT)
        if (meat >= meat0 + 100) {
            __log_marker('ostrich_hunt_meat_ok')
            meat_ok = true
            test49_dbg('MEAT DELIVERED at i=' + i + ' meat=' + meat)
            break
        }
    }

    test49_snapshot('loop_end', bid, hunter, ostrich, shots, meat0)

    if (!hunting) {
        __log_info_native('[test:49] FAIL: hunter never entered HUNT'
            + ' action=' + __figure_get_action_state(hunter))
        __test_signal_ready()
        return
    }
    if (!killed) {
        __log_info_native('[test:49] FAIL: ostrich not killed'
            + ' state=' + __figure_get_state(ostrich)
            + ' dmg=' + test_figure_get_damage(ostrich)
            + ' shots=' + shots
            + ' arrows=' + __test_count_figures(FIGURE_HUNTER_ARROW))
        __test_signal_ready()
        return
    }
    if (!meat_ok) {
        __log_info_native('[test:49] FAIL: no gamemeat on lodge'
            + ' stored=' + city.get_building(bid).stored_resource(RESOURCE_GAMEMEAT)
            + ' hunter_action=' + (__figure_is_valid(hunter)
                ? test49_action_name(__figure_get_action_state(hunter))
                : 'gone')
            + ' hunter_anim=' + (__figure_is_valid(hunter) ? __figure_get_anim_key(hunter) : '-'))
        __test_signal_ready()
        return
    }

    __test49_ok = true
    __test_signal_ready()
}

function check_valid() {
    if (!__test49_ok) {
        __log_info_native('[test:49] run_test did not complete')
        return false
    }

    var markers = [
        '[test-marker] ostrich_hunt_setup_ok',
        '[test-marker] ostrich_hunt_hunting_ok',
        '[test-marker] ostrich_hunt_killed_ok',
        '[test-marker] ostrich_hunt_meat_ok'
    ]
    for (var i = 0; i < markers.length; i++) {
        if (!__test_find_inlog(markers[i])) {
            __log_info_native('[test:49] missing marker: ' + markers[i])
            return false
        }
    }
    return true
}
