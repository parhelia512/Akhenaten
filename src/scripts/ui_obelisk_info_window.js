log_info("akhenaten: ui obelisk info window")

[es=building_info_window]
info_window_obelisk {
    related_buildings [BUILDING_SMALL_OBELISK, BUILDING_LARGE_OBELISK]
    ui {
        background    : outer_panel({size: [29, 16]}),
        title         : text({pos: [0, 16], text:"${building.name}", size: [px(29), 20], font : FONT_LARGE_BLACK_ON_LIGHT, align:"center"}),
        subtitle      : text({pos: [32, 46], text:"${text.12}", size: [px(27), -1], wrap:px(27), font : FONT_NORMAL_BLACK_ON_LIGHT, multiline:true }),
        progress_text : text({pos: [32, 66], size:[px(27), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        warning_text  : text({pos: [32, 96], size:[px(27), -1], wrap:px(27), multiline:true, font : FONT_NORMAL_BLACK_ON_LIGHT }),
        timber_icon   : resource_icon({pos: [32, 200], resource: RESOURCE_TIMBER }),
        timber_text   : text({pos: [70, 204], size:[px(15), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        granite_icon  : resource_icon({pos: [180, 200], resource: RESOURCE_GRANITE }),
        granite_text  : text({pos: [218, 204], size:[px(15), 20], font : FONT_NORMAL_BLACK_ON_LIGHT }),
        button_help   : help_button({}),
        button_close  : close_button({}),
    }
}

function info_window_obelisk_fill_resource_slot(window, mon, resource, icon_key, text_key) {
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

[es=(info_window_obelisk, init)]
function info_window_obelisk_on_init(window) {
    var mon = city.get_monument(window.bid)
    if (!mon) {
        return
    }

    if (mon.phase() !== -1) {
        var reason = { group: 199, id: 43 }
        var carpenters = city.count_active_buildings(BUILDING_CARPENTERS_GUILD)
        var stonemasons = city.count_active_buildings(BUILDING_STONEMASONS_GUILD)

        if (mon.phase() <= 1 && carpenters === 0 && mon.needs_resource(RESOURCE_TIMBER) > 0) {
            reason = { group: 178, id: 42 }
        } else if (mon.need_stonemason() && stonemasons > 0) {
            reason = { group: 178, id: 43 }
        } else if (mon.need_stonemason() && stonemasons === 0) {
            reason = { group: 199, id: 43 }
        } else {
            reason = { group: 178, id: 42 }
        }

        window.warning_text.text = __loc(reason)
        window.progress_text.text = mon.phase() + " / " + mon.phases_total() + "    " + mon.material_pct_min() + "%"
        info_window_obelisk_fill_resource_slot(window, mon, RESOURCE_TIMBER, "timber_icon", "timber_text")
        info_window_obelisk_fill_resource_slot(window, mon, RESOURCE_GRANITE, "granite_icon", "granite_text")
    } else {
        window.warning_text.text = __loc(199, 46)
        window.progress_text.text = ""
        window.timber_text.text = ""
        window.granite_text.text = ""
        window.timber_icon.enabled = false
        window.granite_icon.enabled = false
    }
}
