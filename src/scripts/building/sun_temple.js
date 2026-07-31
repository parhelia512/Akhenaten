log_info("akhenaten: building sun_temple started")

building_sun_temple {
    animations {
      // Body packs: id 1/2 = map orientations (I4). Stages sa→sd = construction.
      sa1 { pack:PACK_SUN_TEMPLE_1, id:1 }
      sa2 { pack:PACK_SUN_TEMPLE_1, id:2 }
      sb1 { pack:PACK_SUN_TEMPLE_2, id:1 }
      sb2 { pack:PACK_SUN_TEMPLE_2, id:2 }
      sc1 { pack:PACK_SUN_TEMPLE_3, id:1 }
      sc2 { pack:PACK_SUN_TEMPLE_3, id:2 }
      sd1 { pack:PACK_SUN_TEMPLE_3, id:1 }
      sd2 { pack:PACK_SUN_TEMPLE_3, id:2 }
      preview { pack:PACK_SUN_TEMPLE_1, id:1 }

      // PACK_SUN_TEMPLE_EXTRA inventory (I10):
      // 1–2  118×127  hall / vestibule composites (orients)
      // 3–8  ~58×68   wall / pillar pieces
      // 9–12 ~58×30   path / plaza strips
      // 13   178×187  larger complex piece
      // 14–15 50×86   ornaments (used as stake ghost)
      hall1 { pack:PACK_SUN_TEMPLE_EXTRA, id:1 }
      hall2 { pack:PACK_SUN_TEMPLE_EXTRA, id:2 }
      wall_a { pack:PACK_SUN_TEMPLE_EXTRA, id:3 }
      wall_b { pack:PACK_SUN_TEMPLE_EXTRA, id:4 }
      wall_c { pack:PACK_SUN_TEMPLE_EXTRA, id:5 }
      wall_d { pack:PACK_SUN_TEMPLE_EXTRA, id:6 }
      wall_e { pack:PACK_SUN_TEMPLE_EXTRA, id:7 }
      wall_f { pack:PACK_SUN_TEMPLE_EXTRA, id:8 }
      path1 { pack:PACK_SUN_TEMPLE_EXTRA, id:9 }
      path2 { pack:PACK_SUN_TEMPLE_EXTRA, id:10 }
      path_b { pack:PACK_SUN_TEMPLE_EXTRA, id:11 }
      path_c { pack:PACK_SUN_TEMPLE_EXTRA, id:12 }
      complex { pack:PACK_SUN_TEMPLE_EXTRA, id:13 }
      stake { pack:PACK_SUN_TEMPLE_EXTRA, id:14 }
      ornament { pack:PACK_SUN_TEMPLE_EXTRA, id:15 }
    }
    // Body square; full AABB via init_tiles (body + path + hall).
    building_size : 10
    // Heaven: 10×10 body + 8×2 path + 3×3 hall → AABB ~10×15 along axis.
    init_tiles [10, 15]
    path_size [8, 2]
    hall_size [3, 3]
    // Offsets from body origin for placement rotation 0..3 (path then hall).
    // rot0 +Y: path centered on south edge; hall beyond path.
    part_path_offset [[1, 10], [-2, 1], [1, -2], [10, 1]]
    part_hall_offset [[4, 12], [-5, 4], [4, -5], [12, 4]]
    art_stages : 4
    // Heaven chart: wood = Y for carpenters (no fixed monument load published).
    // Remake keeps a phase-2 delivery gate; provisional 100 until .pak dump.
    timber_loads [ 100 ]
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
