log_info("akhenaten: overlay irrigation started")

[es=(overlay_irrigation, get_tooltip)]
function irrigation_tile_tooltip(ev) {
    // Skip non-irrigable terrain (water/rock/trees/cliffs/roads) — "none" there is noise.
    var skip = TERRAIN_TREE | TERRAIN_ROCK | TERRAIN_WATER | TERRAIN_ROAD | TERRAIN_ELEVATION
        | TERRAIN_ACCESS_RAMP | TERRAIN_RUBBLE | TERRAIN_WALL
    if (terrain.is(ev.tile, skip) && !terrain.is(ev.tile, TERRAIN_BUILDING)) {
        return
    }

    var value = __irrigation_value_at_tile(ev.tile.x, ev.tile.y)
    if (value <= 0) {
        city.overlay_tooltip = "#irrigation_none"
    } else if (value < 30) {
        city.overlay_tooltip = "#irrigation_low"
    } else if (value < 60) {
        city.overlay_tooltip = "#irrigation_medium"
    } else {
        city.overlay_tooltip = "#irrigation_high"
    }
}
