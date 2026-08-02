function mission_pharaoh_favour_invasion_outcome(seq) {
	if (!seq) {
		return -1
	}
	var n = city.invasion_history_count()
	for (var i = 0; i < n; i++) {
		var h = city.invasion_history_at(i)
		if (h.seq == seq) {
			return h.outcome
		}
	}
	return -1
}

// Favour-KR Pharaoh army waves. Call each month.
//   mission_pharaoh_favour_invasion_tick(mission, 51)
//   mission_pharaoh_favour_invasion_tick(mission, 25, 60)           // 2 waves (legacy)
//   mission_pharaoh_favour_invasion_tick(mission, [50, 20, 50])     // N waves
//   mission_pharaoh_favour_invasion_tick(mission, [20,20,20,20,20], { targets: [..., FOOD] })
// First wave when rating_kingdom <= 0; later waves after prior wave bind COMPLETED
// (B2-migrate). Formation poll remains only as mid-save / no-seq fallback.
function mission_pharaoh_favour_invasion_tick(mission, army_size_or_waves, chain_or_opts) {
	var waves
	var targets = null
	if (army_size_or_waves && typeof army_size_or_waves.length === "number") {
		waves = army_size_or_waves
		if (chain_or_opts && chain_or_opts.targets) {
			targets = chain_or_opts.targets
		}
	} else {
		waves = [army_size_or_waves]
		if (typeof chain_or_opts === "number" && chain_or_opts > 0) {
			waves.push(chain_or_opts)
		}
	}
	if (!waves || waves.length == 0) {
		return
	}

	var next = mission.pharaoh_favour_wave_next
	if (typeof next !== "number" || next < 0) {
		// Mid-save / unset (-1): prefer waveN_done (N-wave), else chain_done (2-wave).
		next = mission_pharaoh_favour_remigrate_next(mission, waves.length)
		mission.pharaoh_favour_wave_next = next
		if (!mission.pharaoh_favour_enemies_seen) {
			mission_pharaoh_favour_remigrate_enemies_seen(mission, next)
		}
	}
	if (next >= waves.length) {
		return
	}

	if (next == 0) {
		if (city.rating_kingdom > 0) {
			return
		}
		mission.pharaoh_favour_invasion_done = true
		mission.pharaoh_favour_enemies_seen = false
		mission.pharaoh_favour_chain_done = (waves.length < 2)
		mission.pharaoh_favour_wave_next = 1
		mission.pharaoh_favour_wave_seq = mission_pharaoh_favour_fire_wave(
			waves[0], 24, targets ? targets[0] : undefined)
		return
	}

	var ready = false
	var seq = mission.pharaoh_favour_wave_seq | 0
	if (seq > 0) {
		var outcome = mission_pharaoh_favour_invasion_outcome(seq)
		if (outcome == 1) {
			ready = true
		} else if (outcome == 2 || outcome == 3) {
			mission.pharaoh_favour_wave_next = waves.length
			return
		}
	} else {
		// Mid-save without seq: legacy any-enemy formation poll.
		if (city.num_enemy_formations > 0) {
			mission.pharaoh_favour_enemies_seen = true
			return
		}
		if (!mission.pharaoh_favour_enemies_seen) {
			return
		}
		ready = true
	}
	if (!ready) {
		return
	}

	mission.pharaoh_favour_enemies_seen = false
	if (next == 1) {
		mission.pharaoh_favour_chain_done = true
	}
	// Mirror into legacy waveN_done (N-wave missions remigrate from these if wave_next lost).
	mission_pharaoh_favour_mark_wave_done(mission, next)
	var size = waves[next]
	mission.pharaoh_favour_wave_next = next + 1
	mission.pharaoh_favour_wave_seq = mission_pharaoh_favour_fire_wave(
		size, 24 + next, targets ? targets[next] : undefined)
}

function mission_pharaoh_favour_mark_wave_done(mission, wave_index) {
	// wave_index is 0-based; local polls used wave2_done for the second wave.
	if (wave_index == 1) {
		mission.pharaoh_favour_wave2_done = true
	} else if (wave_index == 2) {
		mission.pharaoh_favour_wave3_done = true
	} else if (wave_index == 3) {
		mission.pharaoh_favour_wave4_done = true
	} else if (wave_index == 4) {
		mission.pharaoh_favour_wave5_done = true
	}
}

function mission_pharaoh_favour_remigrate_next(mission, wave_count) {
	if (!mission.pharaoh_favour_invasion_done) {
		return 0
	}
	var next = 1
	if (mission.pharaoh_favour_wave2_done) { next = 2 }
	if (mission.pharaoh_favour_wave3_done) { next = 3 }
	if (mission.pharaoh_favour_wave4_done) { next = 4 }
	if (mission.pharaoh_favour_wave5_done) { next = 5 }
	// 2-wave legacy: chain_done without wave2_done means the chain finished.
	if (next == 1 && mission.pharaoh_favour_chain_done && !mission.pharaoh_favour_wave2_done) {
		return wave_count
	}
	if (next > wave_count) {
		return wave_count
	}
	return next
}

function mission_pharaoh_favour_remigrate_enemies_seen(mission, next) {
	// next = index of wave waiting to fire; prior wave's enemies_seen lived in wave{N}_enemies_seen.
	if (next == 1 && mission.pharaoh_favour_wave2_enemies_seen) {
		mission.pharaoh_favour_enemies_seen = true
	} else if (next == 2 && mission.pharaoh_favour_wave3_enemies_seen) {
		mission.pharaoh_favour_enemies_seen = true
	} else if (next == 3 && mission.pharaoh_favour_wave4_enemies_seen) {
		mission.pharaoh_favour_enemies_seen = true
	} else if (next == 4 && mission.pharaoh_favour_wave5_enemies_seen) {
		mission.pharaoh_favour_enemies_seen = true
	}
}

function mission_pharaoh_favour_fire_wave(size, invasion_id, attack_target) {
	if (attack_target === undefined) {
		attack_target = EVENT_ATTACK_TARGET_RANDOM
	}
	log_info("akhenaten: pharaoh favour invasion size=" + size + " id=" + invasion_id + " kr=" + city.rating_kingdom)
	__image_request_pak(PACK_ENEMY_EGYPTIAN)
	return city.start_foreign_army_invasion({
		mode: ATTACK_TYPE_ENEMIES,
		enemy: ENEMY_3_EGYPTIAN,
		kind: INVASION_KIND_KINGDOME,
		size: size,
		invasion_id: invasion_id,
		tilex: -1,
		tiley: -1,
		want_destroy_buildings: 0,
		invasion_attack_target: attack_target
	})
}
