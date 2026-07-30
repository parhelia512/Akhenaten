// Sealed flood basin halves canal water decay.
// Marker: [test-marker] test_basin_irrigation_hold:...

var __test107_result = null
var __test107_feat_prev = false

function test107_add_dike_rect(x0, y0, x1, y1) {
    for (var x = x0; x <= x1; x++) {
        terrain.add({ x: x, y: y0 }, TERRAIN_DIKE)
        terrain.add({ x: x, y: y1 }, TERRAIN_DIKE)
    }
    for (var y = y0; y <= y1; y++) {
        terrain.add({ x: x0, y: y }, TERRAIN_DIKE)
        terrain.add({ x: x1, y: y }, TERRAIN_DIKE)
    }
}

function test107_fail(reason) {
    __test107_result = { ok: false, reason: reason }
    game_features.set('gameplay_enhanced_flood_basins', __test107_feat_prev)
    __test_signal_ready()
}

function run_test() {
    __log_info_native('[test:107] flood basin irrigation hold')
    test_reload_city_session('data/default.map')
    __test_set_treasury(10000)

    __test107_feat_prev = game_features.get('gameplay_enhanced_flood_basins') === true
    game_features.set('gameplay_enhanced_flood_basins', true)

    var cx = (__scenario_map.width / 2) | 0
    var cy = (__scenario_map.height / 2) | 0
    test_prepare_terrain_patch(cx, cy, 24, TERRAIN_FLOODPLAIN)

    var x0 = cx - 4
    var y0 = cy - 4
    var x1 = cx + 3
    var y1 = cy + 3
    test107_add_dike_rect(x0, y0, x1, y1)
    __map_basin_rebuild()

    var inside = { x: cx, y: cy }
    var outside = { x: x0 - 3, y: y0 - 3 }
    if (!terrain.basin_sealed(inside)) {
        test107_fail('inside_not_sealed')
        return
    }
    if (terrain.basin_id(outside) !== 0) {
        test107_fail('outside_false_basin')
        return
    }

    // Isolated canals (no river adjacency assumed on synthetic FP patch).
    terrain.add(inside, TERRAIN_CANAL)
    terrain.add(outside, TERRAIN_CANAL)
    __map_canal_set(inside, 10)
    __map_canal_set(outside, 10)

    var passes = 6
    for (var i = 0; i < passes; i++) {
        __map_canals_decrease_water_level()
    }

    var level_in = __map_canal_at(inside)
    var level_out = __map_canal_at(outside)
    // Outside: -1 per pass → 10-6=4. Inside half-rate → ~10-3=7.
    if (level_out !== 10 - passes) {
        test107_fail('outside_decay:' + level_out + '!=' + (10 - passes))
        return
    }
    if (level_in <= level_out) {
        test107_fail('inside_not_held:in=' + level_in + ':out=' + level_out)
        return
    }
    if (level_in < 10 - ((passes + 1) / 2 | 0)) {
        test107_fail('inside_hold_too_weak:in=' + level_in)
        return
    }

    // Farm-facing effect: IRRIGATION_RANGE is cleared each canal update and must be
    // re-stamped from residual wet canals (half-rate alone is invisible to farms).
    __map_canal_set(inside, 3)
    __map_canal_set(outside, 0)
    __map_update_canals()
    // update_canals decreases once more; sealed wet should still stamp irrigation.
    if (__map_canal_at(inside) <= 0) {
        test107_fail('inside_dried_before_stamp')
        return
    }
    if (!terrain.is(inside, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('inside_no_irrigation_stamp')
        return
    }
    if (terrain.is(outside, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('outside_false_irrigation_stamp')
        return
    }

    // Unsealed wet canal (lift/residual path) must also restamp after the clear —
    // sealed-only stamp left lift-fed outside networks dry for farms.
    __map_canal_set(outside, 4)
    __map_canal_set(inside, 0)
    __map_update_canals()
    if (__map_canal_at(outside) <= 0) {
        test107_fail('outside_dried_before_unsealed_stamp')
        return
    }
    if (!terrain.is(outside, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('outside_unsealed_no_irrigation_stamp')
        return
    }
    if (terrain.is(inside, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('inside_false_stamp_from_outside')
        return
    }

    // Radius-2 must not leak irrigation across the dike crest.
    var near_wall = { x: x0 + 1, y: cy }
    var across_wall = { x: x0 - 1, y: cy }
    terrain.add(near_wall, TERRAIN_CANAL)
    __map_canal_set(near_wall, 5)
    __map_canal_set(outside, 0)
    __map_canal_set(inside, 0)
    __map_update_canals()
    if (__map_canal_at(near_wall) <= 0) {
        test107_fail('near_wall_dried_before_stamp')
        return
    }
    if (!terrain.is(near_wall, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('near_wall_no_irrigation_stamp')
        return
    }
    if (terrain.is(across_wall, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('irrigation_leaked_across_dike')
        return
    }

    // Overlay irrigation_value must also respect basin walls (case 5 ring spread).
    __irrigation_value_update()
    if (__irrigation_value_get(near_wall) <= 0) {
        test107_fail('near_wall_no_irrigation_value')
        return
    }
    if (__irrigation_value_get(across_wall) > 0) {
        test107_fail('irrigation_value_leaked_across_dike')
        return
    }

    // Topology change must restamp immediately: flag OFF stamps unclipped across the
    // wall line; flag ON rebuilds basins and must clear the stale outside stamp.
    game_features.set('gameplay_enhanced_flood_basins', false)
    __map_canal_set(near_wall, 5)
    __map_canal_set(across_wall, 0)
    __map_canal_set(inside, 0)
    __map_canal_set(outside, 0)
    __map_update_canals()
    if (!terrain.is(across_wall, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('flag_off_expected_across_stamp')
        return
    }
    game_features.set('gameplay_enhanced_flood_basins', true)
    if (!terrain.basin_sealed(inside)) {
        test107_fail('reseal_failed')
        return
    }
    if (terrain.is(across_wall, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('stale_stamp_after_reseal')
        return
    }
    if (!terrain.is(near_wall, TERRAIN_IRRIGATION_RANGE)) {
        test107_fail('near_wall_lost_stamp_after_reseal')
        return
    }

    // Flag OFF → rebuild clears basin ids and restamps irrigation (no extra decay).
    __map_canal_set(inside, 8)
    __map_canal_set(outside, 8)
    game_features.set('gameplay_enhanced_flood_basins', false)
    if (terrain.basin_id(inside) !== 0) {
        test107_fail('flag_off_basin_id_stale:' + terrain.basin_id(inside))
        return
    }
    for (var j = 0; j < 4; j++) {
        __map_canals_decrease_water_level()
    }
    if (__map_canal_at(inside) !== __map_canal_at(outside)) {
        test107_fail('flag_off_unequal:in=' + __map_canal_at(inside) + ':out=' + __map_canal_at(outside))
        return
    }

    __log_marker('test_basin_irrigation_hold:in=' + level_in + ':out=' + level_out)
    __test107_result = { ok: true, level_in: level_in, level_out: level_out }
    game_features.set('gameplay_enhanced_flood_basins', __test107_feat_prev)
    __log_info_native('[test:107] PASS in=' + level_in + ' > out=' + level_out)
    __test_signal_ready()
}

function check_valid() {
    if (!__test107_result || !__test107_result.ok) {
        __log_info_native('[test:107] FAIL: ' + (__test107_result && __test107_result.reason
            ? __test107_result.reason : 'no_result'))
        return false
    }
    return true
}
