log_info("akhenaten: ui bullfight info window")

[es=building_info_window]
info_window_bullfight {
    related_buildings [BUILDING_BULLFIGHT_SCHOOL]
    ui : baseui(building_info_window, {
    })
}

[es=(info_window_bullfight, init)]
function info_window_bullfight_on_init(window) {
    var b = city.get_building(window.bid)
    if (!b) {
        return
    }

    var gid = b.meta_text_id
    if (!b.has_road_access) {
        window.warning_text.text = __loc({ key: "#building_no_road_access" })
    } else {
        var reason = { group: gid, id: 6 }
        if (b.num_workers <= 0) {
            reason.id = 7
        } else if (b.worker_percentage >= 100) {
            reason.id = 2
        } else if (b.worker_percentage >= 75) {
            reason.id = 3
        } else if (b.worker_percentage >= 50) {
            reason.id = 4
        } else if (b.worker_percentage >= 25) {
            reason.id = 5
        }
        window.warning_text.text = __loc(reason)
    }

    var worker_desc = b.worker_percentage > 0 ? 3 : 2
    window.workers_desc.text = __loc(gid, worker_desc)
}
