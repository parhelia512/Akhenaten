log_info("akhenaten: enemy_army_debug")

[es=event_draw_debug_properties]
function enemy_army_debug_draw_properties(ev) {
    if (!imgui.tree_node_ex("Enemy Armies")) {
        return
    }

    var max = __enemy_armies_max()
    for (var i = 0; i < max; i++) {
        var army = new EnemyArmy(i)
        if (!army || !army.formation_id) {
            continue
        }

        var label = "Army #" + army.army_id + " (invasion_id: " + i + ")"
        if (!imgui.tree_node_ex2("enemy_army_" + i, label)) {
            continue
        }

        imgui.begin_table("EnemyArmy" + i, 2, imgui.table_flags_debug_props())
        imgui.property_input("army_id", army, "army_id")
        imgui.property_input("formation_id", army, "formation_id")
        imgui.property_input("layout", army, "layout")
        imgui.property_input("home", army.home)
        imgui.property_input("destination", army.destination)
        imgui.property_input("destination_building_id", army, "destination_building_id")
        imgui.property_input("num_batalions", army, "num_batalions")

        var bats = army.battalion_ids()
        for (var b = 0; b < bats.length; b++) {
            imgui.property_input("battalion " + b, bats[b])
        }

        imgui.property_input("ignore_pharaoh_soldiers", army, "ignore_pharaoh_soldiers")
        imgui.property_input("buildings_to_destroy", army, "buildings_to_destroy")
        imgui.property_input("buildings_destroyed", army, "buildings_destroyed")
        imgui.end_table()
        imgui.tree_pop()
    }

    imgui.tree_pop()
}
