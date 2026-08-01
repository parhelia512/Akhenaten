log_info("akhenaten: building abu_simbel started")

building_abu_simbel {
    animations {
      // PACK_ABUSIMBEL (AbuSimbel.sg3) — verified group starts (system@0 + content@201):
      //   id1 @201 backing×4 | id2 @205 2statue×20 | id3 @203 (alt backing)
      //   id4 @225 midcut_back×14 | id5 @239 midcut_front×14 | id6 @253 stairs×6
      //   id7..14 @259..266 cliffs×8 (one group each) | id15 @267 scaffold×2 | id16 @269 scaffold×2
      // 2statue: 10 progress stages × 2 camera variants (even/odd). sa1..sh2 = stages 1..8.
      sa1 { pack:PACK_ABUSIMBEL, id:2, offset:0 }
      sa2 { pack:PACK_ABUSIMBEL, id:2, offset:1 }
      sb1 { pack:PACK_ABUSIMBEL, id:2, offset:2 }
      sb2 { pack:PACK_ABUSIMBEL, id:2, offset:3 }
      sc1 { pack:PACK_ABUSIMBEL, id:2, offset:4 }
      sc2 { pack:PACK_ABUSIMBEL, id:2, offset:5 }
      sd1 { pack:PACK_ABUSIMBEL, id:2, offset:6 }
      sd2 { pack:PACK_ABUSIMBEL, id:2, offset:7 }
      se1 { pack:PACK_ABUSIMBEL, id:2, offset:8 }
      se2 { pack:PACK_ABUSIMBEL, id:2, offset:9 }
      sf1 { pack:PACK_ABUSIMBEL, id:2, offset:10 }
      sf2 { pack:PACK_ABUSIMBEL, id:2, offset:11 }
      sg1 { pack:PACK_ABUSIMBEL, id:2, offset:12 }
      sg2 { pack:PACK_ABUSIMBEL, id:2, offset:13 }
      sh1 { pack:PACK_ABUSIMBEL, id:2, offset:14 }
      sh2 { pack:PACK_ABUSIMBEL, id:2, offset:15 }
      // Finished / ghost (last 2statue pair).
      preview { pack:PACK_ABUSIMBEL, id:2, offset:18 }
      finish1 { pack:PACK_ABUSIMBEL, id:2, offset:18 }
      finish2 { pack:PACK_ABUSIMBEL, id:2, offset:19 }

      // Layer bases (C++ adds progressive offsets).
      backing { pack:PACK_ABUSIMBEL, id:1 }
      midcut_back { pack:PACK_ABUSIMBEL, id:4 }
      midcut_front { pack:PACK_ABUSIMBEL, id:5 }
      stairs { pack:PACK_ABUSIMBEL, id:6 }
      cliff { pack:PACK_ABUSIMBEL, id:7 }
      scaffold_a { pack:PACK_ABUSIMBEL, id:15 }
      scaffold_b { pack:PACK_ABUSIMBEL, id:15, offset:1 }
      scaffold_c { pack:PACK_ABUSIMBEL, id:16 }
      scaffold_d { pack:PACK_ABUSIMBEL, id:16, offset:1 }
    }
    // Heaven cliff bulk 9×21; 3×3 entrance protrudes past far depth (total ~9×24).
    building_size : 9
    init_tiles [9, 21]
    entrance_size [3, 3]
    art_stages : 8
    // AS0.6: no counts in Heaven chart / AbuSimbel.sg3 / Pharaoh.exe (searched).
    // GIF-informed scaffold-heavy early phases; still provisional vs live OG.
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
