log_info("akhenaten: mission 6 behdet started")

// Trade / requests / invasions verified vs mission1.pak scenario 6 (2026-07-24 dump).

mission6 { // Behdet
	start_message : "message_soldiers_and_forts"
	selection_title : "Behdet"
	player_rank : 2

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

	env {
		has_animals : false
		marshland_grow : default_marshland_grow
		tree_grow : default_tree_grow
	}

	sounds {
		briefing : "Voice/Mission/206_mission.mp3"
		victory : "Voice/Mission/206_victory.mp3"
	}

	buildings [
					BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
					BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION, BUILDING_VILLAGE_PALACE,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION,
					BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN,
					BUILDING_WORK_CAMP, BUILDING_CHICKPEAS_FARM, BUILDING_BARLEY_FARM,
					BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION, BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
					BUILDING_RECRUITER, BUILDING_FORT_INFANTRY, BUILDING_FORT_ARCHERS, BUILDING_WEAPONSMITH,
					BUILDING_SCRIBAL_SCHOOL,
					BUILDING_CLAY_PIT, BUILDING_REED_GATHERER, BUILDING_GOLD_MINE,
					BUILDING_POTTERY_WORKSHOP, BUILDING_WEAVER_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_PAPYRUS_WORKSHOP, BUILDING_BRICKS_WORKSHOP,
					BUILDING_SHIPWRIGHT, BUILDING_FISHING_WHARF, BUILDING_FERRY, BUILDING_DOCK,
					BUILDING_WARSHIP_WHARF, BUILDING_TRANSPORT_WHARF,
					BUILDING_BRICKLAYERS_GUILD,
					BUILDING_SMALL_MASTABA, BUILDING_MEDIUM_MASTABA,
					BUILDING_TEMPLE_OSIRIS, BUILDING_SHRINE_OSIRIS, BUILDING_TEMPLE_RA, BUILDING_SHRINE_RA,
					BUILDING_FESTIVAL_SQUARE, BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY, BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL,
				]

	win_criteria {
		population    {enabled : true, goal : 2500 }
		culture       {enabled : true, goal : 15 }
		prosperity    {enabled : true, goal : 20 }
		monuments     {enabled : true, goal : 11 }
		kingdom       {enabled : true, goal : 45 }
		housing_level {enabled : true, goal : 10 }
	}

	// Empire from pak (sea + land, start closed).
	cities [
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
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CHICKPEAS, RESOURCE_POTTERY, RESOURCE_PAPYRUS ]
			buys [ RESOURCE_LETTUCE, RESOURCE_BRICKS, RESOURCE_BARLEY, RESOURCE_BEER, RESOURCE_LUXURY_GOODS ]
		}

		{
			name : "Nekhen"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_FLAX, RESOURCE_LINEN ]
			buys [ RESOURCE_PAPYRUS ]
		}

		{
			name : "Perwadjyt"
			is_sea_trade : true
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FIGS, RESOURCE_CLAY, RESOURCE_BRICKS, RESOURCE_POTTERY, RESOURCE_REEDS ]
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
		pharaoh_pottery_requested : false
		pharaoh_fish_requested : false
		pharaoh_beer_requested : false
		pharaoh_beer_late_requested : false
		pharaoh_bricks_gift_sent : false
		kushite_invasion_1 : false
		kushite_invasion_2 : false
		kushite_invasion_3 : false
		pharaoh_favour_invasion_done : false
		start_message_shown : false
	}
}

function mission6_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

function mission6_kushite_raid(invasion_id, size) {
	city.start_foreign_army_invasion({
		invasion_id: invasion_id,
		enemy: ENEMY_6_KUSHITE,
		size: size,
		tilex: -1,
		tiley: -1,
		want_destroy_buildings: size
	})
}

[es=event_mission_start, mission=mission6]
function mission6_on_start(ev) {
	__image_request_pak(PACK_ENEMY_KUSHITE)
	mission_show_start_message(mission, "message_soldiers_and_forts")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

// pak: year=1 month=7 pottery 14 / 9mo
[es=event_advance_month, mission=mission6]
function mission6_pharaoh_request_pottery(ev) {
	if (mission.pharaoh_pottery_requested) {
		return
	}
	if (ev.years_since_start < 1 || (ev.years_since_start == 1 && ev.month < 7)) {
		return
	}
	mission.pharaoh_pottery_requested = true
	mission6_fire_request(1, RESOURCE_POTTERY, 14, 9, 101, 102, 6, 6)
}

// pak: year=2 month=0 beer 11 / 12mo
[es=event_advance_month, mission=mission6]
function mission6_pharaoh_request_beer(ev) {
	if (mission.pharaoh_beer_requested) {
		return
	}
	if (ev.years_since_start < 2) {
		return
	}
	mission.pharaoh_beer_requested = true
	mission6_fire_request(2, RESOURCE_BEER, 11, 12, 201, 202, 12, 3)
}

// Gift not in request dump; keep prior JS timing (year 1).
[es=event_advance_month, mission=mission6]
function mission6_pharaoh_bricks_gift(ev) {
	if (mission.pharaoh_bricks_gift_sent) {
		return
	}
	if (ev.years_since_start < 1) {
		return
	}
	mission.pharaoh_bricks_gift_sent = true
	var gift = city.create_pharaoh_gift({ tag_id: 3, resource: RESOURCE_BRICKS, amount: 28 })
	gift.execute()
}

// pak: year=3 month=3 fish 13 / 12mo
[es=event_advance_month, mission=mission6]
function mission6_pharaoh_request_fish(ev) {
	if (mission.pharaoh_fish_requested) {
		return
	}
	if (ev.years_since_start < 3 || (ev.years_since_start == 3 && ev.month < 3)) {
		return
	}
	mission.pharaoh_fish_requested = true
	mission6_fire_request(4, RESOURCE_FISH, 13, 12, 401, 402, 7, 10)
}

// pak: year=17 month=0 beer 21 / 16mo
[es=event_advance_month, mission=mission6]
function mission6_pharaoh_request_beer_late(ev) {
	if (mission.pharaoh_beer_late_requested) {
		return
	}
	if (ev.years_since_start < 17) {
		return
	}
	mission.pharaoh_beer_late_requested = true
	mission6_fire_request(5, RESOURCE_BEER, 21, 16, 501, 502, 12, 7)
}

// pak invasions (scenario enemy = Kushite). Favour-KR Pharaoh army amount=45 via JS helper.
[es=event_advance_month, mission=mission6]
function mission6_kushite_invasion_1(ev) {
	if (mission.kushite_invasion_1) { return }
	if (ev.years_since_start < 5 || (ev.years_since_start == 5 && ev.month < 1)) { return }
	mission.kushite_invasion_1 = true
	log_info("akhenaten: mission 6 behdet kushite invasion 1 size=5", {ev:ev})
	mission6_kushite_raid(0, 5)
}

[es=event_advance_month, mission=mission6]
function mission6_kushite_invasion_2(ev) {
	if (mission.kushite_invasion_2) { return }
	if (ev.years_since_start < 15) { return }
	mission.kushite_invasion_2 = true
	log_info("akhenaten: mission 6 behdet kushite invasion 2 size=16", {ev:ev})
	mission6_kushite_raid(1, 16)
}

[es=event_advance_month, mission=mission6]
function mission6_kushite_invasion_3(ev) {
	if (mission.kushite_invasion_3) { return }
	if (ev.years_since_start < 19 || (ev.years_since_start == 19 && ev.month < 4)) { return }
	mission.kushite_invasion_3 = true
	log_info("akhenaten: mission 6 behdet kushite invasion 3 size=4", {ev:ev})
	mission6_kushite_raid(2, 4)
}

[es=event_advance_month, mission=mission6]
function mission6_pharaoh_favour_invasion(ev) {
	mission_pharaoh_favour_invasion_tick(mission, 45)
}
