log_info("akhenaten: mission 5 timna started")

// Trade / requests / invasions verified vs mission1.pak scenario 5 (2026-07-24 dump).
// Stub events/attacks/trade_routes removed — they were never wired (enable_scenario_events:false).

mission5 { // Timna
	start_message : "message_history_military"
	selection_title : "Timna"
	env {
		has_animals : false
		marshland_grow : default_marshland_grow
	    tree_grow : default_tree_grow
		hide_nilometer : true
	}

	sounds {
		briefing : "Voice/Mission/205_mission.mp3"
		victory : "Voice/Mission/205_victory.mp3"
	}

	player_rank : 2

	choice_background {pack:PACK_UNLOADED, id:12}
	choice_image1 {pack:PACK_UNLOADED, id:13}
	choice_image1_pos [192, 144]
	choice_title [144, 19]

	choice [
		{
			name : "Behdet"
			id : 6
			image {pack:PACK_UNLOADED, id:20}
			tooltip [144, 20]
			pos [640, 480]
		}

		{
			name : "Abydos"
			id : 7
			image {pack:PACK_UNLOADED, id:20}
			tooltip [144, 21]
			pos [620, 420]
		}
	]

	initial_funds [7500, 5000, 3750, 2500, 2000]
	rescue_loans [7500, 5000, 3750, 2500, 2000]
	house_tax_multipliers [300, 200, 150, 100, 75]

	buildings [
		         	BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_GARDENS, BUILDING_PLAZA,
					BUILDING_ROADBLOCK, BUILDING_FIREHOUSE, BUILDING_ARCHITECT_POST, BUILDING_POLICE_STATION, BUILDING_VILLAGE_PALACE,
					BUILDING_TAX_COLLECTOR, BUILDING_COURTHOUSE, BUILDING_PERSONAL_MANSION,
					BUILDING_WATER_SUPPLY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN,
					BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL, BUILDING_BANDSTAND, BUILDING_CONSERVATORY, BUILDING_PAVILLION, BUILDING_DANCE_SCHOOL,
					BUILDING_BAZAAR, BUILDING_GRANARY, BUILDING_STORAGE_YARD,
					BUILDING_RECRUITER, BUILDING_FORT_INFANTRY, BUILDING_FORT_ARCHERS, BUILDING_WEAPONSMITH,
					BUILDING_SCRIBAL_SCHOOL, BUILDING_CLAY_PIT, BUILDING_GEMSTONE_MINE, BUILDING_GOLD_MINE, BUILDING_COPPER_MINE, BUILDING_POTTERY_WORKSHOP,
					BUILDING_WEAVER_WORKSHOP, BUILDING_HUNTING_LODGE, BUILDING_TEMPLE_SETH, BUILDING_SHRINE_SETH, BUILDING_FESTIVAL_SQUARE,

					BUILDING_MORTUARY, BUILDING_STONEMASONS_GUILD, BUILDING_CARPENTERS_GUILD
				]

	win_criteria {
		population    {enabled : true, goal : 2000 }
		prosperity    {enabled : true, goal : 15 }
		kingdom       {enabled : true, goal : 70 }
		housing_level {enabled : true, goal : 10 }
	}

	stages {
		tutorial_irrigation { buildings: [BUILDING_WATER_LIFT, BUILDING_IRRIGATION_DITCH, ] }
		tutorial_guilds { buildings: [BUILDING_STORAGE_YARD, BUILDING_TAX_COLLECTOR, BUILDING_BOOTH, BUILDING_JUGGLER_SCHOOL] }
	}

	// Empire from pak (land routes, start closed). Men-nefer is NOT a partner here.
	cities [
		{
			name : "Nekhen"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CLAY, RESOURCE_POTTERY, RESOURCE_BEER, RESOURCE_FLAX, RESOURCE_LINEN ]
			buys [ RESOURCE_PAPYRUS ]
		}

		{
			name : "Nubt"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_GAMEMEAT ]
		}

		{
			name : "Thinis"
			is_sea_trade : false
			max_traders : 1
			trade_limits : default_trade_limits
			sells [ RESOURCE_CHICKPEAS, RESOURCE_POTTERY, RESOURCE_BEER ]
			buys [ RESOURCE_COPPER ]
		}
	]

	vars {
		pharaoh_requested_copper : false
		pharaoh_requested_gems : false
		pharaoh_requested_deben : false
		pharaoh_requested_weapons : false
		libyan_invasion_1 : false
		libyan_invasion_2 : false
		libyan_invasion_3 : false
		pharaoh_favour_invasion_done : false
		start_message_shown : false
	}
}

function mission5_fire_request(tag, resource, amount, months, ok_tag, fail_tag, ok_amt, fail_amt) {
	var request = city.create_good_request({ tag_id: tag, resource: resource, amount: amount, months_initial: months })
	city.create_chain_event({ tag_id: ok_tag, type: EVENT_TYPE_REPUTATION_INCREASE, amount: ok_amt })
	city.create_chain_event({ tag_id: fail_tag, type: EVENT_TYPE_REPUTATION_DECREASE, amount: fail_amt })
	request.set_completed_action_tag(ok_tag)
	request.set_refusal_action_tag(fail_tag)
	request.execute()
}

function mission5_libyan_raid(invasion_id, size) {
	city.start_foreign_army_invasion({
		invasion_id: invasion_id,
		enemy: ENEMY_7_LIBIAN,
		size: size,
		tilex: -1,
		tiley: -1,
		want_destroy_buildings: size
	})
}

[es=event_mission_start, mission=mission5]
function mission5_on_start(ev) {
	__image_request_pak(PACK_ENEMY_LIBIAN)
	mission_show_start_message(mission, "message_history_military")
	city.set_empire_available(1)
	for (var i = ADVISOR_NONE + 1; i <= ADVISOR_DIPLOMACY; i++) {
		city.set_advisor_available(i, 1)
	}
}

// pak: year=0 month=8 copper 5 / 9mo
[es=event_advance_month, mission=mission5]
function mission5_pharaoh_request_copper(ev) {
	if (mission.pharaoh_requested_copper) {
		return
	}
	if (ev.years_since_start == 0 && ev.month < 8) {
		return
	}
	mission.pharaoh_requested_copper = true
	mission5_fire_request(1, RESOURCE_COPPER, 5, 9, 101, 102, 4, 2)
}

// pak: year=2 month=4 gems 15 / 12mo
[es=event_advance_month, mission=mission5]
function mission5_pharaoh_request_gems(ev) {
	if (mission.pharaoh_requested_gems) {
		return
	}
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 4)) {
		return
	}
	mission.pharaoh_requested_gems = true
	mission5_fire_request(2, RESOURCE_GEMS, 15, 12, 201, 202, 4, 3)
}

// pak: year=2 month=8 item=31 amount=885 / 12mo. Id 31 is C3 denarii / money slot;
// Cleopatra remapped that id to RESOURCE_OIL — briefing + amount say treasury deben.
[es=event_advance_month, mission=mission5]
function mission5_pharaoh_request_deben(ev) {
	if (mission.pharaoh_requested_deben) {
		return
	}
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 8)) {
		return
	}
	mission.pharaoh_requested_deben = true
	mission5_fire_request(3, RESOURCE_DEBEN, 885, 12, 301, 302, 4, 2)
}

// pak: year=7 month=3 weapons 11 / 12mo
[es=event_advance_month, mission=mission5]
function mission5_pharaoh_request_weapons(ev) {
	if (mission.pharaoh_requested_weapons) {
		return
	}
	if (ev.years_since_start < 7 || (ev.years_since_start == 7 && ev.month < 3)) {
		return
	}
	mission.pharaoh_requested_weapons = true
	mission5_fire_request(4, RESOURCE_WEAPONS, 11, 12, 401, 402, 4, 3)
}

// pak invasions (scenario enemy = Libyan). Favour-KR Pharaoh army amount=45 via JS helper.
[es=event_advance_month, mission=mission5]
function mission5_libyan_invasion_1(ev) {
	if (mission.libyan_invasion_1) { return }
	if (ev.years_since_start < 2 || (ev.years_since_start == 2 && ev.month < 7)) { return }
	mission.libyan_invasion_1 = true
	log_info("akhenaten: mission 5 timna libyan invasion 1 size=9", {ev:ev})
	mission5_libyan_raid(0, 9)
}

[es=event_advance_month, mission=mission5]
function mission5_libyan_invasion_2(ev) {
	if (mission.libyan_invasion_2) { return }
	if (ev.years_since_start < 3 || (ev.years_since_start == 3 && ev.month < 9)) { return }
	mission.libyan_invasion_2 = true
	log_info("akhenaten: mission 5 timna libyan invasion 2 size=16", {ev:ev})
	mission5_libyan_raid(1, 16)
}

[es=event_advance_month, mission=mission5]
function mission5_libyan_invasion_3(ev) {
	if (mission.libyan_invasion_3) { return }
	if (ev.years_since_start < 5) { return }
	mission.libyan_invasion_3 = true
	log_info("akhenaten: mission 5 timna libyan invasion 3 size=24", {ev:ev})
	mission5_libyan_raid(2, 24)
}

[es=event_advance_month, mission=mission5]
function mission5_pharaoh_favour_invasion(ev) {
	mission_pharaoh_favour_invasion_tick(mission, 45)
}
