log_info("akhenaten: overlay flood basin started")

[es=(overlay_flood_basin, get_tooltip)]
function flood_basin_tile_tooltip(ev) {
    if (game_features.get('gameplay_enhanced_flood_basins') !== true) {
        city.overlay_tooltip = "#overlay_flood_basin_off"
        return
    }

    var t = ev.tile
    if (terrain.is(t, TERRAIN_DIKE)) {
        var adj = terrain.basin_adjacent_id(t)
        if (adj !== 0) {
            city.overlay_tooltip = "#terrain_dike_sealed"
        } else {
            city.overlay_tooltip = "#terrain_dike_breached"
        }
        return
    }

    if (terrain.basin_sealed(t)) {
        city.overlay_tooltip = "#terrain_dike_sealed"
    } else if (terrain.is(t, TERRAIN_FLOODPLAIN)) {
        city.overlay_tooltip = "#overlay_flood_basin_open"
    } else {
        city.overlay_tooltip = "#overlay_flood_basin_none"
    }
}
