log_info("akhenaten: ui building info window started")

function building_info_window_toggle_overlay() {
    var b = city.get_building(city.object_info.bid)
    city.current_overlay = (city.current_overlay == b.overlay) ? OVERLAY_NONE : b.overlay
}

function building_info_window_text_mothball(window) {
    var b = city.get_building(city.object_info.bid)
    return (b.state == 1 ? "x" : "")
}

function building_info_window_text_overlay(window) {
    var b = city.get_building(city.object_info.bid)
    return (city.current_overlay == b.overlay ? "V" : "v")
}

function building_info_window_toggle_mothball() {
    var b = city.get_building(city.object_info.bid)
    if (b.max_workers) {
        b.mothball_toggle()
    }
}

building_info_window {
    ui {
        background     : outer_panel({size: [29, 17]})
        title          : text({ pos[0, 16], text:"${building.name}", size[px(29), 20], font : FONT_LARGE_BLACK_ON_LIGHT, align:"center" })
        warning_text   : text({ pos[20, 46], wrap:px(27), font : FONT_NORMAL_BLACK_ON_LIGHT, multiline:true })
        inner_panel    : inner_panel({
                                        pos[16, 100], size[27, 5]
                                        ui {
                                            workers_img : image({pack:PACK_GENERAL, id:134, offset:14, pos[20, 10] })
                                            workers_text : text({pos[50, 16], text:"${building.num_workers} ${loc.building_employee} ( ${model.laborers}  ${loc.building_employee_needed} )", font: FONT_NORMAL_BLACK_ON_DARK})
                                            workers_desc : text({pos[50, 16 + 16], font: FONT_NORMAL_BLACK_ON_DARK,  multiline:true, wrap:px(24) })
                                        }
                                    })
        first_advisor  : image_button({ pos[40, -1], size[28, 28], pack:PACK_GENERAL, id:106 })
        second_advisor : image_button({ pos[65, -1], size[28, 28], pack:PACK_GENERAL, id:106 })
        third_advisor  : image_button({ pos[96, -1], size[28, 28], pack:PACK_GENERAL, id:106 })

        show_overlay   : button({
                                  margin{right:-64, bottom:-40}, size[23, 23]
                                  textfn: building_info_window_text_overlay
                                })
        mothball       : button({
                                  margin{right:-90, bottom:-40}, size[23, 23]
                                  textfn: building_info_window_text_mothball
                                })

        button_help    : help_button({})
        button_close   : close_button({})
    }
}


[es=(artisans_guild_info_window, show_overlay), es=(bazaar_info_window, show_overlay), es=(bricklayers_guild_info_window, show_overlay), es=(brickworks_info_window, show_overlay), es=(building_info_window, show_overlay), es=(carpenters_guild_info_window, show_overlay), es=(festival_square_info_window, show_overlay), es=(health_info_window, show_overlay), es=(industry_office_info_window, show_overlay), es=(info_window_architect_post, show_overlay), es=(info_window_bandstand, show_overlay), es=(info_window_booth, show_overlay), es=(info_window_bullfight, show_overlay), es=(info_window_burning_ruin, show_overlay), es=(info_window_courthouse, show_overlay), es=(info_window_dock, show_overlay), es=(info_window_education, show_overlay), es=(info_window_entertainment, show_overlay), es=(info_window_farm, show_overlay), es=(info_window_ferry, show_overlay), es=(info_window_fishing_wharf, show_overlay), es=(info_window_food_mill, show_overlay), es=(info_window_fort, show_overlay), es=(info_window_gatehouse, show_overlay), es=(info_window_granary, show_overlay), es=(info_window_house, show_overlay), es=(info_window_hunting_lodge, show_overlay), es=(info_window_lamp_workshop, show_overlay), es=(info_window_mansion, show_overlay), es=(info_window_milacademy, show_overlay), es=(info_window_mission_post, show_overlay), es=(info_window_mortuary, show_overlay), es=(info_window_native_crops, show_overlay), es=(info_window_native_hut, show_overlay), es=(info_window_native_meeting, show_overlay), es=(info_window_oracle, show_overlay), es=(info_window_palace, show_overlay), es=(info_window_pavilion, show_overlay), es=(info_window_police_station, show_overlay), es=(info_window_raw_material, show_overlay), es=(info_window_recruiter, show_overlay), es=(info_window_roadblock, show_overlay), es=(info_window_senet_house, show_overlay), es=(info_window_shrine, show_overlay), es=(info_window_statue, show_overlay), es=(info_window_storageyard, show_overlay), es=(info_window_temple, show_overlay), es=(info_window_temple_complex, show_overlay), es=(info_window_tower, show_overlay), es=(info_window_triumphal_arch, show_overlay), es=(info_window_vacant_lot, show_overlay), es=(info_window_water_lift, show_overlay), es=(info_window_water_supply, show_overlay), es=(info_window_work_camp, show_overlay), es=(info_window_zoo, show_overlay), es=(infrastructure_common_info_window, show_overlay), es=(mortuary_info_window, show_overlay), es=(raw_material_info_window, show_overlay), es=(scribal_school_info_window, show_overlay), es=(shipyard_info_window, show_overlay), es=(stonemason_guild_info_window, show_overlay), es=(taxcollector_info_window, show_overlay), es=(warshipwharf_info_window, show_overlay), es=(well_info_window, show_overlay), es=(workshop_info_window, show_overlay)]
function building_info_window_on_show_overlay(window) {
    building_info_window_toggle_overlay()
}

[es=(artisans_guild_info_window, mothball), es=(bazaar_info_window, mothball), es=(bricklayers_guild_info_window, mothball), es=(brickworks_info_window, mothball), es=(building_info_window, mothball), es=(carpenters_guild_info_window, mothball), es=(festival_square_info_window, mothball), es=(health_info_window, mothball), es=(industry_office_info_window, mothball), es=(info_window_architect_post, mothball), es=(info_window_bandstand, mothball), es=(info_window_booth, mothball), es=(info_window_bullfight, mothball), es=(info_window_burning_ruin, mothball), es=(info_window_courthouse, mothball), es=(info_window_dock, mothball), es=(info_window_education, mothball), es=(info_window_entertainment, mothball), es=(info_window_farm, mothball), es=(info_window_ferry, mothball), es=(info_window_fishing_wharf, mothball), es=(info_window_food_mill, mothball), es=(info_window_fort, mothball), es=(info_window_gatehouse, mothball), es=(info_window_granary, mothball), es=(info_window_house, mothball), es=(info_window_hunting_lodge, mothball), es=(info_window_lamp_workshop, mothball), es=(info_window_mansion, mothball), es=(info_window_milacademy, mothball), es=(info_window_mission_post, mothball), es=(info_window_mortuary, mothball), es=(info_window_native_crops, mothball), es=(info_window_native_hut, mothball), es=(info_window_native_meeting, mothball), es=(info_window_oracle, mothball), es=(info_window_palace, mothball), es=(info_window_pavilion, mothball), es=(info_window_police_station, mothball), es=(info_window_raw_material, mothball), es=(info_window_recruiter, mothball), es=(info_window_roadblock, mothball), es=(info_window_senet_house, mothball), es=(info_window_shrine, mothball), es=(info_window_statue, mothball), es=(info_window_storageyard, mothball), es=(info_window_temple, mothball), es=(info_window_temple_complex, mothball), es=(info_window_tower, mothball), es=(info_window_triumphal_arch, mothball), es=(info_window_vacant_lot, mothball), es=(info_window_water_lift, mothball), es=(info_window_water_supply, mothball), es=(info_window_work_camp, mothball), es=(info_window_zoo, mothball), es=(infrastructure_common_info_window, mothball), es=(mortuary_info_window, mothball), es=(raw_material_info_window, mothball), es=(scribal_school_info_window, mothball), es=(shipyard_info_window, mothball), es=(stonemason_guild_info_window, mothball), es=(taxcollector_info_window, mothball), es=(warshipwharf_info_window, mothball), es=(well_info_window, mothball), es=(workshop_info_window, mothball)]
function building_info_window_on_mothball(window) {
    building_info_window_toggle_mothball()
}
