log_info("akhenaten: city delivery_paths.js started")

function building_draw_usable_paths(ev) {
    if (!ev || !ev.bid) {
        return
    }
    __building_draw_usable_paths(ev.bid)
}

[es=(building_granary, draw_usable_paths)]
function building_granary_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_hunting_lodge, draw_usable_paths)]
function building_hunting_lodge_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_food_mill, draw_usable_paths)]
function building_food_mill_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_village_palace, draw_usable_paths)]
function building_village_palace_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_town_palace, draw_usable_paths)]
function building_town_palace_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_city_palace, draw_usable_paths)]
function building_city_palace_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_bazaar, draw_usable_paths)]
function building_bazaar_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_mine_gold, draw_usable_paths)]
function building_mine_gold_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_mine_copper, draw_usable_paths)]
function building_mine_copper_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_mine_gems, draw_usable_paths)]
function building_mine_gems_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_architect_post, draw_usable_paths)]
function building_architect_post_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}

[es=(building_water_supply, draw_usable_paths)]
function building_water_supply_draw_usable_paths(ev) {
    building_draw_usable_paths(ev)
}
