log_info("akhenaten: ui mastaba info window")

[es=building_info_window]
info_window_mastaba {
    related_buildings [
        BUILDING_SMALL_MASTABA, BUILDING_SMALL_MASTABA_SIDE, BUILDING_SMALL_MASTABA_WALL, BUILDING_SMALL_MASTABA_ENTRANCE,
        BUILDING_MEDIUM_MASTABA, BUILDING_MEDIUM_MASTABA_SIDE, BUILDING_MEDIUM_MASTABA_WALL, BUILDING_MEDIUM_MASTABA_ENTRANCE,
        BUILDING_LARGE_MASTABA, BUILDING_LARGE_MASTABA_SIDE, BUILDING_LARGE_MASTABA_WALL, BUILDING_LARGE_MASTABA_ENTRANCE
    ]
    ui {
        background    : outer_panel({size: [29, 18]}),
        title         : text({pos: [0, 16], text:"${building.name}", size: [px(29), 20], font : FONT_LARGE_BLACK_ON_LIGHT, align:"center"}),
        subtitle      : text({pos: [32, 46], text:"${text.12}", size: [px(27), -1], wrap:px(27), font : FONT_NORMAL_BLACK_ON_LIGHT, multiline:true }),
        progress_text : text({pos: [32, 66], size:[px(27), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        warning_text  : text({pos: [32, 96], size:[px(27), -1], wrap:px(27), multiline:true, font : FONT_NORMAL_BLACK_ON_LIGHT }),
        bricks_icon   : resource_icon({pos: [32, 200], resource: RESOURCE_BRICKS }),
        bricks_text   : text({pos: [70, 204], size:[px(15), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        clay_icon     : resource_icon({pos: [32, 230], resource: RESOURCE_CLAY }),
        clay_text     : text({pos: [70, 234], size:[px(15), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        workers_img   : image({pack:PACK_GENERAL, id:134, offset:14, pos:[260, 215] }),
        workers_text  : text({pos: [290, 219], size:[px(10), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        button_help   : help_button({}),
        button_close  : close_button({}),
    }
}

function info_window_mastaba_fill_resource_slot(window, mon, resource, icon_key, text_key) {
    var needed = mon.needs_resource(resource)
    if (needed <= 0) {
        window[text_key].text = ""
        window[icon_key].enabled = false
        return
    }
    window[icon_key].enabled = true
    var pct = mon.resource_pct(resource)
    var delivered = Math.min(Math.floor(needed * pct / 100), needed)
    window[text_key].text = delivered + " / " + needed
}

[es=(info_window_mastaba, init)]
function info_window_mastaba_on_init(window) {
    var mon = city.get_monument(__building_main_id(window.bid))
    if (!mon) {
        return
    }

    if (mon.phase() !== -1) {
        var reason = { group: 0, id: 0 }
        var workers_num = mon.workers_assigned()

        if (mon.phase() < 3) {
            var work_camps_num = city.count_total_buildings(BUILDING_WORK_CAMP)
            var work_camps_active_num = city.count_active_buildings(BUILDING_WORK_CAMP)
            var work_camps_near = city.find_buildings(BUILDING_WORK_CAMP, mon.tile, 10).length

            if (!work_camps_num) { reason = { group: 178, id: 13 } }
            else if (!work_camps_active_num) { reason = { group: 178, id: 15 } }
            else if (workers_num > 0) { reason = { group: 178, id: 39 } }
            else if (work_camps_near < 3) { reason = { group: 178, id: 51 } }
            else { reason = { group: 178, id: 17 } }
        } else {
            var bricklayers_num = city.count_total_buildings(BUILDING_BRICKLAYERS_GUILD)
            var bricklayers_active = city.count_active_buildings(BUILDING_BRICKLAYERS_GUILD)
            var bricks = city.resources.bricks
            var workers_onsite = mon.workers_onsite(FIGURE_LABORER)

            if (bricks.is_stockpiled) { reason = { group: 178, id: 103 } }
            else if (!bricklayers_num) { reason = { group: 178, id: 15 } }
            else if (!bricklayers_active) { reason = { group: 178, id: 19 } }
            else if (!bricks.ready_for_using) { reason = { group: 178, id: 27 } }
            else if (!workers_onsite && workers_num > 0) { reason = { group: 178, id: 114 } }
        }

        window.warning_text.text = __loc(reason)
        window.progress_text.text = mon.phase() + " / " + mon.phases_total() + "    " + mon.material_pct_min() + "%"
        info_window_mastaba_fill_resource_slot(window, mon, RESOURCE_BRICKS, "bricks_icon", "bricks_text")
        info_window_mastaba_fill_resource_slot(window, mon, RESOURCE_CLAY, "clay_icon", "clay_text")
        window.workers_text.text = workers_num + " / " + mon.workers_slots()
    } else {
        window.warning_text.text = __loc(178, 41)
        window.progress_text.text = ""
        window.bricks_text.text = ""
        window.clay_text.text = ""
        window.workers_text.text = ""
        window.bricks_icon.enabled = false
        window.clay_icon.enabled = false
    }
}
