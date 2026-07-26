log_info("akhenaten: overlay zoo started")

[es=city_overlay]
overlay_zoo {
  id:OVERLAY_ZOO
  title: "#overlay_zoo"
  walkers:[FIGURE_ZOOKEEPER]
  buildings:[BUILDING_ZOO, BUILDING_ROADBLOCK]
  column_type: COLUMN_TYPE_WATER_ACCESS
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_zoo, get_tooltip_for_building)]
function zoo_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    if (!house) {
        city.overlay_tooltip = "#zoo_access_low"
        return
    }

    var zoo = house.zookeeper
    if (zoo <= 0) {
        city.overlay_tooltip = "#zoo_access_none"
    } else if (zoo >= 80) {
        city.overlay_tooltip = "#zoo_access_high"
    } else if (zoo >= 20) {
        city.overlay_tooltip = "#zoo_access_medium"
    } else {
        city.overlay_tooltip = "#zoo_access_low"
    }
}

[es=(overlay_zoo, get_column_height)]
function zoo_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    if (!house) {
        city.overlay_column_height = -1
        return
    }

    if (house.population <= 0) {
        city.overlay_column_height = -1
        return
    }

    city.overlay_column_height = house.zookeeper / 10
}
