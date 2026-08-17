log_info("akhenaten: ui top menu actions started")

function top_menu_autosave_options_text(p1, p2) {
	return __loc(game_features.gameopt_monthly_autosave ? "#monthly_autosave_on" : "#monthly_autosave_off")
}
function top_menu_autosave_options_toggle(p1, p2) { game_features.gameopt_monthly_autosave = !game_features.gameopt_monthly_autosave }

function top_menu_autosave_slots_clamp(n) {
	n = Math.round(n)
	if (n < 1) return 1
	if (n > 10) return 10
	return n
}

function top_menu_autosave_slots_text(p1, p2) {
	var n = top_menu_autosave_slots_clamp(game_features.gameopt_autosave_slots)
	return fmt("#autosave_slots: ${n}", { n: n })
}

function top_menu_autosave_slots_cycle(p1, p2) {
	var cur = top_menu_autosave_slots_clamp(game_features.gameopt_autosave_slots)
	var opts = [1, 3, 5, 10]
	var next = opts[0]
	for (var i = 0; i < opts.length; i++) {
		if (cur === opts[i]) {
			next = opts[(i + 1) % opts.length]
			break
		}
		if (cur < opts[i]) {
			next = opts[i]
			break
		}
	}
	game_features.gameopt_autosave_slots = next
}

function top_menu_tooltip_text(p1, p2) {
	var keys = ["#top_menu_mouse_help_off", "#top_menu_mouse_help_some", "#top_menu_mouse_help_full"]
	return __loc(keys[game_features.gameopt_tooltips_mode] || keys[0])
}
function top_menu_tooltip_toggle(p1, p2) { game_features.gameopt_tooltips_mode = (game_features.gameopt_tooltips_mode + 1) % 3 }

function top_menu_warnings_text(p1, p2) {
	return __loc(game_features.gameopt_warnings ? "#top_menu_warnings_on" : "#top_menu_warnings_off")
}
function top_menu_warnings_toggle(p1, p2) { game_features.gameopt_warnings = !game_features.gameopt_warnings }

function top_menu_cities_old_text(p1, p2) {
	return __loc(game_features.gameui_empire_city_old_names ? "#top_menu_cities_egyptian" : "#top_menu_cities_classical")
}
function top_menu_cities_old_toggle(p1, p2) { game_features.gameui_empire_city_old_names = !game_features.gameui_empire_city_old_names }

function top_menu_pyramid_speedup_text(p1, p2) {
	return __loc(game_features.gameopt_pyramid_speedup ? "#top_menu_pyramid_speedup_on" : "#top_menu_pyramid_speedup_off")
}
function top_menu_pyramid_speedup_toggle(p1, p2) { game_features.gameopt_pyramid_speedup = !game_features.gameopt_pyramid_speedup }

function top_menu_open_advisor(advisor, p2) {
	top_menu_dismiss()
	window_advisors_show_advisor(advisor)
}

function top_menu_show_console(p1, p2) {
	top_menu_dismiss()
	window_show_cheat_console(true)
}

function top_menu_js_debugger_text(p1, p2) {
	return __loc(__js_debugger_is_running()
		? "#top_menu_js_debugger_on"
		: "#top_menu_js_debugger_off")
}

function top_menu_js_debugger_toggle(p1, p2) {
	top_menu_dismiss()
	if (__js_debugger_is_running())
		__js_debugger_stop()
	else
		__js_debugger_start(4711)
}

function top_menu_make_fullscreenshot(p1, p2) {
	top_menu_dismiss()
	__game_save_screenshot(SCREENSHOT_FULL_CITY)
}

function top_menu_make_screenshot(p1, p2) {
	top_menu_dismiss()
	__game_save_screenshot(SCREENSHOT_DISPLAY)
}

function top_menu_debug_properties_text(p1, p2) {
	return __loc(game.debug_properties ? "#top_menu_properties_on" : "#top_menu_properties_off")
}

function top_menu_debug_properties_toggle(p1, p2) {
	game.debug_properties = !game.debug_properties
	top_menu_dismiss()
}

function top_menu_debug_terrain_paint_text(p1, p2) {
	return __loc(game.debug_terrain_paint ? "#top_menu_terrain_paint_on" : "#top_menu_terrain_paint_off")
}

function top_menu_debug_terrain_paint_toggle(p1, p2) {
	game.debug_terrain_paint = !game.debug_terrain_paint
	if (!game.debug_terrain_paint) {
		__editor_tool_deactivate()
	}
	top_menu_dismiss()
}

function top_menu_debug_write_video_text(p1, p2) {
	return __loc(video_capture.active ? "#top_menu_write_video_on" : "#top_menu_write_video_off")
}

function top_menu_debug_buildings_text(p1, p2) {
	return __loc(game.debug_render_mode_name == "building" ? "#top_menu_buildings_on" : "#top_menu_buildings_off")
}

function top_menu_debug_buildings_toggle(p1, p2) {
	game.debug_render_mode_name = (game.debug_render_mode_name == "building") ? "" : "building"
}

function top_menu_date_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_date_display", -1) }
function top_menu_population_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_population_display", -1) }
function top_menu_funds_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_money_display_window", -1) }

function top_menu_new_game() {
	top_menu_dismiss()
	ui.show_yesno("#popup_dialog_quit",
		function() {
			__ui_city_planner_reset()
			__game_undo_disable()

			game_mission_options_locked = false
			window_show_by_id("window_dinasty_menu")
		},
		function() {
			ui.window_city_show()
		}
	)
}

function top_menu_replay_map() {
	top_menu_dismiss()
	ui.show_yesno("#replay_mission",
		function() {
			__ui_city_planner_reset()
			var is_custom = scenario.scmode != e_scenario_normal
			if (is_custom) {
				__game_load_savegame("autosave_replay.svx")
				ui.window_city_show()
			} else {
				top_menu_clear_state()
				city.kingdome.campaign_carry_personal_savings = 0
				city.kingdome.personal_savings = 0
				__campaign_carry_clear()
				__game_load_mission(scenario.campaign_scenario_id, 1)
			}
		},
		function() {
			ui.window_city_show()
		}
	)
}

function top_menu_load_map() {
	if (!game_allows_midgame_load()) {
		top_menu_dismiss()
		game_toast_ironwill_load_blocked()
		return
	}
	top_menu_dismiss()
	__ui_city_planner_reset()
	ui.window_city_show()
	window_show_by_id("file_dialog_load")
}

function top_menu_save_map() {
	if (!game_allows_player_save()) {
		top_menu_dismiss()
		game_toast_ironwill_save_blocked()
		return
	}
	top_menu_dismiss()
	ui.window_city_show()
	window_show_by_id("file_dialog_save")
}

function top_menu_delete_map() {
	top_menu_dismiss()
	ui.window_city_show()
	window_show_by_id("file_dialog_delete")
}

function top_menu_exit_game() {
	top_menu_dismiss()
	game_confirm_exit_to_main_menu()
}

function top_menu_features(p1, p2) {
	top_menu_dismiss()
	window_show_by_id("window_features")
}

function top_menu_show_help(p1, p2) {
	top_menu_dismiss()
	ui.window_message_dialog("message_dialog_help")
}

function top_menu_show_about(p1, p2) {
	top_menu_dismiss()
	ui.window_message_dialog("message_dialog_about")
}
