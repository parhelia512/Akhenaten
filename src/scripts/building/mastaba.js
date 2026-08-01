log_info("akhenaten: building mastaba started")

building_small_mastaba {
    animations {
      _pack { pack:PACK_MASTABA }
      preview { id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { id:1, offset:0 }
      base_grounded { path:"mastaba/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"pharaoh_general/plazatiles_00064" }
      empty_land { path:"mastaba/mastaba_00109" }
      enter { id:114 }
    }
    building_size : 2
    info_title_id[198, 18]
    fire_proof :  true
    damage_proof : true
    meta { help_id:4, text_id:178 }
    info_sound : "Wavs/rock3.wav"
    init_tiles [10, 4]

    flags {
        is_monument: true
        allow_rotate: true
    }

    config_north [
        { type: BUILDING_SMALL_MASTABA,      offset[-1, -1], base:true }, { type: BUILDING_SMALL_MASTABA,      offset[2, 0] },  
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, 2] },              { type: BUILDING_SMALL_MASTABA_WALL, offset[2, 2] },
        { type: BUILDING_SMALL_MASTABA_ENTRANCE, offset[2, 4]},           { type: BUILDING_SMALL_MASTABA_WALL, offset[0, 4] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, 6] },              { type: BUILDING_SMALL_MASTABA_WALL, offset[2, 6] },
        { type: BUILDING_SMALL_MASTABA_SIDE, offset[0, 8] },              { type: BUILDING_SMALL_MASTABA_SIDE, offset[2, 8] }
    ]

    config_east [
        { type: BUILDING_SMALL_MASTABA,      offset[-2, 0]},     { type: BUILDING_SMALL_MASTABA,      offset[-1, -1], base:true },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, 2] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, 2] },
        { type: BUILDING_SMALL_MASTABA_ENTRANCE, offset[0, 4] }, { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, 4] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, 6] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, 6] },
        { type: BUILDING_SMALL_MASTABA_SIDE, offset[0, 8] },     { type: BUILDING_SMALL_MASTABA_SIDE, offset[-2, 8] }
    ]

    config_south [
        { type: BUILDING_SMALL_MASTABA,      offset[-2, -8] },    { type: BUILDING_SMALL_MASTABA,      offset[0, -8] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, -2] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, -2] },
        { type: BUILDING_SMALL_MASTABA_ENTRANCE, offset[0, -4] }, { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, -4] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, -6] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[-2, -6] },
        { type: BUILDING_SMALL_MASTABA_SIDE, offset[-2, 0] },     { type: BUILDING_SMALL_MASTABA,      offset[-1, -1], base:true }
    ]

    config_west [
        { type: BUILDING_SMALL_MASTABA,      offset[0, -8] },     { type: BUILDING_SMALL_MASTABA,      offset[2, -8] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, -6] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[2, -6] },
        { type: BUILDING_SMALL_MASTABA_ENTRANCE, offset[2, -4] }, { type: BUILDING_SMALL_MASTABA_WALL, offset[0, -4] },
        { type: BUILDING_SMALL_MASTABA_WALL, offset[0, -2] },     { type: BUILDING_SMALL_MASTABA_WALL, offset[2, -2] },
        { type: BUILDING_SMALL_MASTABA_SIDE, offset[2, 0] },      { type: BUILDING_SMALL_MASTABA, offset[-1, -1], base:true }
    ]
  }

  // Parts are 2×2 blocks on the init_tiles grid — without building_size they stay
  // size 0, claim no map tiles, and only the main-type pieces render (thin strip).
  building_small_mastaba_part_side { building_size: 2, show_in_debug: false }
  building_small_mastaba_part_wall { building_size: 2, show_in_debug: false }
  building_small_mastaba_part_entrance { building_size: 2, show_in_debug: false }

  building_medium_mastaba  {
    animations {
      _pack { pack:PACK_MASTABA }
      preview { id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { id:1, offset:0 }
      base_grounded { path:"mastaba/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"pharaoh_general/plazatiles_00064" }
      empty_land {path:"mastaba/mastaba_00109"}
      enter {id:114}
    }

    building_size : 2
    info_title_id [198, 19]
    fire_proof :  true
    damage_proof : true
    meta { help_id:4, text_id:178 }
    info_sound : "Wavs/rock3.wav"
    init_tiles [14, 6]

    flags {
        is_monument: true
        allow_rotate: true
    }

    config_north [
        { type: BUILDING_MEDIUM_MASTABA, offset[-1, -1], base:true }, { type: BUILDING_MEDIUM_MASTABA,      offset[2, 0] },  { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[4, 0] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 2] },         { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, 2] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, 2] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 4] },         { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, 4] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, 4] },
        { type: BUILDING_MEDIUM_MASTABA_ENTRANCE, offset[4, 6] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 6] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, 6] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 8] },         { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, 8] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, 8] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[0, 10] },        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[2, 10] }, { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[4, 10] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[0, 12] },        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[2, 12] }, { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[4, 12] }
    ]

    config_east [
        { type: BUILDING_MEDIUM_MASTABA,      offset[-2, 0]},     { type: BUILDING_MEDIUM_MASTABA,      offset[-4, 0] },  { type: BUILDING_MEDIUM_MASTABA,      offset[-1, -1], base:true }
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 2] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, 2] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, 2] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 4] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, 4] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, 4] },
        { type: BUILDING_MEDIUM_MASTABA_ENTRANCE, offset[0, 6] }, { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, 6] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, 6] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, 8] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, 8] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, 8] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[0, 10] },    { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-2, 10] }, { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-4, 10] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[0, 12] },    { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-2, 12] }, { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-4, 12] }
    ]

    config_south [
        { type: BUILDING_MEDIUM_MASTABA,      offset[-4, -12] },   { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-2, -12] }, { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[0, -12] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -10] },    { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, -10] }, { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, -10] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -8] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, -8] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, -8] },
        { type: BUILDING_MEDIUM_MASTABA_ENTRANCE, offset[0, -6] }, { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, -6] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, -6] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -4] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, -4] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, -4] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -2] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-2, -2] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[-4, -2] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-2, 0] },     { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[-4, 0] },   { type: BUILDING_MEDIUM_MASTABA, offset[-1, -1], base:true }
    ]

    config_west [
        { type: BUILDING_MEDIUM_MASTABA,      offset[0, -12] },    { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[2, -12] },  { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[4, -12] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -10] },    { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, -10] },  { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, -10] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -8] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, -8] },   { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, -8] },
        { type: BUILDING_MEDIUM_MASTABA_ENTRANCE, offset[4, -6] }, { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, -6] },   { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -6] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -4] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, -4] },   { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, -4] },
        { type: BUILDING_MEDIUM_MASTABA_WALL, offset[0, -2] },     { type: BUILDING_MEDIUM_MASTABA_WALL, offset[2, -2] },   { type: BUILDING_MEDIUM_MASTABA_WALL, offset[4, -2] },
        { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[2, 0] },      { type: BUILDING_MEDIUM_MASTABA_SIDE, offset[4, 0] },   { type: BUILDING_MEDIUM_MASTABA, offset[-1, -1], base:true }
    ]
  }

  building_medium_mastaba_part_side { building_size: 2, show_in_debug: false }
  building_medium_mastaba_part_wall { building_size: 2, show_in_debug: false }
  building_medium_mastaba_part_entrance { building_size: 2, show_in_debug: false }

  building_large_mastaba {
    animations {
      _pack { pack:PACK_MASTABA }
      preview { id:2, offset:7 }
      base { id:2, offset:7 }
      base_bricks { id:1, offset:0 }
      base_grounded { path:"mastaba/pyramid_phase_one_00013" }
      clear_land { id:2, offset:12 }
      image_stick { path:"pharaoh_general/plazatiles_00064" }
      empty_land { path:"mastaba/mastaba_00109" }
      enter { id:114 }
    }

    building_size : 2
    info_title_id [198, 20]
    fire_proof :  true
    damage_proof : true
    meta { help_id:4, text_id:178 }
    info_sound : "Wavs/rock3.wav"
    init_tiles [18, 8]

    flags {
        is_monument: true
        allow_rotate: true
    }

    // 4×9 parts on 8×18 footprint (mirror medium pattern scaled).
    config_north [
        { type: BUILDING_LARGE_MASTABA, offset[-1, -1], base:true }, { type: BUILDING_LARGE_MASTABA, offset[2, 0] }, { type: BUILDING_LARGE_MASTABA, offset[4, 0] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, 0] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, 2] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, 4] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, 6] },
        { type: BUILDING_LARGE_MASTABA_ENTRANCE, offset[6, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, 8] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, 10] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[2, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[4, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, 12] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[2, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[4, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, 14] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[2, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[4, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, 16] }
    ]

    config_east [
        { type: BUILDING_LARGE_MASTABA, offset[-2, 0] }, { type: BUILDING_LARGE_MASTABA, offset[-4, 0] }, { type: BUILDING_LARGE_MASTABA, offset[-6, 0] }, { type: BUILDING_LARGE_MASTABA, offset[-1, -1], base:true },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, 2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, 2] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, 4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, 4] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, 6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, 6] },
        { type: BUILDING_LARGE_MASTABA_ENTRANCE, offset[0, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, 8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, 8] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, 10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, 10] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-2, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-4, 12] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-6, 12] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-2, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-4, 14] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-6, 14] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-2, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-4, 16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-6, 16] }
    ]

    config_south [
        { type: BUILDING_LARGE_MASTABA, offset[-6, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-4, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-2, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[0, -16] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -14] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -12] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -10] },
        { type: BUILDING_LARGE_MASTABA_ENTRANCE, offset[0, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -8] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -6] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -4] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-2, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-4, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[-6, -2] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[-2, 0] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-4, 0] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[-6, 0] }, { type: BUILDING_LARGE_MASTABA, offset[-1, -1], base:true }
    ]

    config_west [
        { type: BUILDING_LARGE_MASTABA, offset[0, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[2, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[4, -16] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, -16] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -14] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -14] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -12] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -12] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -10] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -10] },
        { type: BUILDING_LARGE_MASTABA_ENTRANCE, offset[6, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -8] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -8] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -6] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -6] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -4] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -4] },
        { type: BUILDING_LARGE_MASTABA_WALL, offset[0, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[2, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[4, -2] }, { type: BUILDING_LARGE_MASTABA_WALL, offset[6, -2] },
        { type: BUILDING_LARGE_MASTABA_SIDE, offset[2, 0] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[4, 0] }, { type: BUILDING_LARGE_MASTABA_SIDE, offset[6, 0] }, { type: BUILDING_LARGE_MASTABA, offset[-1, -1], base:true }
    ]
  }

  building_large_mastaba_part_side { building_size: 2, show_in_debug: false }
  building_large_mastaba_part_wall { building_size: 2, show_in_debug: false }
  building_large_mastaba_part_entrance { building_size: 2, show_in_debug: false }