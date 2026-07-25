log_info("akhenaten: mission 11 serabit khadim started")

// Trade / requests / invasions verified vs mission1.pak (scenario 11) via
// __test_mission_pak_dump (D1b, 2026-07-24). Remaining pak EVENT_TYPE_INVASION /
// chain events need B2 to fire from the binary; JS proxies timed requests + raids.

mission11 { // Serabit Khadim
	map_file : "data/maps/m_011_serabit_khadim.map"
	start_message : "message_mission_serabit_khadim"
	selection_title : "Serabit Khadim"
	player_rank : 5

	choice_background {pack:PACK_UNLOADED, id:12}
	choice_image1 {pack:PACK_UNLOADED, id:13}
    choice_image1_pos [192, 144]
	choice_title [144, 31]

	choice [
		{
			name : "Buhen"
			id : 13
			image {pack:PACK_UNLOADED, id:20, offset:0}
			tooltip [144, 32]
			pos [620, 420]
		}

		{
			name : "Dahshur"
			id : 14
			image {pack:PACK_UNLOADED, id:20}
			tooltip [144, 33]
			pos [640, 480]
		}
	]

	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	env {
		has_animals : true
	    hide_nilometer : true
	    marshland_grow : default_marshland_grow
	    tree_grow : default_tree_grow
	}

	sounds {
		briefing : "Voice/Mission/211_mission.mp3"
		victory : "Voice/Mission/211_victory.mp3"
	}

	buildings [
                BUILDING_HOUSE_VACANT_LOT, BUILDING_CLEAR_LAND, BUILDING_ROAD,
				BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION,
                BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN,
				BUILDING_VILLAGE_PALACE, BUILDING_HUNTING_LODGE, BUILDING_WORK_CAMP,
				BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
                BUILDING_POTTERY_WORKSHOP, BUILDING_BREWERY_WORKSHOP, BUILDING_JEWELS_WORKSHOP,
				BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION, BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
                BUILDING_RECRUITER, BUILDING_WEAPONSMITH, BUILDING_FORT_INFANTRY, BUILDING_FORT_ARCHERS,
                BUILDING_TEMPLE_SETH, BUILDING_SHRINE_SETH, BUILDING_TEMPLE_RA, BUILDING_SHRINE_RA,
				BUILDING_FESTIVAL_SQUARE, BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY, BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL,
                BUILDING_SCRIBAL_SCHOOL,
				BUILDING_COPPER_MINE, BUILDING_GEMSTONE_MINE,
				BUILDING_MUD_WALL, BUILDING_MUD_GATEHOUSE, BUILDING_MUD_TOWER,
			  ]

	// Goals match mission1.pak (pop 2000, kingdom 80; culture/prosperity/monuments off).
	win_criteria {
		population {enabled : true, goal : 2000 }
		culture    {enabled : false }
		prosperity {enabled : false }
		monuments  {enabled : false }
		kingdom    {enabled : true, goal : 80 }
	}

	enable_scenario_events : true

	invasion_points_land [ [58, 107], [93, 71], [87, 31], [42, 16] ]

	// Empire sells/buys from mission1.pak empire_cities (raw dump, no JS overlay).
	// Kebet is not a partner on this scenario; Nekhen is. All routes start closed (cost>0).
	cities [
		{
			name : "Men-nefer"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CHICKPEAS, RESOURCE_POTTERY, RESOURCE_PAPYRUS ]
			buys [ RESOURCE_BRICKS, RESOURCE_BEER, RESOURCE_GEMS, RESOURCE_LUXURY_GOODS ]
		}

		{
			name : "Abu"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_GRAIN, RESOURCE_STRAW, RESOURCE_FLAX, RESOURCE_LINEN, RESOURCE_GEMS ]
			buys [ RESOURCE_POTTERY, RESOURCE_PAPYRUS ]
		}

		{
			name : "Behdet"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_FISH, RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_REEDS, RESOURCE_PAPYRUS ]
			buys [ RESOURCE_BRICKS, RESOURCE_LINEN, RESOURCE_GEMS, RESOURCE_LUXURY_GOODS ]
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
			name : "Selima Oasis"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_LUXURY_GOODS, RESOURCE_TIMBER ]
			buys [ RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_LINEN, RESOURCE_PAPYRUS, RESOURCE_COPPER ]
		}
	]

	vars {
		pharaoh_requested_copper1 : false
		pharaoh_requested_copper2 : false
		pharaoh_requested_weapons1 : false
		pharaoh_requested_luxury_goods : false
		pharaoh_requested_weapons2 : false
		libyan_invasion_1 : false
		libyan_invasion_2 : false
		libyan_invasion_3 : false
		libyan_invasion_4 : false
		libyan_invasion_5 : false
		libyan_invasion_6 : false
		libyan_invasion_7 : false
		start_message_shown : false
	}
}

[es=event_mission_start, mission=mission11]
function mission11_on_start(ev) {
	__image_request_pak(PACK_ENEMY_LIBIAN)
	mission_show_start_message(mission, "message_mission_serabit_khadim")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

[es=(city_animals, create_herds), mission=mission11]
function mission11_register_animals(ev) {
	city.remove_animals()

	city.add_animals_point(0, /*x*/55, /*y*/75, FIGURE_OSTRICH, 5)
	city.set_animals_area(0, 16)

	city.add_animals_point(1, /*x*/85, /*y*/135, FIGURE_OSTRICH, 5)
	city.set_animals_area(1, 16)
}

function mission11_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

// pak: year=1 month=7 copper 8 / 18mo
[es=event_advance_month, mission=mission11]
function mission11_pharaoh_request_copper1(ev) {
	if (mission.pharaoh_requested_copper1) {
		return
	}
	if (ev.years_since_start < 1 || (ev.years_since_start == 1 && ev.month < 7)) {
		return
	}
	mission.pharaoh_requested_copper1 = true
	mission11_fire_request(1, RESOURCE_COPPER, 8, 18, 101, 102, 5, 10)
}

// pak: year=3 month=4 copper 11 / 18mo
[es=event_advance_month, mission=mission11]
function mission11_pharaoh_request_copper2(ev) {
	if (mission.pharaoh_requested_copper2) {
		return
	}
	if (ev.years_since_start < 3 || (ev.years_since_start == 3 && ev.month < 4)) {
		return
	}
	mission.pharaoh_requested_copper2 = true
	mission11_fire_request(2, RESOURCE_COPPER, 11, 18, 201, 202, 6, 8)
}

// pak: year=5 month=9 weapons 13 / 18mo
[es=event_advance_month, mission=mission11]
function mission11_pharaoh_request_weapons1(ev) {
	if (mission.pharaoh_requested_weapons1) {
		return
	}
	if (ev.years_since_start < 5 || (ev.years_since_start == 5 && ev.month < 9)) {
		return
	}
	mission.pharaoh_requested_weapons1 = true
	mission11_fire_request(3, RESOURCE_WEAPONS, 13, 18, 301, 302, 5, 10)
}

// pak: year=7 month=0 luxury_goods 16 / 24mo
[es=event_advance_month, mission=mission11]
function mission11_pharaoh_request_luxury_goods(ev) {
	if (mission.pharaoh_requested_luxury_goods) {
		return
	}
	if (ev.years_since_start < 7) {
		return
	}
	mission.pharaoh_requested_luxury_goods = true
	mission11_fire_request(4, RESOURCE_LUXURY_GOODS, 16, 24, 401, 402, 6, 8)
}

// pak: year=8 month=2 weapons 21 / 12mo
[es=event_advance_month, mission=mission11]
function mission11_pharaoh_request_weapons2(ev) {
	if (mission.pharaoh_requested_weapons2) {
		return
	}
	if (ev.years_since_start < 8 || (ev.years_since_start == 8 && ev.month < 2)) {
		return
	}
	mission.pharaoh_requested_weapons2 = true
	mission11_fire_request(5, RESOURCE_WEAPONS, 21, 12, 501, 502, 5, 10)
}

function mission11_libyan_raid(invasion_id, size) {
	// Scenario enemy_id in pak is ENEMY_7_LIBIAN. No invasion land points in pak → auto tile.
	city.start_foreign_army_invasion({
		invasion_id: invasion_id,
		enemy: ENEMY_7_LIBIAN,
		size: size,
		tilex: -1,
		tiley: -1,
		want_destroy_buildings: size
	})
}

// pak invasions (timed; scenario enemy = Libyan). Extra chain-triggered invasions await B2.
[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_1(ev) {
	if (mission.libyan_invasion_1) { return }
	if (ev.years_since_start < 1 || (ev.years_since_start == 1 && ev.month < 10)) { return }
	mission.libyan_invasion_1 = true
	log_info("akhenaten: mission 11 libyan invasion 1 size=8", {ev:ev})
	mission11_libyan_raid(0, 8)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_2(ev) {
	if (mission.libyan_invasion_2) { return }
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 4)) { return }
	mission.libyan_invasion_2 = true
	log_info("akhenaten: mission 11 libyan invasion 2 size=16", {ev:ev})
	mission11_libyan_raid(1, 16)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_3(ev) {
	if (mission.libyan_invasion_3) { return }
	if (ev.years_since_start < 3 || (ev.years_since_start == 3 && ev.month < 3)) { return }
	mission.libyan_invasion_3 = true
	log_info("akhenaten: mission 11 libyan invasion 3 size=12", {ev:ev})
	mission11_libyan_raid(2, 12)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_4(ev) {
	if (mission.libyan_invasion_4) { return }
	if (ev.years_since_start < 4 || (ev.years_since_start == 4 && ev.month < 8)) { return }
	mission.libyan_invasion_4 = true
	log_info("akhenaten: mission 11 libyan invasion 4 size=20", {ev:ev})
	mission11_libyan_raid(3, 20)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_5(ev) {
	if (mission.libyan_invasion_5) { return }
	if (ev.years_since_start < 6) { return }
	mission.libyan_invasion_5 = true
	log_info("akhenaten: mission 11 libyan invasion 5 size=28", {ev:ev})
	mission11_libyan_raid(4, 28)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_6(ev) {
	if (mission.libyan_invasion_6) { return }
	if (ev.years_since_start < 8 || (ev.years_since_start == 8 && ev.month < 6)) { return }
	mission.libyan_invasion_6 = true
	log_info("akhenaten: mission 11 libyan invasion 6 size=28", {ev:ev})
	mission11_libyan_raid(5, 28)
}

[es=event_advance_month, mission=mission11]
function mission11_libyan_invasion_7(ev) {
	if (mission.libyan_invasion_7) { return }
	if (ev.years_since_start < 10 || (ev.years_since_start == 10 && ev.month < 1)) { return }
	mission.libyan_invasion_7 = true
	log_info("akhenaten: mission 11 libyan invasion 7 size=32", {ev:ev})
	mission11_libyan_raid(6, 32)
}
