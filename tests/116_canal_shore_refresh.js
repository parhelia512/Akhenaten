// Marker: [test-marker] test_canal_shore_refresh:...

var __test116_result = null
var __test116_poison = 123456

function test116_fail(reason) {
    __test116_result = { ok: false, reason: reason }
    __test_signal_ready()
}

function test116_clear_tile(tile) {
    terrain.remove(tile, TERRAIN_WATER | TERRAIN_CANAL | TERRAIN_ROAD | TERRAIN_ROCK
        | TERRAIN_TREE | TERRAIN_BUILDING | TERRAIN_SHRUB | TERRAIN_FLOODPLAIN | TERRAIN_RUBBLE)
    __map_image_set(tile, 0)
}

function run_test() {
    __log_info_native('[test:116] canal shore image refresh')
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    var canal = { x: cx, y: cy }
    var water_e = { x: cx + 1, y: cy }

    test116_clear_tile(canal)
    test116_clear_tile(water_e)
    terrain.set_canal(canal)

    if (!terrain.is(canal, TERRAIN_CANAL)) {
        test116_fail('canal_not_placed')
        return
    }

    __map_image_set(canal, __test116_poison)
    if (__map_image_at(canal) !== __test116_poison) {
        test116_fail('poison_not_set')
        return
    }

    terrain.set_water(water_e)
    if (!terrain.is(water_e, TERRAIN_WATER)) {
        test116_fail('water_not_set')
        return
    }

    var img_after = __map_image_at(canal)
    if (img_after === __test116_poison) {
        test116_fail('canal_not_refreshed')
        return
    }
    if (!img_after) {
        test116_fail('canal_image_zero_after')
        return
    }

    __test116_result = { ok: true, after: img_after }
    __log_marker('test_canal_shore_refresh:' + canal.x + ',' + canal.y + ':after=' + img_after)
    __test_signal_ready()
}

function check_valid() {
    if (!__test116_result || !__test116_result.ok) {
        __log_info_native('[test:116] fail: ' + ((__test116_result && __test116_result.reason) || 'no_result'))
        return false
    }

    if (!__test_find_inlog('[test-marker] test_canal_shore_refresh:')) {
        __log_info_native('[test:116] missing log marker')
        return false
    }

    return true
}
