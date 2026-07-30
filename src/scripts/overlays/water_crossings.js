log_info("akhenaten: overlay water_crossings started")

[es=city_overlay]
overlay_water_crossings {
  id:OVERLAY_WATER_CROSSINGS
  title: "#overlay_water_crossings"
  walkers:[FIGURE_FERRY_BOAT]
  // Bridges are terrain (map_bridge_*), not BUILDING_LOW_BRIDGE instances.
  buildings:[BUILDING_FERRY, BUILDING_ROADBLOCK]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_water_crossings, get_column_height)]
function water_crossings_building_column_height(ev) {
    city.overlay_column_height = -1
}

[es=(overlay_water_crossings, show_building)]
function water_crossings_show_building(ev) {
    var building = city.get_building(ev.bid)
    if (!building || !building.valid) {
        city.overlay_show_building = 0
        return
    }

    city.overlay_show_building = (
        building.type == BUILDING_FERRY
        || building.type == BUILDING_ROADBLOCK
    ) ? 1 : 0
}

[es=(overlay_water_crossings, get_tooltip_for_building)]
function water_crossings_building_tooltip(ev) {
    var building = city.get_building(ev.bid)
    if (!building || !building.valid) {
        return
    }

    if (building.type == BUILDING_FERRY) {
        city.overlay_tooltip = "#overlay_water_crossings_ferry"
    }
}

[es=(overlay_water_crossings, get_tooltip)]
function water_crossings_tile_tooltip(ev) {
    // Low/ship bridge spans live on water + bridge sprite grid, not as buildings.
    if (__map_is_bridge(ev.tile)) {
        city.overlay_tooltip = "#overlay_water_crossings_bridge"
    }
}
