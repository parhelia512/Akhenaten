log_info("akhenaten: overlay empty_housing started")

[es=city_overlay]
overlay_empty_housing {
  id:OVERLAY_EMPTY_HOUSING
  title: "#overlay_empty_housing"
  walkers:[]
  buildings:[BUILDING_ROADBLOCK]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_empty_housing, get_tooltip_for_building)]
function empty_housing_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    if (!house) {
        return
    }

    if (house.is_vacant_lot || house.population <= 0) {
        city.overlay_tooltip = "#empty_housing_vacant"
    }
}

[es=(overlay_empty_housing, get_column_height)]
function empty_housing_building_column_height(ev) {
    // Empty houses are highlighted via show_building (not columns).
    city.overlay_column_height = -1
}

[es=(overlay_empty_housing, show_building)]
function empty_housing_show_building(ev) {
    var building = city.get_building(ev.bid)
    if (!building || !building.valid) {
        city.overlay_show_building = 0
        return
    }

    if (building.type == BUILDING_ROADBLOCK) {
        city.overlay_show_building = 1
        return
    }

    var house = city.get_house(ev.bid)
    if (!house) {
        city.overlay_show_building = 0
        return
    }

    city.overlay_show_building = (house.is_vacant_lot || house.population <= 0) ? 1 : 0
}
