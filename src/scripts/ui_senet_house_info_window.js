log_info("akhenaten: ui senet house info window")

[es=building_info_window]
info_window_senet_house {
    related_buildings [BUILDING_SENET_HOUSE]
    ui : baseui(building_info_window, {
        advice : text({pos: [36, 164], wrap:400, font : FONT_NORMAL_BLACK_ON_DARK, multiline:true }),
    })
}

[es=(info_window_senet_house, init)]
function info_window_senet_house_on_init(window) {
    var b = city.get_entertainment_building(window.bid)
    if (!b) {
        return
    }
    var gid = b.meta_text_id
    var reason = { group: gid, id: 0 }
    if (!b.has_road_access) {
        reason = { key: "#building_no_road_access" }
    } else if (b.num_workers <= 0) {
        reason.id = 4
    } else if (!b.num_shows) {
        reason.id = 2
    } else if (b.juggler_visited) {
        reason.id = 3
    }
    window.warning_text.text = __loc(reason)

    var worker_desc = b.worker_percentage > 0 ? 3 : 2
    window.workers_desc.text = __loc(gid, worker_desc)

    if (b.juggler_visited > 0) {
        window.advice.text = __loc(gid, 6) + " " + __loc(8, 44) + " " + (2 * b.juggler_visited)
    } else {
        window.advice.text = __loc(gid, 5)
    }
}
