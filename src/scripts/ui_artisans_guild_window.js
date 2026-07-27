log_info("akhenaten: ui artisans guild info window started")

[es=building_info_window]
artisans_guild_info_window {
    related_buildings [BUILDING_ARTISANS_GUILD]
    ui : baseui(workshop_info_window, {
        background    : outer_panel({size[29, 18] })
        resource_icon : resource_icon({pos[32, 56], prop:"${building.first_material}" })
        resource_stored : text({pos[60, 60], size[px(27), 20], font:FONT_NORMAL_BLACK_ON_LIGHT })
        resource_icon_b : resource_icon({pos[32, 80], prop:"${building.second_material}" })
        resource_stored_b : text({pos[60, 84], size[px(27), 20], font:FONT_NORMAL_BLACK_ON_LIGHT })
        warning_text  : text({pos[32, 110], size[px(27), 60], wrap:px(27), font : FONT_NORMAL_BLACK_ON_LIGHT, multiline:true })
        workers_panel : inner_panel({pos[16, 150], size[27, 4] })
        workers_img   : image({pos[40, 162], pack:PACK_GENERAL, id:134, offset:14})
        workers_text  : text({pos[60, 172], font: FONT_NORMAL_BLACK_ON_DARK })
    })
}

[es=(artisans_guild_info_window, init)]
function artisans_guild_info_window_on_init(window) {
    var b = city.get_building(window.bid)
    window.mothball.enabled = game_features.gameui_building_mothball_button && b.max_workers && b.max_workers > 0
    window.mothball.tooltip = __loc(54, b.state == 1 ? 16 : 17)
    workshop_info_window_setup_advisors(window, b.type)

    var paint = b.stored_resource(RESOURCE_PAINT)
    var clay = b.stored_resource(RESOURCE_CLAY)
    var reason = { group: 312, id: 0 }
    if (!b.has_road_access) {
        reason = { key: "#building_no_road_access" }
    } else if (b.num_workers <= 0) {
        reason.id = 2
    } else if (paint < 100) {
        reason.id = 8
    } else if (clay < 100) {
        reason.id = 9
    } else {
        // Locale 312: 3=full … 7=nearly empty (id 4 is staffing, not mothball)
        reason.id = Math.approximate_value(b.worker_percentage / 100.0, [7, 6, 5, 4, 3])
    }
    window.warning_text.text = __loc(reason)
    window.resource_stored.text = __loc(312, 10) + " " + workshop_format_stored_amount(paint)
    window.resource_stored_b.text = __loc(312, 11) + " " + workshop_format_stored_amount(clay)
    window.workers_text.text = workshop_format_workers_text(b)
}
