log_info("akhenaten: main menu started")

var game_updater_windows_url =
    "https://nightly.link/dalerank/Akhenaten/workflows/akhenaten_windows/master/windows_build.zip"

function main_menu_local_build_number() {
	var v = game.version
	var marker = " b"
	var i = v.indexOf(marker)
	if (i < 0) {
		return 0
	}
	return parseInt(v.substring(i + marker.length)) || 0
}

function main_menu_update_panel_pos() {
	// outer_panel size[20, 27] → 320x432; keep clear of centered menu buttons
	return [(sw(0) - px(20)) / 2, (sh(0) - px(27)) / 2]
}

function main_menu_dismiss_update(window) {
	window.update_panel.enabled = false
	window.update_game.enabled = false
	window.new_version.enabled = false
	window.changelog.enabled = false
	window.update_later.enabled = false
	window.update_status.enabled = false
}

function main_menu_update_now(window) {
	if (__platform_is_windows()) {
		window.update_status.text = "Downloading update..."
	} else {
		window.update_status.text = "Opening download page..."
	}
	window.update_status.enabled = true
	__game_download_latest_version(game_updater_windows_url)
}

[es=window]
window_main_menu {
	ui {
		background    : { type:"background", path:"pharaoh_unloaded/title_00001" }

		continue_game : large_button({ pos:mbutton(0), size[256, 25], text[13, 5], onclick: main_menu_continue_game })
		select_player : large_button({ pos:mbutton(1), size[256, 25], text[30, 0], onclick: show_window_by_id("window_player_selection") })
		show_records  : large_button({ pos:mbutton(2), size[256, 25], text[30, 5], onclick: show_window_by_id("records_window") })
		show_config   : large_button({ pos:mbutton(3), size[256, 25], text[2,  0], onclick: show_window_by_id("window_features") })
		show_mods     : large_button({ pos:mbutton(4), size[256, 25], text:"#main_menu_mods", onclick: show_window_by_id("mods_window") })
		show_editor   : large_button({ pos:mbutton(5), size[256, 25], text:"#main_menu_editor", onclick: main_menu_start_editor })
		quit_game     : large_button({ pos:mbutton(6), size[256, 25], text[30, 4], onclick: main_menu_quit_game })

		discord 	  : image_button({ pos[sw(-100), sh(-50)], size[48, 48], icon_texture:"!discord", scale:0.75
							           	onclick: function() { __platform_open_url("https://discord.gg/HS4njmBvpb") }
		 							 })

		patreon 	  : image_button({ pos[sw(-50), sh(-50)], size[48, 48], icon_texture:":patreon_48.png", scale:0.75
			                            onclick: function() { __platform_open_url("https://www.patreon.com/imspinner") }
		                             })
		version_number: text({pos[18, sh(-30)], text: game.version, font: FONT_SMALL_PLAIN, color: 0xffb3b3b3})

		update_panel  : outer_panel({ pos: main_menu_update_panel_pos(), size[20, 27], enabled:false,
			ui {
				update_game : large_button({ pos[32, 16], size[256, 25], text:"update now", enabled: false
					                         onclick: function() { main_menu_update_now(window_main_menu) }
				                           })
				update_later : large_button({ pos[32, 48], size[256, 25], text:"later", enabled: false
					                          onclick: function() { main_menu_dismiss_update(window_main_menu) }
				                            })
				new_version : text({pos[18, 84], text: game.version, font: FONT_SMALL_PLAIN, enabled: false})
				update_status : text({pos[18, 104], size[280, 20], text:"", font: FONT_SMALL_PLAIN, enabled: false})
				changelog : text({pos[18, 124], size[280, 280], wrap:px(17), rich:true, text:"Loading changelog...", font: FONT_SMALL_PLAIN, enabled: false, clip_area: true})
			}
		})
	}
}

function main_menu_can_continue() {
	var last_save = normalize_savegame_path_for_load(game_features.gameopt_last_save_filename)
	var last_player = game_features.gameopt_last_player
	if (!last_save || !last_player) {
		return false
	}
	return __game_file_exists(last_save)
}

function main_menu_continue_game() {
	var last_save = normalize_savegame_path_for_load(game_features.gameopt_last_save_filename)
    var last_player = game_features.gameopt_last_player
    if (last_save && last_player) {
        game_features.gameopt_player_name = last_player
        if (__game_load_savegame(last_save)) {
            ui.window_city_show()
        }
    }
}

function main_menu_quit_game() {
    ui.show_yesno("#popup_dialog_quit", function() {
		emit event_request_exit{ value: true }
	})
}

function main_menu_start_editor() {
    game.init_editor()
}

[es=event_show_main_menu]
function main_menu_on_show(ev) {
    if (ev.play_intro) {
        __game_sound.play_intro()
    }

    game_mission_options_locked = false
    window_show_by_id("window_main_menu")
}

[es=(window_main_menu, init)]
function main_menu_on_init(window) {
    log_info("[test-marker] main_menu_shown")

    __sound_city_stop()
    __sound_city_init()

    window.continue_game.readonly = !main_menu_can_continue()

	if (!game.is_integral_tests && github_is_active()) {
		github_download_changelog_async()
		github_get_total_commits_async("dalerank", "Akhenaten")
	}
}

[es=(window_main_menu, update_changelog)]
function main_menu_on_update(window) {
	window.changelog.text = window.change_log
	window.changelog.enabled = true
}

[es=(window_main_menu, update_version)]
function main_menu_on_update_version(window) {
	log_info("main_menu_download_version: " + window.current_commit)
	if (window.current_commit <= 1)
		return

	var local_build = main_menu_local_build_number()
	if (window.current_commit <= local_build)
		return

	if (game_features.gameopt_last_game_version == window.current_commit)
		return

	window.update_panel.enabled = true
	window.new_version.enabled = true
	window.update_game.enabled = true
	window.update_later.enabled = true
	window.new_version.text = "New build: " + window.current_commit + " (you have " + local_build + ")"

	game_features.gameopt_last_game_version = window.current_commit
}

[es=event_github_totals_commits_loaded]
function main_menu_download_version(ev) {
	emit window_main_menu.update_version{ current_commit: ev.current_commit }
}

[es=event_github_changelog_loaded]
function main_menu_download_changelog(ev) {
	log_info("main_menu_download_changelog: " + ev.change_log)
	emit window_main_menu.update_changelog{ change_log: ev.change_log }
}
