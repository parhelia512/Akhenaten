log_info("akhenaten: ui top menu config started")

function top_menu_autosave_options_text(p1, p2) { return __loc(19, game_features.gameopt_monthly_autosave ? 51 : 52) }
function top_menu_autosave_options_toggle(p1, p2) { game_features.gameopt_monthly_autosave = !game_features.gameopt_monthly_autosave }

function top_menu_autosave_slots_clamp(n) {
	n = Math.round(n)
	if (n < 1) return 1
	if (n > 10) return 10
	return n
}

function top_menu_autosave_slots_text(p1, p2) {
	var n = top_menu_autosave_slots_clamp(game_features.gameopt_autosave_slots)
	return __loc("#autosave_slots") + ": " + n
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

function top_menu_tooltip_text(p1, p2) { return __loc(3, game_features.gameopt_tooltips_mode + 2) }
function top_menu_tooltip_toggle(p1, p2) { game_features.gameopt_tooltips_mode = (game_features.gameopt_tooltips_mode + 1) % 3 }

function top_menu_warnings_text(p1, p2) { return __loc(3, game_features.gameopt_warnings ? 6 : 5) }
function top_menu_warnings_toggle(p1, p2) { game_features.gameopt_warnings = !game_features.gameopt_warnings }

function top_menu_cities_old_text(p1, p2) { return __loc(2, game_features.gameui_empire_city_old_names ? 7 : 8) }
function top_menu_cities_old_toggle(p1, p2) { game_features.gameui_empire_city_old_names = !game_features.gameui_empire_city_old_names }

function top_menu_pyramid_speedup_text(p1, p2) { return __loc(2, game_features.gameopt_pyramid_speedup ? 5 : 4) }
function top_menu_pyramid_speedup_toggle(p1, p2) { game_features.gameopt_pyramid_speedup = !game_features.gameopt_pyramid_speedup }

function top_menu_clear_state() {
	top_menu_widget.open_sub_menu = ""
	top_menu_widget.focus_menu_id = ""
	top_menu_widget.focus_sub_menu_id = ""
	top_menu_widget.item_clicked = 0
}

function top_menu_dismiss() {
	var open = ui.window_is("top_menu_submenu")
	top_menu_clear_state()
	if (open) {
		window_go_back()
	}
}

function widget_top_menu_clear_state() {
	top_menu_clear_state()
}

function top_menu_format_date(year, month) {
	var month_str = __loc(25, month)
	if (year >= 0) {
		return game.locale_year_before_ad
			? _format("{0} {1} AD", month_str, year)
			: _format("{0} AD {1}", month_str, year)
	}
	return _format("{0} {1} BC", month_str, -year)
}

function top_menu_refresh_status_text() {
	top_menu_widget.population_str = __loc(6, 1) + " " + city.population
	top_menu_widget.date_str = top_menu_format_date(game.simtime.year, game.simtime.month)
}

[es=event_level_post_load]
function top_menu_on_level_post_load(ev) {
	top_menu_clear_state()
	top_menu_refresh_status_text()
}

function top_menu_open_advisor(advisor, p2) {
	top_menu_dismiss()
	window_advisors_show_advisor(advisor)
}

function top_menu_show_console(p1, p2) {
	top_menu_dismiss()
	window_show_cheat_console(true)
}

function top_menu_js_debugger_text(p1, p2) {
	return __js_debugger_is_running()
		? "JS debugger ON"
		: "JS debugger OFF"
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
function top_menu_debug_properties_text(p1, p2) { return game.debug_properties ? "Properties ON" : "Properties OFF" }
function top_menu_debug_properties_toggle(p1, p2) {
	game.debug_properties = !game.debug_properties
	top_menu_dismiss()
}
function top_menu_debug_terrain_paint_text(p1, p2) { return game.debug_terrain_paint ? "Terrain paint ON" : "Terrain paint OFF" }
function top_menu_debug_terrain_paint_toggle(p1, p2) {
	game.debug_terrain_paint = !game.debug_terrain_paint
	if (!game.debug_terrain_paint) {
		__editor_tool_deactivate()
	}
	top_menu_dismiss()
}
function top_menu_debug_write_video_text(p1, p2) { return video_capture.active ? "Write Video ON" : "Write Video OFF" }

function top_menu_debug_buildings_text(p1, p2) { return game.debug_render_mode_name == "building" ? "Buildings ON" : "Buildings OFF" }
function top_menu_debug_buildings_toggle(p1, p2) {
	game.debug_render_mode_name = (game.debug_render_mode_name == "building") ? "" : "building"
}

function top_menu_date_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_date_display", -1) }
function top_menu_population_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_population_display", -1) }
function top_menu_funds_explanation(p1, p2) { ui.window_message_dialog_show("message_game_control_money_display_window", -1) }

function top_menu_show_window_by_id(window_id) {
	return function() {
		top_menu_dismiss()
		emit event_show_window{ id:window_id }
	}
}

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

function top_menu_resolve_text(text) {
	if (text === undefined || text === null) {
		return ""
	}
	if (typeof text === "string") {
		if (text.indexOf("${loc.") === 0) {
			return __loc("#" + text.substring(6, text.length - 1))
		}
		return __loc(text)
	}
	if (typeof text === "object" && text.group !== undefined) {
		return __loc(text.group, text.id)
	}
	return ""
}

function top_menu_item_text(item) {
	if (!item) {
		return ""
	}
	if (item.textfn) {
		return item.textfn(item.parameter)
	}
	return top_menu_resolve_text(item.text)
}

function top_menu_find_menu(menu_id) {
	var menus = top_menu_widget.menus
	if (!menus) {
		return null
	}
	for (var i = 0; i < menus.length; i++) {
		if (menus[i].id == menu_id) {
			return menus[i]
		}
	}
	return null
}

function top_menu_find_item(menu, item_id) {
	if (!menu || !menu.items) {
		return null
	}
	for (var i = 0; i < menu.items.length; i++) {
		if (menu.items[i].id == item_id) {
			return menu.items[i]
		}
	}
	return null
}

function top_menu_apply_item_enabled() {
	var file = top_menu_find_menu("file")
	var new_game = top_menu_find_item(file, "new_game")
	if (new_game) {
		new_game.enabled = !game_features.gameui_hide_new_game_top_menu
	}

	var options = top_menu_find_menu("options")
	var display_options = top_menu_find_item(options, "display_options")
	if (display_options) {
		display_options.enabled = !screen.is_fullscreen_only
	}
}

function top_menu_header_at_mouse() {
	var menus = top_menu_widget.menus
	if (!menus) {
		return ""
	}
	var mx = __mouse.x
	var my = __mouse.y
	var y0 = top_menu_widget.offset.y
	var y1 = y0 + 12
	if (my < y0 || my >= y1) {
		return ""
	}
	for (var i = 0; i < menus.length; i++) {
		var menu = menus[i]
		if (menu.x_start <= mx && menu.x_end > mx) {
			return menu.id
		}
	}
	return ""
}

function top_menu_open_submenu(menu_id) {
	top_menu_widget.open_sub_menu = menu_id
	top_menu_apply_item_enabled()
	ui.set_tooltip("")
	if (!ui.window_is("top_menu_submenu")) {
		window_show_by_id("top_menu_submenu")
	}
}

function top_menu_activate_item(menu, item) {
	if (!item || item.enabled === false || item.hidden) {
		return
	}
	top_menu_widget.item_clicked = 1
	if (item.onclick) {
		item.onclick(item.parameter)
	} else if (menu && menu.onclick) {
		menu.onclick(item.parameter)
	}
}

function top_menu_draw_background() {
	var block_img = get_image(top_menu_widget.background)
	if (!block_img) {
		return
	}

	var want_sidebar_offset = top_menu_widget.sidebar_offset
	var block_width = block_img.width
	var screen_width = screen.width
	var current_sidebar_offset = ui.sidebar.offset_x

	var step = block_width - want_sidebar_offset
	if (step <= 0) {
		return
	}

	for (var x = -(screen_width - current_sidebar_offset); x < screen_width; x += step) {
		ui.draw_texture({ x: x, y: 0 }, block_img.tid)
	}

	ui.draw_texture({ x: current_sidebar_offset - block_width + want_sidebar_offset, y: 0 }, block_img.tid)

	var rotate_img = get_image({pack:PACK_GENERAL, id:136, offset:72 + top_menu_widget.rotate_hover })
	ui.draw_texture({ x: screen.width - 170, y: 0 }, rotate_img.tid)
}

function top_menu_draw_headers() {
	var menus = top_menu_widget.menus
	if (!menus) {
		return
	}

	var cur_x = top_menu_widget.offset.x
	var cur_y = top_menu_widget.offset.y
	var spacing = top_menu_widget.spacing
	var highlight = !!game_features.gameui_highlight_top_menu_hover
	var mx = __mouse.x
	var my = __mouse.y
	var y0 = top_menu_widget.offset.y
	var in_bar = (my >= y0 && my < y0 + 12)
	var focus_id = ""
	if (top_menu_widget.open_sub_menu) {
		focus_id = top_menu_widget.open_sub_menu
	}

	for (var i = 0; i < menus.length; i++) {
		var menu = menus[i]
		var text = top_menu_resolve_text(menu.text)
		var width = __ui_text_width(text, FONT_NORMAL_BLACK_ON_LIGHT)

		menu.x_start = cur_x
		menu.x_end = cur_x + width

		var is_hovered
		if (focus_id) {
			is_hovered = (menu.id == focus_id)
		} else {
			is_hovered = in_bar && menu.x_start <= mx && menu.x_end > mx
			if (is_hovered) {
				focus_id = menu.id
			}
		}

		var font = (is_hovered && highlight) ? FONT_NORMAL_YELLOW : FONT_NORMAL_BLACK_ON_LIGHT
		ui.label(text, { x: cur_x, y: cur_y }, font)

		if (is_hovered && menu.tooltip && !top_menu_widget.open_sub_menu) {
			ui.set_tooltip(top_menu_resolve_text(menu.tooltip))
		}

		cur_x += width + spacing
	}

	if (!top_menu_widget.open_sub_menu) {
		top_menu_widget.focus_menu_id = focus_id
	}
}

function top_menu_status_rect(right, w, h) {
	return { x: screen.width + right, y: top_menu_widget.offset.y, w: w, h: h }
}

function top_menu_point_in(mx, my, r) {
	return mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h
}

function top_menu_draw_status() {
	var treasury = city.finance.treasury
	var funds_font = treasury >= 0 ? FONT_NORMAL_BLACK_ON_LIGHT : FONT_NORMAL_BLUE
	var funds_color = treasury < 0 ? COLOR_FONT_RED : COLOR_WHITE
	var funds_text = __loc(6, 0) + " " + treasury

	var date_r = top_menu_status_rect(-110, 117, 20)
	var pop_r = top_menu_status_rect(-310, 117, 20)
	var funds_r = top_menu_status_rect(-440, 117, 20)
	var rot_l = top_menu_status_rect(-174, 14, 24)
	var rot_m = top_menu_status_rect(-160, 14, 24)
	var rot_r = top_menu_status_rect(-144, 14, 24)

	var mx = __mouse.x
	var my = __mouse.y
	top_menu_widget.rotate_hover = 0
	if (top_menu_point_in(mx, my, rot_l)) {
		top_menu_widget.rotate_hover = 2
	} else if (top_menu_point_in(mx, my, rot_m)) {
		top_menu_widget.rotate_hover = 1
	} else if (top_menu_point_in(mx, my, rot_r)) {
		top_menu_widget.rotate_hover = 3
	}

	if (top_menu_point_in(mx, my, date_r)) {
		ui.set_tooltip(__loc(68, 63))
	} else if (top_menu_point_in(mx, my, pop_r)) {
		ui.set_tooltip(__loc(68, 62))
	} else if (top_menu_point_in(mx, my, funds_r)) {
		ui.set_tooltip(__loc(68, 61))
	}

	ui.label(top_menu_widget.date_str, { x: date_r.x, y: date_r.y }, FONT_NORMAL_BLACK_ON_LIGHT)
	ui.label(top_menu_widget.population_str, { x: pop_r.x, y: pop_r.y }, FONT_NORMAL_BLACK_ON_LIGHT)
	if (treasury < 0) {
		ui.label_colored(funds_text, { x: funds_r.x, y: funds_r.y }, funds_font, funds_color)
	} else {
		ui.label(funds_text, { x: funds_r.x, y: funds_r.y }, funds_font)
	}
}

function top_menu_handle_status() {
	var date_r = top_menu_status_rect(-110, 117, 20)
	var pop_r = top_menu_status_rect(-310, 117, 20)
	var funds_r = top_menu_status_rect(-440, 117, 20)
	var rot_l = top_menu_status_rect(-174, 14, 24)
	var rot_m = top_menu_status_rect(-160, 14, 24)
	var rot_r = top_menu_status_rect(-144, 14, 24)
	var mx = __mouse.x
	var my = __mouse.y

	if (top_menu_point_in(mx, my, rot_l) && __mouse.left.went_up) {
		emit event_rotate_map{ value: HOTKEY_ROTATE_MAP_LEFT }
		return 1
	}
	if (top_menu_point_in(mx, my, rot_m) && __mouse.left.went_up) {
		emit event_rotate_map_reset{ value: 0 }
		return 1
	}
	if (top_menu_point_in(mx, my, rot_r) && __mouse.left.went_up) {
		emit event_rotate_map{ value: HOTKEY_ROTATE_MAP_RIGHT }
		return 1
	}
	if (top_menu_point_in(mx, my, date_r) && __mouse.right.went_up) {
		top_menu_date_explanation()
		return 1
	}
	if (top_menu_point_in(mx, my, pop_r) && __mouse.right.went_up) {
		top_menu_population_explanation()
		return 1
	}
	if (top_menu_point_in(mx, my, funds_r) && __mouse.right.went_up) {
		top_menu_funds_explanation()
		return 1
	}
	return 0
}

function top_menu_calc_menu_size(menu) {
	var max_width = 0
	var height_pixels = top_menu_widget.item_height
	var items = menu.items || []
	for (var i = 0; i < items.length; i++) {
		var item = items[i]
		if (item.hidden || item.enabled === false) {
			continue
		}
		var text = top_menu_item_text(item)
		var width_pixels = __ui_text_width(text, FONT_NORMAL_BLACK_ON_LIGHT)
		if (width_pixels > max_width) {
			max_width = width_pixels
		}
		height_pixels += top_menu_widget.item_height
	}
	var blocks = ((max_width + 8) / 16 | 0) + 1
	if (blocks < 10) {
		blocks = 10
	}
	menu.calculated_width_blocks = blocks
	menu.calculated_height_blocks = (height_pixels / 16) | 0
	if (menu.calculated_height_blocks < 1) {
		menu.calculated_height_blocks = 1
	}
}

top_menu_widget {
	offset {x: 10, y: 6}
	submenu_offset {x: 8, y: -8}
	item_height : 20
	height : 30
	background { pack:PACK_GENERAL, id:121, offset:8 }
	sidebar_offset : 158
	spacing : 16
	open_sub_menu : ""
	focus_menu_id : ""
	focus_sub_menu_id : ""
	date_str : ""
	population_str : ""
	item_clicked : 0
	rotate_hover : 0

	menus [
		{
			id: "file"
			text: "#top_menu_file"
			tooltip: "#top_menu_file_tooltip"
			items [
				{ id: "new_game", text {group:1, id:1}, onclick: top_menu_new_game }
				{ id: "replay_map", text: "#replay_mission", onclick: top_menu_replay_map }
				{ id: "load_game", text {group:1, id:3}, onclick: top_menu_load_map }
				{ id: "save_game", text {group:1, id:4}, onclick: top_menu_save_map }
				{ id: "delete_game", text {group:1, id:6}, onclick: top_menu_delete_map }
				{ id: "exit_game", text {group:1, id:5}, onclick: top_menu_exit_game }
			]
		}
		{
			id: "options"
			text: "#top_menu_options"
			tooltip: "#top_menu_options_tooltip"
			items [
				{ id: "display_options", text {group:2, id:1}, onclick: top_menu_show_window_by_id("display_options_window") }
				{ id: "sound_options", text {group:2, id:2}, onclick: top_menu_show_window_by_id("sound_options_window") }
				{ id: "speed_options", text {group:2, id:3}, onclick: top_menu_show_window_by_id("speed_options_window") }
				{ id: "pyramid_speedup", textfn: top_menu_pyramid_speedup_text, onclick: top_menu_pyramid_speedup_toggle }
				{ id: "difficulty_options", text {group:2, id:6}, onclick: top_menu_show_window_by_id("difficulty_options_window") }
				{ id: "popup_messages", text {group:2, id:11}, onclick: top_menu_show_window_by_id("popup_messages_window") }
				{ id: "cities_options", textfn: top_menu_cities_old_text, onclick: top_menu_cities_old_toggle }
				{ id: "autosave_options", textfn: top_menu_autosave_options_text, onclick: top_menu_autosave_options_toggle }
				{ id: "autosave_slots", textfn: top_menu_autosave_slots_text, onclick: top_menu_autosave_slots_cycle }
				{ id: "hotkeys_options", text: "Hotkeys options", onclick: top_menu_show_window_by_id("window_hotkey_config") }
				{ id: "enhanced_options", text: "Enhanced options", onclick: top_menu_features }
			]
		}
		{
			id: "help"
			text: "#top_menu_help"
			tooltip: "#top_menu_help_tooltip"
			items [
				{ id: "help", text {group:3, id:1}, onclick: top_menu_show_help }
				{ id: "mouse", textfn: top_menu_tooltip_text, onclick: top_menu_tooltip_toggle }
				{ id: "warnings", textfn: top_menu_warnings_text, onclick: top_menu_warnings_toggle }
				{ id: "about", text {group:3, id:7}, onclick: top_menu_show_about }
			]
		}
		{
			id: "advisors"
			text: "#top_menu_overseers"
			tooltip: "#top_menu_overseers_tooltip"
			onclick: top_menu_open_advisor
			items [
				{ id: "advisor_labor", text {group:4, id:ADVISOR_LABOR}, parameter: ADVISOR_LABOR }
				{ id: "advisor_military", text {group:4, id:ADVISOR_MILITARY}, parameter: ADVISOR_MILITARY }
				{ id: "advisor_imperial", text {group:4, id:ADVISOR_IMPERIAL}, parameter: ADVISOR_IMPERIAL }
				{ id: "advisor_ratings", text {group:4, id:ADVISOR_RATINGS}, parameter: ADVISOR_RATINGS }
				{ id: "advisor_trade", text {group:4, id:ADVISOR_TRADE}, parameter: ADVISOR_TRADE }
				{ id: "advisor_population", text {group:4, id:ADVISOR_POPULATION}, parameter: ADVISOR_POPULATION }
				{ id: "advisor_health", text {group:4, id:ADVISOR_HEALTH}, parameter: ADVISOR_HEALTH }
				{ id: "advisor_education", text {group:4, id:ADVISOR_EDUCATION}, parameter: ADVISOR_EDUCATION }
				{ id: "advisor_entertainment", text {group:4, id:ADVISOR_ENTERTAINMENT}, parameter: ADVISOR_ENTERTAINMENT }
				{ id: "advisor_religion", text {group:4, id:ADVISOR_RELIGION}, parameter: ADVISOR_RELIGION }
				{ id: "advisor_financial", text {group:4, id:ADVISOR_FINANCIAL}, parameter: ADVISOR_FINANCIAL }
				{ id: "advisor_chief", text {group:4, id:ADVISOR_CHIEF}, parameter: ADVISOR_CHIEF }
			]
		}
		{
			id: "debug"
			text: "Debug"
			items [
				{ id: "properties", textfn: top_menu_debug_properties_text, onclick: top_menu_debug_properties_toggle }
				{ id: "terrain_paint", textfn: top_menu_debug_terrain_paint_text, onclick: top_menu_debug_terrain_paint_toggle }
				{ id: "make_screenshot", text: "Make screenshot", onclick: top_menu_make_screenshot }
				{ id: "make_full_screenshot", text: "Make full screenshot", onclick: top_menu_make_fullscreenshot }
				{ id: "write_video", textfn: top_menu_debug_write_video_text, onclick: video_capture.toggle }
				{ id: "show_console", text: "Cheat console", onclick: top_menu_show_console }
				{ id: "js_debugger", textfn: top_menu_js_debugger_text, onclick: top_menu_js_debugger_toggle }
			]
		}
		{
			id: "debug_render"
			text: "Render"
			items [
				{ id: "buildings", textfn: top_menu_debug_buildings_text, onclick: top_menu_debug_buildings_toggle }
			]
		}
	]
}

[es=modal_window]
top_menu_submenu {
	allow_rmb_goback: true
	ui {
		background : dummy({size[sw(0), sh(0)]})
	}
}

[es=top_menu_draw]
function top_menu_draw(ev) {
	if (top_menu_widget.open_sub_menu && !ui.window_is("top_menu_submenu")) {
		top_menu_clear_state()
	}
	top_menu_draw_background()
	top_menu_draw_headers()
	top_menu_draw_status()
}

function top_menu_handle_input() {
	if (__ui_screen_city_capture_input()) {
		return 0
	}

	if (top_menu_handle_status()) {
		return 1
	}

	var menu_id = top_menu_header_at_mouse()
	top_menu_widget.focus_menu_id = menu_id
	if (menu_id && __mouse.left.went_up) {
		top_menu_open_submenu(menu_id)
		return 1
	}
	return 0
}

[es=top_menu_handle_input]
function top_menu_handle_input_es(ev) {
	top_menu_handle_input()
}

[es=(top_menu_submenu, init)]
function top_menu_submenu_init(window) {
	top_menu_apply_item_enabled()
	top_menu_widget.item_clicked = 0
}

[es=(top_menu_submenu, go_back)]
function top_menu_submenu_go_back(window) {
	top_menu_dismiss()
}

[es=(top_menu_submenu, draw_background)]
function top_menu_submenu_draw_background(window) {
	__ui_window_city_draw_panels()
	top_menu_draw()
	__ui_window_city_draw()
	__ui_widget_sidebar_city_draw_foreground()
}

[es=(top_menu_submenu, ui_draw_foreground)]
function top_menu_submenu_ui_draw_foreground(window) {
	var menu = top_menu_find_menu(top_menu_widget.open_sub_menu)
	if (!menu) {
		return
	}

	top_menu_calc_menu_size(menu)
	__ui_unbordered_panel(menu.x_start, top_menu_widget.height, menu.calculated_width_blocks, menu.calculated_height_blocks)

	var sub = top_menu_widget.submenu_offset
	var y_offset = top_menu_widget.height + top_menu_widget.offset.y * 2 + sub.y
	var items = menu.items || []
	var focus_id = ""

	for (var i = 0; i < items.length; i++) {
		var item = items[i]
		if (item.hidden || item.enabled === false) {
			continue
		}

		var text = top_menu_item_text(item)
		var item_x = menu.x_start + sub.x
		var btn_pos = { x: menu.x_start, y: y_offset - 2 }
		var btn_size = { x: 16 * menu.calculated_width_blocks, y: 19 }
		var hovered = (__mouse.x >= btn_pos.x && __mouse.x < btn_pos.x + btn_size.x
			&& __mouse.y >= btn_pos.y && __mouse.y < btn_pos.y + btn_size.y)
		if (hovered) {
			focus_id = item.id
		}

		var font = (item.id == focus_id) ? FONT_NORMAL_YELLOW : FONT_NORMAL_BLACK_ON_LIGHT
		var clicked = ui.button({
			text: text,
			pos: [item_x, y_offset],
			size: [btn_size.x - sub.x, top_menu_widget.item_height],
			font: font,
			border: false,
			body: false,
			flags: UiFlags_AlignYCentered
		})
		if (clicked == ui.button_clicked) {
			top_menu_activate_item(menu, item)
			return
		}

		y_offset += top_menu_widget.item_height
	}

	top_menu_widget.focus_sub_menu_id = focus_id
}

[es=(top_menu_submenu, ui_handle_mouse)]
function top_menu_submenu_ui_handle_mouse(window) {
	if (__input_go_back_requested()) {
		top_menu_clear_state()
		return
	}

	var menu_id = top_menu_header_at_mouse()
	if (menu_id) {
		top_menu_widget.focus_menu_id = menu_id
		if (menu_id != top_menu_widget.open_sub_menu) {
			top_menu_widget.open_sub_menu = menu_id
			top_menu_apply_item_enabled()
		}
	}

	if (!__mouse.left.went_up) {
		return
	}

	if (top_menu_widget.item_clicked) {
		top_menu_widget.item_clicked = 0
		return
	}

	top_menu_dismiss()
}

[es=event_population_changed]
function top_menu_update_population_text(ev) {
	top_menu_widget.population_str = __loc(6, 1) + " " + ev.value
}

[es=event_advance_day]
function top_menu_update_date_text(ev) {
	top_menu_widget.date_str = top_menu_format_date(ev.year, ev.month)
}
