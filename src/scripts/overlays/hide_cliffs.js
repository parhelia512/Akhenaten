log_info("akhenaten: overlay hide_cliffs started")

[es=(overlay_hide_cliffs, get_tooltip)]
function hide_cliffs_tile_tooltip(ev) {
    // Hint only on flattened cliff / ramp tiles — not every empty land tile.
    if (terrain.is(ev.tile, TERRAIN_ELEVATION | TERRAIN_ACCESS_RAMP)
        && !terrain.is(ev.tile, TERRAIN_BUILDING)) {
        city.overlay_tooltip = "#overlay_hide_cliffs_hint"
    }
}
