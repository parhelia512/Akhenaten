log_info("akhenaten: mission 7 abydos started")

// Trade / requests / invasions verified vs mission1.pak scenario 7 (2026-07-24 dump).

mission7 { // Abydos / Abedju
	start_message : "message_soldiers_and_forts"
	selection_title : "Abydos"
	env {
		has_animals : false
		marshland_grow : default_marshland_grow
		tree_grow : default_tree_grow
	}
	player_rank : 3

	choice_background {pack:PACK_UNLOADED, id:12}
	choice_image1 {pack:PACK_UNLOADED, id:13}
	choice_image1_pos [192, 144]
	choice_title [144, 22]

	choice [
		{
			name : "Selima"
			id : 8
			image {pack:PACK_UNLOADED, id:20, offset:0}
			tooltip [144, 23]
			pos [620, 420]
		}

		{
			name : "Abu"
			id : 9
			image {pack:PACK_UNLOADED, id:20}
			tooltip [144, 24]
			pos [640, 480]
		}
	]

	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	init_resources : {
		barley: { type:RESOURCE_BARLEY, allow: true},
		flax: { type:RESOURCE_FLAX, allow:true},
	}

	sounds {
		briefing : "Voice/Mission/207_mission.mp3"
		victory : "Voice/Mission/207_victory.mp3"
	}

	buildings [
					BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
					BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION, BUILDING_VILLAGE_PALACE,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION,
					BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN, BUILDING_DENTIST,
					BUILDING_WORK_CAMP, BUILDING_FLAX_FARM, BUILDING_BARLEY_FARM,
					BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION, BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
					BUILDING_RECRUITER, BUILDING_FORT_INFANTRY, BUILDING_FORT_ARCHERS, BUILDING_WEAPONSMITH,
					BUILDING_SCRIBAL_SCHOOL,
					BUILDING_CLAY_PIT, BUILDING_REED_GATHERER,
					BUILDING_POTTERY_WORKSHOP, BUILDING_WEAVER_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_PAPYRUS_WORKSHOP, BUILDING_BRICKS_WORKSHOP,
					BUILDING_SHIPWRIGHT, BUILDING_FISHING_WHARF, BUILDING_FERRY,
					BUILDING_WARSHIP_WHARF, BUILDING_TRANSPORT_WHARF, BUILDING_DOCK,
					BUILDING_TEMPLE_OSIRIS, BUILDING_SHRINE_OSIRIS,
					BUILDING_BRICKLAYERS_GUILD,
					BUILDING_SMALL_MASTABA, BUILDING_MEDIUM_MASTABA,
					BUILDING_FESTIVAL_SQUARE, BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY,
					BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL, BUILDING_SENET_HOUSE
				]

	win_criteria {
		population    {enabled : true, goal : 2500 }
		culture       {enabled : true, goal : 25 }
		prosperity    {enabled : true, goal : 25 }
		monuments     {enabled : true, goal : 17 }
		kingdom       {enabled : true, goal : 60 }
		housing_level {enabled : true, goal : 10 }
	}

	// hide_pak_cities: only cities below appear on the empire map (pak leftovers hidden).
	// Display-only city example: { name : "Selima", trade : false } or type : EMPIRE_CITY_FOREIGN
	hide_pak_cities : true
	cities [
		{
			name : "Behdet"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_REEDS, RESOURCE_PAPYRUS, RESOURCE_GRANITE ]
			buys [ RESOURCE_BRICKS, RESOURCE_LINEN, RESOURCE_GEMS, RESOURCE_LUXURY_GOODS, RESOURCE_TIMBER ]
		}

		{
			name : "Byblos"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_LUXURY_GOODS, RESOURCE_TIMBER, RESOURCE_COPPER ]
			buys [ RESOURCE_GEMS, RESOURCE_LUXURY_GOODS ]
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
			name : "Nubt"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_GAMEMEAT, RESOURCE_STRAW, RESOURCE_CLAY, RESOURCE_BRICKS ]
		}

		{
			name : "Perwadjyt"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FIGS, RESOURCE_STRAW, RESOURCE_CLAY, RESOURCE_BRICKS, RESOURCE_POTTERY, RESOURCE_REEDS ]
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
		pharaoh_beer_requested : false
		pharaoh_fish_requested : false
		pharaoh_bricks_gift_sent : false
		kushite_invasion_1 : false
		pharaoh_favour_invasion_done : false
		start_message_shown : false
	}
}

function mission7_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

[es=event_mission_start, mission=mission7]
function mission7_on_start(ev) {
	__image_request_pak(PACK_ENEMY_KUSHITE)
	mission_show_start_message(mission, "message_soldiers_and_forts")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

// pak: year=2 month=2 beer 9 / 12mo
[es=event_advance_month, mission=mission7]
function mission7_pharaoh_request_beer(ev) {
	if (mission.pharaoh_beer_requested) {
		return
	}
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 2)) {
		return
	}
	mission.pharaoh_beer_requested = true
	mission7_fire_request(1, RESOURCE_BEER, 9, 12, 101, 102, 4, 3)
}

// Gift not in request dump; keep prior JS timing (y3 m4, bricks 21).
[es=event_advance_month, mission=mission7]
function mission7_pharaoh_bricks_gift(ev) {
	if (mission.pharaoh_bricks_gift_sent) {
		return
	}
	if (ev.years_since_start < 3 || (ev.years_since_start == 3 && ev.month < 4)) {
		return
	}
	mission.pharaoh_bricks_gift_sent = true
	var gift = city.create_pharaoh_gift({ tag_id: 2, resource: RESOURCE_BRICKS, amount: 21 })
	gift.execute()
}

// pak: year=12 month=1 fish 18 / 12mo
[es=event_advance_month, mission=mission7]
function mission7_pharaoh_request_fish(ev) {
	if (mission.pharaoh_fish_requested) {
		return
	}
	if (ev.years_since_start < 12 || (ev.years_since_start == 12 && ev.month < 1)) {
		return
	}
	mission.pharaoh_fish_requested = true
	mission7_fire_request(3, RESOURCE_FISH, 18, 12, 301, 302, 4, 3)
}

// pak: y14 m9 size=3 Kushite (timed). Favour-KR Pharaoh army amount=40 via JS helper.
// chain_only Pharaoh army (amount=40, trigger=1) still needs B2c.
[es=event_advance_month, mission=mission7]
function mission7_kushite_invasion_1(ev) {
	if (mission.kushite_invasion_1) {
		return
	}
	if (ev.years_since_start < 14 || (ev.years_since_start == 14 && ev.month < 9)) {
		return
	}
	mission.kushite_invasion_1 = true
	log_info("akhenaten: mission 7 abydos kushite invasion size=3", {ev:ev})
	city.start_foreign_army_invasion({
		invasion_id: 0,
		enemy: ENEMY_6_KUSHITE,
		size: 3,
		tilex: -1,
		tiley: -1,
		want_destroy_buildings: 3
	})
}

[es=event_advance_month, mission=mission7]
function mission7_pharaoh_favour_invasion(ev) {
	mission_pharaoh_favour_invasion_tick(mission, 40)
}
