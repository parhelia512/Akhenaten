log_info("akhenaten: mission 8 selima started")

// Trade / requests / invasions verified vs mission1.pak scenario 8 (2026-07-24 dump).
// Favour Pharaoh army size=63 (trigger=by_favour) proxied in JS until B2b.

mission8 { // Selima
	start_message : "message_the_finer_things_tutorial"
	selection_title : "Selima"
	player_rank : 3
	next_mission : 10
	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	env {
		has_animals : false
		hide_nilometer : true
		marshland_grow : default_marshland_grow
		tree_grow : default_tree_grow
	}

	sounds {
		briefing : "Voice/Mission/208_mission.mp3"
		victory : "Voice/Mission/20_victory.mp3"
	}

	buildings [
                BUILDING_HOUSE_VACANT_LOT, BUILDING_CLEAR_LAND, BUILDING_ROAD,
				BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION,
                BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN,
				BUILDING_VILLAGE_PALACE, BUILDING_HUNTING_LODGE,
				BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
                BUILDING_WOOD_CUTTERS, BUILDING_POTTERY_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_PAPYRUS_WORKSHOP,
				BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION, BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
                BUILDING_RECRUITER, BUILDING_WEAPONSMITH, BUILDING_FORT_CHARIOTEERS, BUILDING_FORT_ARCHERS, BUILDING_FORT_INFANTRY,
                BUILDING_TEMPLE_SETH, BUILDING_SHRINE_SETH, BUILDING_TEMPLE_RA, BUILDING_SHRINE_RA,
				BUILDING_TEMPLE_COMPLEX_SETH, BUILDING_TEMPLE_COMPLEX_ALTAR_ANUBIS, BUILDING_TEMPLE_COMPLEX_ORACLE_SEKHMET,
				BUILDING_FESTIVAL_SQUARE, BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY, BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL,
                BUILDING_SCRIBAL_SCHOOL,
			  ]

	win_criteria {
		population    {enabled : true, goal : 3000 }
		culture       {enabled : true, goal : 20 }
		prosperity    {enabled : true, goal : 20 }
		kingdom       {enabled : true, goal : 55 }
		housing_level {enabled : true, goal : 10 }
		monuments     {enabled : false }
	}

	enable_scenario_events : true

	invasion_points_land [
		[30, 26]
	]

	// Empire from pak (all land, start closed).
	cities [
		{
			name : "Abedju"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_BARLEY, RESOURCE_BEER, RESOURCE_LINEN, RESOURCE_STONE ]
			buys [ RESOURCE_GAMEMEAT, RESOURCE_CLAY, RESOURCE_BRICKS, RESOURCE_TIMBER, RESOURCE_PAPYRUS, RESOURCE_LIMESTONE, RESOURCE_GRANITE, RESOURCE_SANDSTONE ]
		}

		{
			name : "Behdet"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_REEDS, RESOURCE_PAPYRUS, RESOURCE_GRANITE ]
			buys [ RESOURCE_BRICKS, RESOURCE_LINEN, RESOURCE_GEMS, RESOURCE_LUXURY_GOODS, RESOURCE_TIMBER ]
		}

		{
			name : "Kerma"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_LUXURY_GOODS ]
			buys [ RESOURCE_LINEN, RESOURCE_LUXURY_GOODS ]
		}

		{
			name : "Men-nefer"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CHICKPEAS, RESOURCE_POTTERY, RESOURCE_PAPYRUS ]
			buys [ RESOURCE_LETTUCE, RESOURCE_BRICKS, RESOURCE_BARLEY, RESOURCE_BEER, RESOURCE_LUXURY_GOODS ]
		}

		{
			name : "Timna"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_WEAPONS, RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_COPPER ]
			buys [ RESOURCE_FISH, RESOURCE_BEER, RESOURCE_LINEN, RESOURCE_PAPYRUS ]
		}
	]

	vars {
		pharaoh_luxury_request_count : 0
		pharaoh_troops_requested_1 : false
		pharaoh_troops_requested_2 : false
		random_trade_city_under_siege : false
		hyksos_invasion_1 : false
		hyksos_invasion_2 : false
		pharaoh_favour_invasion_done : false
		distant_battle_requested : false
		start_message_shown : false
	}
}

function mission8_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

[es=event_mission_start, mission=mission8]
function mission8_on_start(ev) {
	__image_request_pak(PACK_ENEMY_HYKSOS)
	__image_request_pak(PACK_ENEMY_EGYPTIAN)
	__image_request_pak(PACK_TEMPLE_SETH)
	mission_show_start_message(mission, "message_the_finer_things_tutorial")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

// pak: luxury_goods 2 / 4mo, trigger=recurring — re-request every 2 years.
[es=event_advance_month, mission=mission8]
function mission8_pharaoh_request_luxury(ev) {
	var next_year = 1 + mission.pharaoh_luxury_request_count * 2
	if (ev.years_since_start < next_year) {
		return
	}
	mission.pharaoh_luxury_request_count = mission.pharaoh_luxury_request_count + 1
	log_info("akhenaten: mission 8 selima luxury request #" + mission.pharaoh_luxury_request_count, {ev:ev})
	mission8_fire_request(1, RESOURCE_LUXURY_GOODS, 2, 4, 101, 102, 10, 19)
}

// pak: request subtype=CITY_ASKS_FOR_TROOPS (item id 32 collides with henna) amount=7 @ y6m8.
[es=event_advance_month, mission=mission8]
function mission8_pharaoh_request_troops_1(ev) {
	if (mission.pharaoh_troops_requested_1) {
		return
	}
	if (ev.years_since_start < 6 || (ev.years_since_start == 6 && ev.month < 8)) {
		return
	}
	mission.pharaoh_troops_requested_1 = true
	mission8_fire_request(3, RESOURCE_TROOPS, 7, 12, 301, 302, 2, 10)
}

// pak: troop request amount=4 @ y16m4 (chain in pak; calendar proxy).
[es=event_advance_month, mission=mission8]
function mission8_pharaoh_request_troops_2(ev) {
	if (mission.pharaoh_troops_requested_2) {
		return
	}
	if (ev.years_since_start < 16 || (ev.years_since_start == 16 && ev.month < 4)) {
		return
	}
	mission.pharaoh_troops_requested_2 = true
	mission8_fire_request(4, RESOURCE_TROOPS, 4, 16, 401, 402, 2, 10)
}

[es=event_advance_month, mission=mission8]
function mission8_trade_city_under_siege(ev) {
	if (mission.random_trade_city_under_siege) {
		return
	}
	if (ev.years_since_start < 2) {
		return
	}
	mission.random_trade_city_under_siege = true
	var request = city.create_trade_city_under_siege(/*tag_id*/2, /*months_initial*/12)
	request.set_reasons(PHRASE_trade_city_siege_no_reason_A, PHRASE_trade_city_siege_no_reason_B, PHRASE_trade_city_siege_no_reason_C, -1)
	request.execute()
}

// pak: year=2 month=4 enemy size=9 recurring. Scenario enemy = Hyksos.
[es=event_advance_month, mission=mission8]
function mission8_hyksos_invasion_1(ev) {
	if (mission.hyksos_invasion_1) {
		return
	}
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 4)) {
		return
	}
	mission.hyksos_invasion_1 = true
	log_info("akhenaten: mission 8 selima hyksos invasion 1 size=9", {ev:ev})
	city.start_foreign_army_invasion({
		invasion_id: 0,
		enemy: ENEMY_5_HYKSOS,
		size: 9,
		tilex: 30,
		tiley: 26,
		want_destroy_buildings: 9
	})
}

// pak: year=7 month=0 enemy size=22 once.
[es=event_advance_month, mission=mission8]
function mission8_hyksos_invasion_2(ev) {
	if (mission.hyksos_invasion_2) {
		return
	}
	if (ev.years_since_start < 7) {
		return
	}
	mission.hyksos_invasion_2 = true
	log_info("akhenaten: mission 8 selima hyksos invasion 2 size=22", {ev:ev})
	city.start_foreign_army_invasion({
		invasion_id: 1,
		enemy: ENEMY_5_HYKSOS,
		size: 22,
		tilex: 30,
		tiley: 26,
		want_destroy_buildings: 22
	})
}

// pak: favour Pharaoh army amount=63. Walkthrough distant battles ≈ CITY_STATUS_CHANGE;
// keep a Kerma distant-battle proxy (no EVENT_TYPE_DISTANT_BATTLE in pak).
[es=event_advance_month, mission=mission8]
function mission8_distant_battle_request(ev) {
	if (mission.distant_battle_requested) {
		return
	}
	if (ev.years_since_start < 5) {
		return
	}
	mission.distant_battle_requested = true
	log_info("akhenaten: mission 8 selima distant battle proxy Kerma", {ev:ev})
	var battle = city.create_distant_battle({ tag_id: 5, city: "Kerma" })
	battle.set_location_fields(-1, -1, -1, -1)
	battle.set_image("pharaoh_unloaded/dialougedrawing_00012")
	battle.set_param("months_initial", 14)
	battle.set_param("amount", 2)
	battle.set_reasons(PHRASE_distant_battle_initial_announcement_P, -1, -1, -1)
	battle.execute()
}

[es=event_advance_month, mission=mission8]
function mission8_pharaoh_favour_invasion(ev) {
	mission_pharaoh_favour_invasion_tick(mission, 63)
}
