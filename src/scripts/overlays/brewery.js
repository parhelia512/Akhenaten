log_info("akhenaten: overlay brewery started")

[es=city_overlay]
overlay_brewery {
  id:OVERLAY_BREWERY
  title: "#overlay_beer"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_brewery, get_tooltip_for_building)]
function brewery_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(3) : 0
    goods_stock_apply_tooltip(ev, amount, "beer")
}

[es=(overlay_brewery, get_column_height)]
function brewery_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(3) : 0
    goods_stock_apply_column(ev, amount)
}
