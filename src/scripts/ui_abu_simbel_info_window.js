log_info("akhenaten: abu simbel info window started")

abu_simbel_info_layout {
    ui {
        background    : outer_panel({size: [29, 16]}),
        title         : text({pos: [0, 16], text:"${building.name}", size: [px(29), 20], font : FONT_LARGE_BLACK_ON_LIGHT, align:"center"}),
        subtitle      : text({pos: [32, 46], text:"${text.12}", size: [px(27), -1], wrap:px(27), font : FONT_NORMAL_BLACK_ON_LIGHT, multiline:true }),
        progress_text : text({pos: [32, 66], size:[px(20), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        progress_pct  : text({pos: [200, 66], size:[px(8), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        warning_text  : text({pos: [32, 96], size:[px(27), -1], wrap:px(27), multiline:true, font : FONT_NORMAL_BLACK_ON_LIGHT }),
        timber_icon   : resource_icon({pos: [32, 200], resource: RESOURCE_TIMBER }),
        timber_text   : text({pos: [70, 204], size:[px(15), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        button_help   : help_button({}),
        button_close  : close_button({}),
    }
}

[es=building_info_window]
info_window_abu_simbel {
    related_buildings [BUILDING_ABU_SIMBEL]
    ui : baseui(abu_simbel_info_layout, {
    })
}

function info_window_abu_simbel_fill_resource_slot(window, mon, resource, icon_key, text_key) {
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

function info_window_abu_simbel_progress_pct(mon) {
    var min_pct = 100
    var any_resource = false
    var needed = mon.needs_resource(RESOURCE_TIMBER)
    if (needed > 0) {
        any_resource = true
        min_pct = mon.resource_pct(RESOURCE_TIMBER)
    }
    if (!any_resource) {
        var art_phases = Math.max(1, mon.phases_total() - 1)
        min_pct = Math.min(99, Math.floor(mon.phase() * 100 / art_phases))
    }
    return min_pct
}

[es=(info_window_abu_simbel, init)]
function info_window_abu_simbel_on_init(window) {
    var mon = city.get_monument(window.bid)
    if (!mon) {
        return
    }

    if (mon.phase() !== -1) {
        var reason = { group: 178, id: 141 }
        var carpenters = city.count_active_buildings(BUILDING_CARPENTERS_GUILD)
        var stonemasons = city.count_active_buildings(BUILDING_STONEMASONS_GUILD)

        if (mon.phase() === 0 && carpenters === 0 && stonemasons === 0) {
            reason = { group: 199, id: 71 }
        } else if (mon.needs_resource(RESOURCE_TIMBER) > 0) {
            reason = carpenters ? { group: 178, id: 141 } : { group: 178, id: 16 }
        } else if (mon.need_stonemason()) {
            reason = stonemasons ? { group: 178, id: 141 } : { group: 178, id: 14 }
        }

        window.warning_text.text = __loc(reason)
        window.progress_text.text = __loc(199, 73)
        window.progress_pct.text = info_window_abu_simbel_progress_pct(mon) + "%"
        info_window_abu_simbel_fill_resource_slot(window, mon, RESOURCE_TIMBER, "timber_icon", "timber_text")
    } else {
        window.warning_text.text = __loc(178, 142)
        window.progress_text.text = __loc(199, 74)
        window.progress_pct.text = ""
        window.timber_text.text = ""
        window.timber_icon.enabled = false
    }
}
