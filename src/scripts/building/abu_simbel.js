log_info("akhenaten: building abu_simbel started")

building_abu_simbel {
    animations {
      // PACK_ABUSIMBEL: progressive art stages (ids provisional until pak dump).
      // Within-pack orientation variants are a follow-up (same as sun temple).
      sa { pack:PACK_ABUSIMBEL, id:1 }
      sb { pack:PACK_ABUSIMBEL, id:2 }
      sc { pack:PACK_ABUSIMBEL, id:3 }
      sd { pack:PACK_ABUSIMBEL, id:4 }
      se { pack:PACK_ABUSIMBEL, id:5 }
      sf { pack:PACK_ABUSIMBEL, id:6 }
      sg { pack:PACK_ABUSIMBEL, id:7 }
      sh { pack:PACK_ABUSIMBEL, id:8 }
      preview { pack:PACK_ABUSIMBEL, id:1 }
    }
    // Heaven cliff bulk 9×21; 3×3 entrance protrudes past far depth (total ~9×24).
    building_size : 9
    init_tiles [9, 21]
    entrance_size [3, 3]
    art_stages : 8
    // AS0.6: Heaven/web only W=Y (no counts). Defaults also in C++ base_params.
    // Sequential: timber+carpenter phases, then mason-only carve (timber 0).
    timber_loads [ 400, 400, 400, 200, 0, 0, 0, 0 ]
    fire_proof : true
    damage_proof : true
    info_title_id [198, 37]
    meta { help_id: 492, text_id: 178 }
    info_sound : "Wavs/rock3.wav"
    cost [ 5000, 6500, 8000, 11000, 16000 ]
    flags {
        is_monument: true
        non_deletable: true
        allow_rotate: true
    }
}
