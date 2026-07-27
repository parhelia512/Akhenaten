log_info("akhenaten: building mastaba started")

building_small_stepped_pyramid {
    animations {
      _pack { pack:PACK_STEPPED_PYRAMID }
      preview { id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { path:"stepped_pyramid/stepped_pyramid_00103" }
      corner_bricks { path:"stepped_pyramid/stepped_pyramid_00001" }
      wall_bricks { path:"stepped_pyramid/stepped_pyramid_00005" }
      base_grounded { path:"stepped_pyramid/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"stepped_pyramid/pyramid_phase_one_00021" }
      empty_land {path:"stepped_pyramid/stepped_pyramid_00109"}
      ditches_phase_1 { path:"stepped_pyramid/pyramid_phase_one_00022" }
      ditches_phase_2 { path:"stepped_pyramid/pyramid_phase_one_00031" }
      ditches_phase_3 { path:"stepped_pyramid/pyramid_phase_one_00040" }
      ditches_phase_4 { path:"stepped_pyramid/pyramid_phase_one_00049" }
      ground_phase_0 { path:"stepped_pyramid/pyramid_phase_one_00013" }
      basement { path:"stepped_pyramid/pyramid_phase_one_00058" }
      enter { path:"pharaoh_general/plazatiles_00064"}
    }
    build_menu_text : "Small Stepped Pyramid"
    building_size : 2
    info_title_id [198, 18]
    fire_proof :  true
    damage_proof : true
    meta { help_id:375, text_id:178 }
    init_tiles [8, 8]

    flags {
      is_monument: true
    }

    enter_offset : [1, 8]
    stair_0_0_offset : [2, 8]
    stair_0_1_offset : [4, 8]
    stair_0_4_offset : [6, 8]
    stair_0_4_corner_offset : [6, 6]
    stair_0_5_offset : [6, 5]
    stair_0_6_offset : [6, 4]

    corner_type : BUILDING_SMALL_STEPPED_PYRAMID_CORNER
    wall_type : BUILDING_SMALL_STEPPED_PYRAMID_WALL
    cone_type : BUILDING_SMALL_STEPPED_PYRAMID_CONE
    filler_type : BUILDING_SMALL_STEPPED_PYRAMID
    // todo
  }

  // Parts are 2×2 blocks on the init_tiles grid — without building_size they stay
  // size 0 and never claim map tiles (edges/corners missing). Alias main so they
  // also inherit brick animations used by get_bricks_image.
  building_small_stepped_pyramid_corner = building_small_stepped_pyramid
  building_small_stepped_pyramid_wall = building_small_stepped_pyramid
  building_small_stepped_pyramid_cone = building_small_stepped_pyramid

  building_medium_stepped_pyramid {
    animations {
      _pack { pack:PACK_STEPPED_PYRAMID }
      preview { pack:PACK_STEPPED_PYRAMID, id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { path:"stepped_pyramid/stepped_pyramid_00103" }
      corner_bricks { path:"stepped_pyramid/stepped_pyramid_00001" }
      wall_bricks { path:"stepped_pyramid/stepped_pyramid_00005" }
      base_grounded { path:"stepped_pyramid/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"stepped_pyramid/pyramid_phase_one_00021" }
      empty_land {path:"stepped_pyramid/stepped_pyramid_00109"}
      ditches_phase_1 { path:"stepped_pyramid/pyramid_phase_one_00022" }
      ditches_phase_2 { path:"stepped_pyramid/pyramid_phase_one_00031" }
      ditches_phase_3 { path:"stepped_pyramid/pyramid_phase_one_00040" }
      ditches_phase_4 { path:"stepped_pyramid/pyramid_phase_one_00049" }
      ground_phase_0 { path:"stepped_pyramid/pyramid_phase_one_00013" }
      basement { path:"stepped_pyramid/pyramid_phase_one_00058" }
      enter { path:"pharaoh_general/plazatiles_00064"}
      stair_0_2 { path:"stepped_pyramid/stepped_pyramid_00112" }
      stair_0_4 { path:"stepped_pyramid/stepped_pyramid_00110" }
      stair_0_4_corner { path:"stepped_pyramid/stepped_pyramid_00122" }
      stair_0_5 { path:"stepped_pyramid/stepped_pyramid_00114" }
      stair_0_6 { path:"stepped_pyramid/stepped_pyramid_00115" }
    }
    build_menu_text : "Medium Stepped Pyramid"
    building_size : 2
    info_title_id [198, 18]
    fire_proof :  true
    damage_proof : true
    meta { help_id:375, text_id:178 }
    init_tiles [12, 12]

    enter_offset : [2, 12]

    stairs [
      {
        phase : 7
        part : [2, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [-15, 25]
      }

      {
        phase : 8
        part : [4, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [-15, 25]
      }

      {
        phase : 9
        part : [6, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [-15, 25]
      }

      {
        phase : 10
        part : [8, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [15, -35]
      }

      {
        phase : 10
        part : [10, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00122" }
        offset : [15, -40]
      }

      {
        phase : 11
        part : [10, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [25, -30]
      }

      {
        phase : 12
        part : [10, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00115" }
        offset : [20, -30]
      }

      {
        phase : 13
        part : [10, 4]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [0, -90]
      }

      {
        phase : 13
        part : [10, 2]
        tex { path:"stepped_pyramid/stepped_pyramid_00124" }
        offset : [0, -95]
      }

      {
        phase : 16
        part : [4, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00118" }
        offset : [-28, -155]
      }

      {
        phase : 17
        part : [6, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [-48, -158]
      }

      {
        phase : 18
        part : [8, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [-48, -158]
      }

      {
        phase : 19
        part : [8, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [0, -180]
      }

      {
        phase : 20
        part : [8, 4]
        tex { path:"stepped_pyramid/stepped_pyramid_00124" }
        offset : [-0, -180]
      }

      {
        phase : 21
        part : [4, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00119" }
        offset : [-20, -175]
      }

      {
        phase : 21
        part : [6, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [-40, -175]
      }

      {
        phase : 22
        part : [6, 4]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [10, -200]
      }
    ]

    flags {
        is_monument: true
    }

    corner_type : BUILDING_MEDIUM_STEPPED_PYRAMID_CORNER
    wall_type : BUILDING_MEDIUM_STEPPED_PYRAMID_WALL
    cone_type : BUILDING_MEDIUM_STEPPED_PYRAMID_CONE
    filler_type : BUILDING_MEDIUM_STEPPED_PYRAMID
    // todo
  }

  building_medium_stepped_pyramid_corner = building_medium_stepped_pyramid
  building_medium_stepped_pyramid_wall = building_medium_stepped_pyramid
  building_medium_stepped_pyramid_cone = building_medium_stepped_pyramid

  // Large stepped pyramid (20x20, id 250). Same PACK_STEPPED_PYRAMID render pipeline as
  // small/medium; only footprint (init_tiles) differs. Stairs: ascending ramp from
  // enter_offset up the south face to the tier-1 ledge (one layer ≈ TILE_HEIGHT*3 = 90px).
  building_large_stepped_pyramid {
    animations {
      _pack { pack:PACK_STEPPED_PYRAMID }
      preview { pack:PACK_STEPPED_PYRAMID, id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { path:"stepped_pyramid/stepped_pyramid_00103" }
      corner_bricks { path:"stepped_pyramid/stepped_pyramid_00001" }
      wall_bricks { path:"stepped_pyramid/stepped_pyramid_00005" }
      base_grounded { path:"stepped_pyramid/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"stepped_pyramid/pyramid_phase_one_00021" }
      empty_land {path:"stepped_pyramid/stepped_pyramid_00109"}
      ditches_phase_1 { path:"stepped_pyramid/pyramid_phase_one_00022" }
      ditches_phase_2 { path:"stepped_pyramid/pyramid_phase_one_00031" }
      ditches_phase_3 { path:"stepped_pyramid/pyramid_phase_one_00040" }
      ditches_phase_4 { path:"stepped_pyramid/pyramid_phase_one_00049" }
      ground_phase_0 { path:"stepped_pyramid/pyramid_phase_one_00013" }
      basement { path:"stepped_pyramid/pyramid_phase_one_00058" }
      enter { path:"pharaoh_general/plazatiles_00064"}
      stair_0_2 { path:"stepped_pyramid/stepped_pyramid_00112" }
      stair_0_4 { path:"stepped_pyramid/stepped_pyramid_00110" }
      stair_0_4_corner { path:"stepped_pyramid/stepped_pyramid_00122" }
      stair_0_5 { path:"stepped_pyramid/stepped_pyramid_00114" }
      stair_0_6 { path:"stepped_pyramid/stepped_pyramid_00115" }
    }
    build_menu_text : "Large Stepped Pyramid"
    building_size : 2
    info_title_id [198, 18]
    fire_proof :  true
    damage_proof : true
    meta { help_id:375, text_id:178 }
    init_tiles [20, 20]

    enter_offset : [2, 20]

    // Medium-style L-ramp: short approach on south from enter, SE corner, then
    // climb the east face (decreasing y) up to the tier-1 ledge. Sprites 00114+
    // are the ascending ramp pieces; offset.y rises ~+28 → -110 (≈ one tier).
    stairs [
      {
        phase : 7
        part : [4, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [-20, 20]
      }

      {
        phase : 8
        part : [6, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [-25, 20]
      }

      {
        phase : 9
        part : [8, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [-20, 20]
      }

      {
        phase : 10
        part : [12, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [0, -40]
      }

      {
        phase : 11
        part : [14, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [-5, -40]
      }

      {
        phase : 12
        part : [16, 18]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [-5, -45]
      }

      {
        phase : 13
        part : [18, 14]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [10, -100]
      }

      {
        phase : 14
        part : [18, 12]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [0, -100]
      }

      {
        phase : 15
        part : [18, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00115" }
        offset : [-5, -100]
      }

      {
        phase : 16
        part : [18, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [0, -170]
      }

      {
        phase : 17
        part : [18, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [0, -170]
      }

      {
        phase : 18
        part : [18, 4]
        tex { path:"stepped_pyramid/stepped_pyramid_00125" }
        offset : [-5, -170]
      }

      {
        phase : 19
        part : [12, 16]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [15, -200]
      }

      {
        phase : 20
        part : [14, 16]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [10, -200]
      }

      {
        phase : 20
        part : [16, 16]
        tex { path:"stepped_pyramid/stepped_pyramid_00123" }
        offset : [15, -205]
      }

      {
        phase : 21
        part : [16, 14]
        tex { path:"stepped_pyramid/stepped_pyramid_00115" }
        offset : [20, -200]
      }

      {
        phase : 22
        part : [14, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [20, -210]
      }

      {
        phase : 23
        part : [14, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [20, -210]
      }

      // --- Layer 2 (third tier): L2 south y=14 (begin+4, size 12) → SE [14,14].
      // Offsets ~two tiers up (≈ -180..-270).
      {
        phase : 24
        part : [14, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00115" }
        offset : [15, -210]
      }

      {
        phase : 24
        part : [14, 4]
        tex { path:"stepped_pyramid/stepped_pyramid_00127" }
        offset : [10, -220]
      }

      {
        phase : 25
        part : [8, 14]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [0, -300]
      }

      {
        phase : 26
        part : [10, 14]
        tex { path:"stepped_pyramid/stepped_pyramid_00111" }
        offset : [0, -300]
      }

      {
        phase : 27
        part : [12, 14]
        tex { path:"stepped_pyramid/stepped_pyramid_00112" }
        offset : [0, -300]
      }

      {
        phase : 28
        part : [12, 10]
        tex { path:"stepped_pyramid/stepped_pyramid_00113" }
        offset : [20, -300]
      }

      {
        phase : 29
        part : [12, 8]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [15, -300]
      }

      {
        phase : 30
        part : [12, 6]
        tex { path:"stepped_pyramid/stepped_pyramid_00127" }
        offset : [15, -300]
      }

      {
        phase : 31
        part : [10, 12]
        tex { path:"stepped_pyramid/stepped_pyramid_00110" }
        offset : [0, -390]
      }

      {
        phase : 32
        part : [12, 12]
        tex { path:"stepped_pyramid/stepped_pyramid_00122" }
        offset : [0, -400]
      }

      {
        phase : 33
        part : [10, 12]
        tex { path:"stepped_pyramid/stepped_pyramid_00114" }
        offset : [0, -400]
      }
    ]

    flags {
        is_monument: true
    }

    corner_type : BUILDING_LARGE_STEPPED_PYRAMID_CORNER
    wall_type : BUILDING_LARGE_STEPPED_PYRAMID_WALL
    cone_type : BUILDING_LARGE_STEPPED_PYRAMID_CONE
    filler_type : BUILDING_LARGE_STEPPED_PYRAMID
  }

  building_large_stepped_pyramid_corner = building_large_stepped_pyramid
  building_large_stepped_pyramid_wall = building_large_stepped_pyramid
  building_large_stepped_pyramid_cone = building_large_stepped_pyramid

  // --- Bent pyramids (Snofru's, mission 14 South Dahshur) --------------------
  // The bent_pyramid image pack is structurally identical to stepped_pyramid
  // (159 "Bent_pyramid_*" main sprites at the same indices as "stepped_pyramid_*",
  // plus the shared "pyramid phase one_*" and "Pyramid buildings_*" groups). So the
  // config mirrors the stepped one with the pack + two path stems swapped, which
  // yields the real bent-pyramid art through the shared stepped render pipeline.
  building_small_bent_pyramid {
    animations {
      _pack { pack:PACK_BENT_PYRAMID }
      preview { id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { path:"bent_pyramid/Bent_pyramid_00103" }
      corner_bricks { path:"bent_pyramid/Bent_pyramid_00001" }
      wall_bricks { path:"bent_pyramid/Bent_pyramid_00005" }
      base_grounded { path:"bent_pyramid/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"bent_pyramid/pyramid_phase_one_00021" }
      empty_land {path:"bent_pyramid/Bent_pyramid_00109"}
      ditches_phase_1 { path:"bent_pyramid/pyramid_phase_one_00022" }
      ditches_phase_2 { path:"bent_pyramid/pyramid_phase_one_00031" }
      ditches_phase_3 { path:"bent_pyramid/pyramid_phase_one_00040" }
      ditches_phase_4 { path:"bent_pyramid/pyramid_phase_one_00049" }
      ground_phase_0 { path:"bent_pyramid/pyramid_phase_one_00013" }
      basement { path:"bent_pyramid/pyramid_phase_one_00058" }
      enter { path:"pharaoh_general/plazatiles_00064"}
    }
    build_menu_text : "Small Bent Pyramid"
    building_size : 2
    info_title_id [198, 1]
    fire_proof :  true
    damage_proof : true
    meta { help_id:375, text_id:178 }
    init_tiles [8, 8]

    flags {
      is_monument: true
    }

    enter_offset : [1, 8]
    stair_0_0_offset : [2, 8]
    stair_0_1_offset : [4, 8]
    stair_0_4_offset : [6, 8]
    stair_0_4_corner_offset : [6, 6]
    stair_0_5_offset : [6, 5]
    stair_0_6_offset : [6, 4]

    corner_type : BUILDING_SMALL_BENT_PYRAMID_CORNER
    wall_type : BUILDING_SMALL_BENT_PYRAMID_WALL
    cone_type : BUILDING_SMALL_BENT_PYRAMID_CONE
    filler_type : BUILDING_SMALL_BENT_PYRAMID
  }

  building_small_bent_pyramid_corner = building_small_bent_pyramid
  building_small_bent_pyramid_wall = building_small_bent_pyramid
  building_small_bent_pyramid_cone = building_small_bent_pyramid

  building_medium_bent_pyramid {
    animations {
      _pack { pack:PACK_BENT_PYRAMID }
      preview { pack:PACK_BENT_PYRAMID, id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { path:"bent_pyramid/Bent_pyramid_00103" }
      corner_bricks { path:"bent_pyramid/Bent_pyramid_00001" }
      wall_bricks { path:"bent_pyramid/Bent_pyramid_00005" }
      base_grounded { path:"bent_pyramid/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"bent_pyramid/pyramid_phase_one_00021" }
      empty_land {path:"bent_pyramid/Bent_pyramid_00109"}
      ditches_phase_1 { path:"bent_pyramid/pyramid_phase_one_00022" }
      ditches_phase_2 { path:"bent_pyramid/pyramid_phase_one_00031" }
      ditches_phase_3 { path:"bent_pyramid/pyramid_phase_one_00040" }
      ditches_phase_4 { path:"bent_pyramid/pyramid_phase_one_00049" }
      ground_phase_0 { path:"bent_pyramid/pyramid_phase_one_00013" }
      basement { path:"bent_pyramid/pyramid_phase_one_00058" }
      enter { path:"pharaoh_general/plazatiles_00064"}
      stair_0_2 { path:"bent_pyramid/Bent_pyramid_00112" }
      stair_0_4 { path:"bent_pyramid/Bent_pyramid_00110" }
      stair_0_4_corner { path:"bent_pyramid/Bent_pyramid_00122" }
      stair_0_5 { path:"bent_pyramid/Bent_pyramid_00114" }
      stair_0_6 { path:"bent_pyramid/Bent_pyramid_00115" }
    }
    build_menu_text : "Medium Bent Pyramid"
    building_size : 2
    info_title_id [198, 2]
    fire_proof :  true
    damage_proof : true
    meta { help_id:375, text_id:178 }
    init_tiles [12, 12]

    enter_offset : [2, 12]

    stairs [
      {
        phase : 7
        part : [2, 10]
        tex { path:"bent_pyramid/Bent_pyramid_00110" }
        offset : [-15, 25]
      }

      {
        phase : 8
        part : [4, 10]
        tex { path:"bent_pyramid/Bent_pyramid_00111" }
        offset : [-15, 25]
      }

      {
        phase : 9
        part : [6, 10]
        tex { path:"bent_pyramid/Bent_pyramid_00112" }
        offset : [-15, 25]
      }

      {
        phase : 10
        part : [8, 10]
        tex { path:"bent_pyramid/Bent_pyramid_00110" }
        offset : [15, -35]
      }

      {
        phase : 10
        part : [10, 10]
        tex { path:"bent_pyramid/Bent_pyramid_00122" }
        offset : [15, -40]
      }

      {
        phase : 11
        part : [10, 8]
        tex { path:"bent_pyramid/Bent_pyramid_00114" }
        offset : [25, -30]
      }

      {
        phase : 12
        part : [10, 6]
        tex { path:"bent_pyramid/Bent_pyramid_00115" }
        offset : [20, -30]
      }

      {
        phase : 13
        part : [10, 4]
        tex { path:"bent_pyramid/Bent_pyramid_00113" }
        offset : [0, -90]
      }

      {
        phase : 13
        part : [10, 2]
        tex { path:"bent_pyramid/Bent_pyramid_00124" }
        offset : [0, -95]
      }

      {
        phase : 16
        part : [4, 8]
        tex { path:"bent_pyramid/Bent_pyramid_00118" }
        offset : [-28, -155]
      }

      {
        phase : 17
        part : [6, 8]
        tex { path:"bent_pyramid/Bent_pyramid_00111" }
        offset : [-48, -158]
      }

      {
        phase : 18
        part : [8, 8]
        tex { path:"bent_pyramid/Bent_pyramid_00112" }
        offset : [-48, -158]
      }

      {
        phase : 19
        part : [8, 6]
        tex { path:"bent_pyramid/Bent_pyramid_00113" }
        offset : [0, -180]
      }

      {
        phase : 20
        part : [8, 4]
        tex { path:"bent_pyramid/Bent_pyramid_00124" }
        offset : [-0, -180]
      }

      {
        phase : 21
        part : [4, 6]
        tex { path:"bent_pyramid/Bent_pyramid_00119" }
        offset : [-20, -175]
      }

      {
        phase : 21
        part : [6, 6]
        tex { path:"bent_pyramid/Bent_pyramid_00112" }
        offset : [-40, -175]
      }

      {
        phase : 22
        part : [6, 4]
        tex { path:"bent_pyramid/Bent_pyramid_00113" }
        offset : [10, -200]
      }
    ]

    flags {
        is_monument: true
    }

    corner_type : BUILDING_MEDIUM_BENT_PYRAMID_CORNER
    wall_type : BUILDING_MEDIUM_BENT_PYRAMID_WALL
    cone_type : BUILDING_MEDIUM_BENT_PYRAMID_CONE
    filler_type : BUILDING_MEDIUM_BENT_PYRAMID
  }

  building_medium_bent_pyramid_corner = building_medium_bent_pyramid
  building_medium_bent_pyramid_wall = building_medium_bent_pyramid
  building_medium_bent_pyramid_cone = building_medium_bent_pyramid