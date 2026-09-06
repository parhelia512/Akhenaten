log_info("akhenaten: figure bricklayer started")

figure_bricklayer {
	animations {
		_pack { pack:PACK_SPR_MAIN }
		walk { id:109, max_frames:12 }
		death { id:110, max_frames:8, loop:false }
		work { id:111, max_frames:12, duration:4 }
		idle { id:112, max_frames:8, duration:2 }
		big_image { pack:PACK_UNLOADED, id:25, offset:FIGURE_BRICKLAYER }
	}

	sounds {
		brick_bricklaying_time_at_monument { sound:"brick_e01.wav", text: "#brick_bricklaying_time_at_monument" }
		brick_monument_will_be_strong { sound:"brick_e02.wav", text: "#brick_monument_will_be_strong" }
	}

	category: figure_category_citizen
	max_damage: 10
	terrain_usage : TERRAIN_USAGE_ROADS,
}

[es=(figure_bricklayer, setup_phrase)]
function figure_bricklayer_setup_phrase(ev) {
	var f = city.get_figure(ev.fid)
	if (!f || !f.valid) {
		return
	}

	var state = f.action_state
	switch (state) {
	case ACTION_4_BRICKLAYER_LAY_BRICKS:
	case ACTION_12_BRICKLAYER_WORK_STATUE:
	case ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE:
	case ACTION_6_BRICKLAYER_RETURN_HOME:
	case ACTION_7_BRICKLAYER_EXIT_FROM_MONUMENT:
		figure_apply_phrase(f, "brick_monument_will_be_strong")
		return
	}

	figure_apply_phrase(f, "brick_bricklaying_time_at_monument")
}
