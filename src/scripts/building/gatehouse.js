log_info("akhenaten: building_gatehouse started")

var building_gatehouse_restricted_terrain = TERRAIN_NOT_CLEAR - TERRAIN_ROAD

function building_gatehouse_shift(tile, dx, dy) {
    return { x: tile.x + dx, y: tile.y + dy }
}

function building_gatehouse_tile_blocked(tile) {
    return __city_planner_is_blocked_for_building(tile, 1, building_gatehouse_restricted_terrain)
}

function building_gatehouse_second_part_tile(end, city_orientation) {
    var local_rotation = -1
    var tile_second_part = { x: 0, y: 0 }
    var possible_next
    var possible_next_w

    if (city_orientation == 0) {
        possible_next = building_gatehouse_shift(end, 0, -1)
        if (!building_gatehouse_tile_blocked(possible_next)) {
            local_rotation = 0
            tile_second_part = possible_next
        }
        possible_next_w = building_gatehouse_shift(end, 1, 0)
        if (!building_gatehouse_tile_blocked(possible_next_w)) {
            local_rotation = 1
            tile_second_part = possible_next_w
        }
    } else if (city_orientation == 1) {
        possible_next = building_gatehouse_shift(end, 0, -1)
        if (!building_gatehouse_tile_blocked(possible_next)) {
            local_rotation = 1
            tile_second_part = possible_next
        }
        possible_next_w = building_gatehouse_shift(end, 1, 0)
        if (!building_gatehouse_tile_blocked(possible_next_w)) {
            local_rotation = 2
            tile_second_part = possible_next_w
        }
    } else if (city_orientation == 2) {
        possible_next = building_gatehouse_shift(end, 0, 1)
        if (!building_gatehouse_tile_blocked(possible_next)) {
            local_rotation = 2
            tile_second_part = possible_next
        }
        possible_next_w = building_gatehouse_shift(end, 1, 0)
        if (!building_gatehouse_tile_blocked(possible_next_w)) {
            local_rotation = 3
            tile_second_part = possible_next_w
        }
    } else if (city_orientation == 3) {
        possible_next = building_gatehouse_shift(end, 0, 1)
        if (!building_gatehouse_tile_blocked(possible_next)) {
            local_rotation = 3
            tile_second_part = possible_next
        }
        possible_next_w = building_gatehouse_shift(end, -1, 0)
        if (!building_gatehouse_tile_blocked(possible_next_w)) {
            local_rotation = 0
            tile_second_part = possible_next_w
        }
    }

    return { tile: tile_second_part, orientation: local_rotation }
}

function building_gatehouse_placement_check(ev) {
    var fully_blocked = !!city.finance.is_out_of_money
    var blocked = fully_blocked
    var end = ev.end
    var city_orientation = Math.floor(__camera.orientation / 2)
    var back_tile = building_gatehouse_second_part_tile(end, city_orientation)
    var blocked_tiles_main = []
    var blocked_tiles_second = []

    if (back_tile.orientation >= 0) {
        var main_blocked = building_gatehouse_tile_blocked(end)
        var second_blocked = building_gatehouse_tile_blocked(back_tile.tile)
        blocked_tiles_main.push({ tile: end, blocked: main_blocked })
        blocked_tiles_second.push({ tile: back_tile.tile, blocked: second_blocked })
        blocked = blocked || main_blocked || second_blocked
    } else {
        blocked = true
        blocked_tiles_main.push({ tile: end, blocked: true })
        blocked_tiles_main.push({ tile: building_gatehouse_shift(end, 0, -1), blocked: true })
    }

    return {
        blocked: blocked,
        fully_blocked: fully_blocked,
        city_orientation: city_orientation,
        back_tile: back_tile,
        blocked_tiles_main: blocked_tiles_main,
        blocked_tiles_second: blocked_tiles_second
    }
}

function building_gatehouse_draw_partially_blocked(tiles, fully_blocked) {
    for (var i = 0; i < tiles.length; i++) {
        var entry = tiles[i]
        var pixel = city_planner.tile_to_pixel(entry.tile)
        var color = (fully_blocked || entry.blocked) ? COLOR_MASK_RED_30 : COLOR_MASK_GREEN_30
        city_planner.draw_flat_tile(pixel, color)
    }
}

function building_gatehouse_draw_blocked(check) {
    building_gatehouse_draw_partially_blocked(check.blocked_tiles_main, check.fully_blocked)
    building_gatehouse_draw_partially_blocked(check.blocked_tiles_second, check.fully_blocked)
}

function building_gatehouse_ghost_blocked(ev) {
    building_gatehouse_draw_blocked(building_gatehouse_placement_check(ev))
}

function building_gatehouse_ghost_preview(ev) {
    var check = building_gatehouse_placement_check(ev)
    if (check.blocked) {
        building_gatehouse_draw_blocked(check)
        return
    }

    var params = city.get_building_params_by_type(city_planner.build_type)
    var cfg = get_building_config_by_id(city_planner.build_type)
    var ghost = cfg.ghost
    var back_tile = check.back_tile
    var final_rot = (check.city_orientation + back_tile.orientation) % 8
    var main_off = ghost.main_view_offset[final_rot] || [0, 0]
    var part_off = ghost.part_view_offset[final_rot] || [0, 0]
    var pixel = ev.pixel
    var main_pixel = { x: pixel.x + main_off[0], y: pixel.y + main_off[1] }
    var ground_pixel = { x: pixel.x + part_off[0], y: pixel.y + part_off[1] }

    if (back_tile.orientation == 0 || back_tile.orientation == 2) {
        city_planner.draw_ghost(ground_pixel, params.first_img("base_second_n"))
        city_planner.draw_ghost(main_pixel, params.first_img("base_n"))
    } else {
        city_planner.draw_ghost(main_pixel, params.first_img("base_second_w"))
        city_planner.draw_ghost(ground_pixel, params.first_img("base_w"))
    }
}

[es=(building_brick_gatehouse, ghost_preview)]
function building_brick_gatehouse_ghost_preview(ev) {
    building_gatehouse_ghost_preview(ev)
}

[es=(building_brick_gatehouse, ghost_blocked)]
function building_brick_gatehouse_ghost_blocked(ev) {
    building_gatehouse_ghost_blocked(ev)
}

[es=(building_mud_gatehouse, ghost_preview)]
function building_mud_gatehouse_ghost_preview(ev) {
    building_gatehouse_ghost_preview(ev)
}

[es=(building_mud_gatehouse, ghost_blocked)]
function building_mud_gatehouse_ghost_blocked(ev) {
    building_gatehouse_ghost_blocked(ev)
}
