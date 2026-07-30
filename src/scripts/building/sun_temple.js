log_info("akhenaten: building sun_temple started")

building_sun_temple {
    animations {
      sa { pack:PACK_SUN_TEMPLE_1, id:1 }
      sb { pack:PACK_SUN_TEMPLE_2, id:1 }
      sc { pack:PACK_SUN_TEMPLE_3, id:1 }
      sd { pack:PACK_SUN_TEMPLE_3, id:2 }
      preview { pack:PACK_SUN_TEMPLE_1, id:1 }
    }
    // 10×10 body only; path + hall extension are not implemented yet.
    building_size : 10
    art_stages : 4
    // Phase 2 scaffolding only (provisional until .pak timber dump).
    timber_loads [ 100 ]
    // Phase 4 vestibule / foretemple.
    sandstone_loads [ 160 ]
    build_sandstone : 160
    placement_resources [
        { resource: RESOURCE_SANDSTONE, count: 220 }
    ]
    fire_proof : true
    damage_proof : true
    info_title_id [198, 24]
    meta { help_id: 69, text_id: 178 }
    info_sound : "Wavs/rock3.wav"
    cost [ 4000, 5000, 6500, 9000, 13000 ]
    flags {
        is_monument: true
    }
}
