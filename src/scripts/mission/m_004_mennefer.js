log_info("akhenaten: mission 4 started")

// Trade + pharaoh requests verified vs mission1.pak scenario 4 (2026-07-24 dump).

mission4 {
	start_message : "message_trade_on_the_water"
	selection_title : "Mennefer"
	env {
		has_animals : false
		marshland_grow : default_marshland_grow
	    tree_grow : default_tree_grow
	}
	player_rank : 1
	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	buildings  [
					BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
					BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION, BUILDING_VILLAGE_PALACE,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION,
					BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN,
					BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY,
					BUILDING_TEMPLE_OSIRIS, BUILDING_SHRINE_OSIRIS, BUILDING_TEMPLE_PTAH, BUILDING_SHRINE_PTAH, BUILDING_TEMPLE_BAST, BUILDING_SHRINE_BAST, BUILDING_FESTIVAL_SQUARE,
					BUILDING_CLAY_PIT, BUILDING_POTTERY_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
					BUILDING_CHICKPEAS_FARM, BUILDING_BARLEY_FARM, BUILDING_WORK_CAMP,
					BUILDING_MUD_WALL,

					BUILDING_BULLFIGHT_SCHOOL, BUILDING_MUD_GATEHOUSE, BUILDING_TOWER_GATEHOUSE, BUILDING_MUD_TOWER,
				]

	sounds {
		briefing : "Voice/Mission/204_mission.mp3"
		victory : "Voice/Mission/204_victory.mp3"
	}

	// Win ratings match mission1.pak; housing_level goal 10 from pak (count goal 0 ignored).
	win_criteria {
		population    {enabled : true, goal : 1500 }
		culture       {enabled : true, goal : 15 }
		prosperity    {enabled : true, goal : 20 }
		monuments     {enabled : true, goal : 9 }
		kingdom       {enabled : true, goal : 40 }
		housing_level {enabled : true, goal : 10 }
	}

	// Empire map from pak (empire id=1): ours + 2 trade + 2 display-only.
	hide_pak_cities : true
	cities [
		{
			name : "Perwadjyt"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FIGS, RESOURCE_CLAY, RESOURCE_BRICKS, RESOURCE_POTTERY, RESOURCE_REEDS ]
		}

		{
			name : "Nekhen"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER ]
			buys [ RESOURCE_PAPYRUS ]
		}

		{
			name : "Nubt"
			trade : false
			type : EMPIRE_CITY_EGYPTIAN
		}

		{
			name : "Thinis"
			trade : false
			type : EMPIRE_CITY_EGYPTIAN
		}
	]

	// Region labels from mission1.pak empire_map_objects (scenario 4 dump).
	hide_pak_objects : true
	empire_texts [
		{ name : "#crete", pos : [83, 159] }
		{ name : "#cyprus", pos : [594, 107] }
		{ name : "#eastern_africa", pos : [1019, 1560] }
		{ name : "#eastern_desert", pos : [702, 773] }
		{ name : "#greece", pos : [1, 67] }
		{ name : "#libya", pos : [17, 425] }
		{ name : "#lower_egypt", pos : [429, 476] }
		{ name : "#delta", pos : [518, 362] }
		{ name : "#fayuum", pos : [462, 606] }
		{ name : "#nubia", pos : [806, 1445] }
		{ name : "#palestine", pos : [833, 182] }
		{ name : "#sinai", pos : [787, 478] }
		{ name : "#syria", pos : [1003, 46] }
		{ name : "#upper_egypt", pos : [696, 993] }
		{ name : "#western_desert", pos : [230, 774] }
		{ name : "#lebanon", pos : [877, 109] }
		{ name : "#canaan", pos : [850, 271] }
	]

	vars {
		spacious_apartment_needed : 1
		papyrus_stored : 100
		bricks_stored_needed : 100
		victory_last_action_delay : 3

		spacious_apartment_built : false
		papyrus_made_handled : false
		bricks_bought_handled : false
		pharaoh_requested_beer : false
		pharaoh_requested_papyrus : false
		pharaoh_requested_barley : false
		pharaoh_requested_pottery : false
		last_action_time : 0
		start_message_shown : false
	}
}

function mission4_get_goal_tooltip() {
	if (!mission.spacious_apartment_built) {
		return "#tutorial_goal_education"
	}

	if (!mission.papyrus_made_handled) {
		return "#tutorial_goal_scribal_school"
	}

	if (!mission.bricks_bought_handled) {
		return "#tutorial_goal_import_bricks"
	}

	return ""
}

function mission4_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

[es=event_mission_start, mission=mission4]
function mission4_on_start(ev) {
	mission_show_start_message(mission, "message_trade_on_the_water")
	city.set_empire_available(1)

	if (mission.papyrus_made_handled) {
		city.set_advisor_available(ADVISOR_TRADE, 1)
		city.use_building(BUILDING_DOCK, true)
	}

	if (mission.spacious_apartment_built) {
		city.use_building(BUILDING_REED_GATHERER, true)
		city.use_building(BUILDING_PAPYRUS_WORKSHOP, true)
		city.use_building(BUILDING_SCRIBAL_SCHOOL, true)
		city.set_advisor_available(ADVISOR_EDUCATION, 1)
	}

	if (mission.bricks_bought_handled) {
		city.use_building(BUILDING_BRICKLAYERS_GUILD, true)
		city.use_building(BUILDING_SMALL_MASTABA, true)
		city.set_advisor_available(ADVISOR_MONUMENTS, 1)
	}

	city.set_advisor_available(ADVISOR_LABOR, 1)
	city.set_advisor_available(ADVISOR_IMPERIAL, 1)
	city.set_advisor_available(ADVISOR_RATINGS, 1)
	city.set_advisor_available(ADVISOR_POPULATION, 1)
	city.set_advisor_available(ADVISOR_HEALTH, 1)
	city.set_advisor_available(ADVISOR_ENTERTAINMENT, 1)
	city.set_advisor_available(ADVISOR_RELIGION, 1)
	city.set_advisor_available(ADVISOR_FINANCIAL, 1)
	city.set_advisor_available(ADVISOR_CHIEF, 1)
}

[event=event_advance_day, mission=mission4]
function mission4_handle_spacious_apartment() {
    if (mission.spacious_apartment_built) {
        return
    }

    var spacious_apartment_count = city.count_total_buildings(BUILDING_HOUSE_SPACIOUS_APARTMENT);
    if (spacious_apartment_count < mission.spacious_apartment_needed) {
        return
    }

    mission.spacious_apartment_built = true
    mission.last_action_time = game.absolute_day

    city.set_advisor_available(ADVISOR_EDUCATION, 1)

	city.use_building(BUILDING_REED_GATHERER, true)
	city.use_building(BUILDING_PAPYRUS_WORKSHOP, true)
	city.use_building(BUILDING_SCRIBAL_SCHOOL, true)

    ui.popup_message("message_tutorial_education")
	city.goal_tooltip = mission4_get_goal_tooltip
}

[event=event_update_victory_state, mission=mission4]
function mission4_handle_victory_state(ev) {
	city.victory.set_reason("spacious_apartment_built", mission.spacious_apartment_built)
	city.victory.set_reason("papyrus_made", mission.papyrus_made_handled)
	city.victory.set_reason("bricks_bought", mission.bricks_bought_handled)

	var some_days_after_last_action = (game.absolute_day - mission.last_action_time) > mission.victory_last_action_delay
	city.victory.set_reason("some_days_after_last_action", some_days_after_last_action)
}

[event=event_warehouse_filled, mission=mission4]
function mission4_handle_papyrus(ev) {
    if (mission.papyrus_made_handled) {
        return
    }

	var papyrus_amount = city.yards_stored(RESOURCE_PAPYRUS)
    if (papyrus_amount < mission.papyrus_stored) {
        return
    }

    mission.papyrus_made_handled = true
	mission.last_action_time = game.absolute_day

	city.set_advisor_available(ADVISOR_TRADE, 1)
	city.set_empire_available(1)
    city.use_building(BUILDING_DOCK, true)

	ui.popup_message("message_tutorial_trade_with_other_cities")
}

[event=event_warehouse_filled, mission=mission4]
function mission4_handle_bricks(ev) {
    if (mission.bricks_bought_handled) {
        return
    }

    var bricks_amount = city.yards_stored(RESOURCE_BRICKS)
    if (bricks_amount < mission.bricks_stored_needed) {
        return
    }

    mission.bricks_bought_handled = true
	mission.last_action_time = game.absolute_day

	city.use_building(BUILDING_BRICKLAYERS_GUILD, true)
	city.use_building(BUILDING_SMALL_MASTABA, true)

	city.set_advisor_available(ADVISOR_MONUMENTS, 1)
	ui.popup_message("message_tutorial_monuments")
}

// pak requests (reputation chains approximated from pak ±5/±6 style events).
// beer y6m8 / 9 / 12mo
[es=event_advance_month, mission=mission4]
function mission4_pharaoh_request_beer(ev) {
	if (mission.pharaoh_requested_beer) {
		return
	}
	if (ev.years_since_start < 6 || (ev.years_since_start == 6 && ev.month < 8)) {
		return
	}
	mission.pharaoh_requested_beer = true
	mission4_fire_request(10, RESOURCE_BEER, 9, 12, 110, 111, 5, 6)
}

// papyrus y8m10 / 8 / 9mo
[es=event_advance_month, mission=mission4]
function mission4_pharaoh_request_papyrus(ev) {
	if (mission.pharaoh_requested_papyrus) {
		return
	}
	if (ev.years_since_start < 8 || (ev.years_since_start == 8 && ev.month < 10)) {
		return
	}
	mission.pharaoh_requested_papyrus = true
	mission4_fire_request(11, RESOURCE_PAPYRUS, 8, 9, 112, 113, 5, 6)
}

// barley y10m5 / 10 / 18mo
[es=event_advance_month, mission=mission4]
function mission4_pharaoh_request_barley(ev) {
	if (mission.pharaoh_requested_barley) {
		return
	}
	if (ev.years_since_start < 10 || (ev.years_since_start == 10 && ev.month < 5)) {
		return
	}
	mission.pharaoh_requested_barley = true
	mission4_fire_request(12, RESOURCE_BARLEY, 10, 18, 114, 115, 5, 6)
}

// pottery y15m4 / 8 / 12mo
[es=event_advance_month, mission=mission4]
function mission4_pharaoh_request_pottery(ev) {
	if (mission.pharaoh_requested_pottery) {
		return
	}
	if (ev.years_since_start < 15 || (ev.years_since_start == 15 && ev.month < 4)) {
		return
	}
	mission.pharaoh_requested_pottery = true
	mission4_fire_request(13, RESOURCE_POTTERY, 8, 12, 116, 117, 5, 6)
}
