function run_test() {
    var fails = 0
    function check(name, cond) {
        if (!cond) {
            log_info("FAIL: " + name)
            fails = fails + 1
        } else {
            log_info("ok: " + name)
        }
    }

    var cfg = get_mission_config(48)
    check("playable48", mission_is_playable(48))
    check("enable_events", !!cfg.enable_scenario_events)
    check("events40", cfg.events && cfg.events.length == 40)
    check("hide_cities", !!cfg.hide_pak_cities)
    check("cities19", cfg.cities && cfg.cities.length == 19)
    check("hide_routes", !!cfg.hide_pak_routes)
    check("routes13", cfg.empire_routes && cfg.empire_routes.length == 13)
    check("hide_objects", !!cfg.hide_pak_objects)
    check("ornaments8", cfg.empire_ornaments && cfg.empire_ornaments.length == 8)
    check("texts16", cfg.empire_texts && cfg.empire_texts.length == 16)

    var by_name = {}
    for (var i = 0; i < cfg.cities.length; i++) {
        by_name[cfg.cities[i].name] = cfg.cities[i]
    }
    check("ours_alex", by_name["Alexandria"] && by_name["Alexandria"].type == EMPIRE_CITY_OURS)
    check("byblos_open", by_name["Byblos"] && by_name["Byblos"].is_open == true)
    check("byblos_oil", by_name["Byblos"].sells && by_name["Byblos"].sells.indexOf(RESOURCE_OIL) >= 0)
    check("knossos_r0", by_name["Knossos"] && by_name["Knossos"].trade == false && by_name["Knossos"].route == 0)
    check("bubastis_r0", by_name["Bubastis"] && by_name["Bubastis"].route == 0)
    check("tyre_r0", by_name["Tyre"] && by_name["Tyre"].route == 0)
    check("migdol_r0", by_name["Migdol"] && by_name["Migdol"].route == 0)
    check("waset_r0", by_name["Waset"] && by_name["Waset"].route == 0)
    check("on_r14", by_name["On"] && by_name["On"].trade == false && by_name["On"].route == 14)
    check("bahariya_r6", by_name["Bahariya Oasis"] && by_name["Bahariya Oasis"].route == 6)

    check("orn0_img", cfg.empire_ornaments[0].image == 13856)
    check("text_crete", cfg.empire_texts[0].name == "#crete")
    check("text_lebanon", cfg.empire_texts[15].name == "#lebanon")

    var by_route = {}
    for (var r = 0; r < cfg.empire_routes.length; r++) {
        by_route[cfg.empire_routes[r].route] = cfg.empire_routes[r]
    }
    check("route1_sea", by_route[1] && by_route[1].type == 2)
    check("route14_on", !!by_route[14])
    check("no_orphan15", !by_route[15])

    __game_load_mission(48, 1)
    check("48.pop", __win_criteria.population.goal == 4000)
    check("48.year", scenario.start_year == -331)

    log_info("CC48 display-route0 smoke fails=" + fails)
    __test_signal_ready()
}
function check_valid() {
    return __test_find_inlog("CC48 display-route0 smoke fails=0")
}
