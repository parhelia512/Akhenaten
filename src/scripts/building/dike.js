log_info("akhenaten: building dike started")

building_dike {
  animations {
    preview { pack:PACK_GENERAL, id:138, offset:27 }
    base { pack:PACK_GENERAL, id:138 }
  }
  building_size : 1
  planner_update_rule {
    is_draggable : true
  }
  cost [ 3, 6, 10, 15, 25 ]
  laborers [0]
  fire_proof : true
  damage_proof : true
  fire_risk [0]
  damage_risk [0]
  build_menu_text : "#building_dike"
  info_title_id : "#building_dike"
  flags {
    is_infrastructure: true
    no_road_access: true
  }
}
