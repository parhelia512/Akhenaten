log_info("akhenaten: building royal_tomb started")

// Small Royal Burial Tomb — Heaven cliff bulk 11×20 + 1×1 clear entrance (chart tn=3035).
// Pack TombThut / TombThutFin; group ids provisional until pak dump (RT0.2).
building_small_royal_tomb {
    animations {
      sa { pack:PACK_TOMB_THUTMOS, id:1 }
      sb { pack:PACK_TOMB_THUTMOS, id:1 }
      sc { pack:PACK_TOMB_THUTMOS, id:1 }
      sd { pack:PACK_TOMB_THUTMOS, id:1 }
      se { pack:PACK_TOMB_THUTMOS, id:1 }
      sf { pack:PACK_TOMB_THUTMOS, id:1 }
      sg { pack:PACK_TOMB_THUTMOS, id:1 }
      sh { pack:PACK_TOMB_THUTMOS, id:1 }
      si { pack:PACK_TOMB_THUTMOS, id:1 }
      preview { pack:PACK_TOMB_THUTMOS, id:1 }
      finish { pack:PACK_TOMB_THUTMOS_FINAL, id:1 }
    }
    building_size : 11
    init_tiles [11, 20]
    entrance_size [1, 1]
    art_stages : 9
    // Help 478: 400 lamps before work. Later phase loads TODO(orig-data) vs My Palace.
    lamp_loads [ 400, 0, 0, 0, 0, 0, 0, 0, 0 ]
    fire_proof : true
    damage_proof : true
    info_title_id [198, 33]
    meta { help_id: 478, text_id: 178 }
    info_sound : "Wavs/rock3.wav"
    cost [ 2500, 3500, 4500, 6000, 8500 ]
    flags {
        is_monument: true
        allow_rotate: true
    }
}
