log_info("akhenaten: overlay goods_by_type started")

[es=city_overlay]
overlay_grain {
  id:OVERLAY_GRAIN
  title: "#overlay_grain"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_grain, get_tooltip_for_building)]
function grain_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_GRAIN) : 0
    goods_stock_apply_tooltip(ev, amount, "grain")
}

[es=(overlay_grain, get_column_height)]
function grain_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_GRAIN) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_chickpeas {
  id:OVERLAY_CHICKPEAS
  title: "#overlay_chickpeas"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_chickpeas, get_tooltip_for_building)]
function chickpeas_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_CHICKPEAS) : 0
    goods_stock_apply_tooltip(ev, amount, "chickpeas")
}

[es=(overlay_chickpeas, get_column_height)]
function chickpeas_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_CHICKPEAS) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_pomegranates {
  id:OVERLAY_POMEGRANATES
  title: "#overlay_pomegranates"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_pomegranates, get_tooltip_for_building)]
function pomegranates_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_POMEGRANATES) : 0
    goods_stock_apply_tooltip(ev, amount, "pomegranates")
}

[es=(overlay_pomegranates, get_column_height)]
function pomegranates_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_POMEGRANATES) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_figs {
  id:OVERLAY_FIGS
  title: "#overlay_figs"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_figs, get_tooltip_for_building)]
function figs_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_FIGS) : 0
    goods_stock_apply_tooltip(ev, amount, "figs")
}

[es=(overlay_figs, get_column_height)]
function figs_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_FIGS) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_meat {
  id:OVERLAY_MEAT
  title: "#overlay_meat"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_meat, get_tooltip_for_building)]
function meat_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_MEAT) : 0
    goods_stock_apply_tooltip(ev, amount, "meat")
}

[es=(overlay_meat, get_column_height)]
function meat_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_MEAT) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_game {
  id:OVERLAY_GAME
  title: "#overlay_game"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_game, get_tooltip_for_building)]
function game_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_GAMEMEAT) : 0
    goods_stock_apply_tooltip(ev, amount, "game")
}

[es=(overlay_game, get_column_height)]
function game_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? goods_stock_food_amount(house, RESOURCE_GAMEMEAT) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_pottery {
  id:OVERLAY_POTTERY
  title: "#overlay_pottery"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_pottery, get_tooltip_for_building)]
function pottery_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(0) : 0
    goods_stock_apply_tooltip(ev, amount, "pottery")
}

[es=(overlay_pottery, get_column_height)]
function pottery_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(0) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_jewelry {
  id:OVERLAY_JEWELRY
  title: "#overlay_jewelry"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_jewelry, get_tooltip_for_building)]
function jewelry_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(1) : 0
    goods_stock_apply_tooltip(ev, amount, "jewelry")
}

[es=(overlay_jewelry, get_column_height)]
function jewelry_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(1) : 0
    goods_stock_apply_column(ev, amount)
}

[es=city_overlay]
overlay_linen {
  id:OVERLAY_LINEN
  title: "#overlay_linen"
  walkers:[]
  buildings:[]
  column_type: COLUMN_TYPE_POSITIVE
  column_anim: {pack:PACK_GENERAL, id:103}
}

[es=(overlay_linen, get_tooltip_for_building)]
function linen_building_tooltip(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(2) : 0
    goods_stock_apply_tooltip(ev, amount, "linen")
}

[es=(overlay_linen, get_column_height)]
function linen_building_column_height(ev) {
    var house = city.get_house(ev.bid)
    var amount = house ? house.inv(2) : 0
    goods_stock_apply_column(ev, amount)
}
