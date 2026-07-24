log_info("akhenaten: mission 9 abu started")

// Trade / requests verified vs mission1.pak scenario 9 (2026-07-24 dump).
// Favour Pharaoh army size=40 proxied in JS. Chain Pharaoh armies (20) need B2c.
// CITY_STATUS_CHANGE (Selima conquered @y6) not wired — needs status-change handler.

mission9 { // Abu
	start_message : "message_history_abu"
	selection_title : "Abu"
	env {
		has_animals : false
		marshland_grow : default_marshland_grow
		tree_grow : default_tree_grow
	}
	player_rank : 6
	next_mission : 10
	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	init_resources : {
		grain : { type:RESOURCE_GRAIN, allow: true}
		barley: { type:RESOURCE_BARLEY, allow: true}
		flax: { type:RESOURCE_FLAX, allow:true}
		lettuce: { type:RESOURCE_LETTUCE, allow:true}
		chickpeas: { type:RESOURCE_CHICKPEAS, allow:true}
	}

	sounds {
		briefing : "Voice/Mission/209_mission.mp3"
		victory : "Voice/Mission/209_victory.mp3"
	}

	buildings [
		         BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
				 BUILDING_JEWELS_WORKSHOP, BUILDING_POTTERY_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_WEAVER_WORKSHOP, BUILDING_PAPYRUS_WORKSHOP, BUILDING_BRICKS_WORKSHOP,
				 BUILDING_GRANITE_QUARRY, BUILDING_SANDSTONE_QUARRY, BUILDING_GEMSTONE_MINE,
				 BUILDING_GRAIN_FARM, BUILDING_BARLEY_FARM, BUILDING_CHICKPEAS_FARM, BUILDING_FLAX_FARM, BUILDING_LETTUCE_FARM, BUILDING_WORK_CAMP,
				 BUILDING_TEMPLE_OSIRIS, BUILDING_SHRINE_OSIRIS, BUILDING_TEMPLE_PTAH, BUILDING_SHRINE_PTAH,
				 BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION,
				 BUILDING_WATER_SUPPLY,
				 BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
				 BUILDING_ROADBLOCK, BUILDING_FERRY, BUILDING_DOCK,
				 BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY, BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL,
				 BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_FAMILY_MANSION, BUILDING_TOWN_PALACE,
				 BUILDING_FESTIVAL_SQUARE, BUILDING_TEMPLE_COMPLEX_OSIRIS, BUILDING_TEMPLE_COMPLEX_PTAH,
				 BUILDING_IRRIGATION_DITCH, BUILDING_WATER_LIFT,
				]

	win_criteria {
		population    {enabled : true, goal : 4000 }
		culture       {enabled : true, goal : 30 }
		prosperity    {enabled : true, goal : 30 }
		kingdom       {enabled : true, goal : 50 }
		housing_level {enabled : true, goal : 10 }
		monuments     {enabled : false }
	}

	// Empire from pak (4 trade partners). Wiki Byblos/Nubt/Kyrene/Selima not in this pak empire.
	cities [
		{
			name : "Abedju"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_BARLEY, RESOURCE_BEER, RESOURCE_LINEN, RESOURCE_STONE ]
			buys [ RESOURCE_GAMEMEAT, RESOURCE_CLAY, RESOURCE_BRICKS, RESOURCE_TIMBER, RESOURCE_PAPYRUS, RESOURCE_GRANITE, RESOURCE_SANDSTONE ]
		}

		{
			name : "Behdet"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_REEDS, RESOURCE_PAPYRUS, RESOURCE_GRANITE ]
			buys [ RESOURCE_BRICKS, RESOURCE_LINEN, RESOURCE_GEMS, RESOURCE_LUXURY_GOODS, RESOURCE_TIMBER ]
		}

		{
			name : "Men-nefer"
			is_sea_trade : true
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
		pharaoh_requested_granite : false
		pharaoh_requested_sandstone : false
		pharaoh_favour_invasion_done : false
		start_message_shown : false
	}
}

function mission9_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

[es=event_mission_start, mission=mission9]
function mission9_on_start(ev) {
	__image_request_pak(PACK_ENEMY_EGYPTIAN)
	mission_show_start_message(mission, "message_history_abu")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

// pak: year=4 month=7 granite 13 / 12mo
[es=event_advance_month, mission=mission9]
function mission9_pharaoh_request_granite(ev) {
	if (mission.pharaoh_requested_granite) {
		return
	}
	if (ev.years_since_start < 4 || (ev.years_since_start == 4 && ev.month < 7)) {
		return
	}
	mission.pharaoh_requested_granite = true
	mission9_fire_request(1, RESOURCE_GRANITE, 13, 12, 101, 102, 6, 7)
}

// pak: year=10 month=0 sandstone 22 / 16mo (subtype=construction project)
[es=event_advance_month, mission=mission9]
function mission9_pharaoh_request_sandstone(ev) {
	if (mission.pharaoh_requested_sandstone) {
		return
	}
	if (ev.years_since_start < 10) {
		return
	}
	mission.pharaoh_requested_sandstone = true
	mission9_fire_request(2, RESOURCE_SANDSTONE, 22, 16, 201, 202, 6, 11)
}

[es=event_advance_month, mission=mission9]
function mission9_pharaoh_favour_invasion(ev) {
	mission_pharaoh_favour_invasion_tick(mission, 40)
}
