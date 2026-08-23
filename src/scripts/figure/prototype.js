log_info("akhenaten: figure_prototype.js loaded")

/* type, action_state, state, wait_ticks, direction, target_figure_id,
   destination_building_id, home_building_id, movement_ticks_watchdog,
   resource_id, resource_amount_full, draw_mode: native CPTROFF (js_register_figure). */

Figure.property.valid = { get: function() { return this.__valid() } }
Figure.property.resource = { get: function() { return this.resource_id } }
Figure.property.resource_amount = { get: function() { return this.resource_amount_full } }
Figure.property.movement_watchdog = { get: function() { return this.movement_ticks_watchdog } }
Figure.property.is_on_previous_tile = { get: function() { return this.__is_on_previous_tile() } }
Figure.property.destination_id = { get: function() { return this.destination_building_id } }
Figure.property.destination = { get: function() { return city.get_building(this.destination_building_id) } }
Figure.property.home = { get: function() { return city.get_building(this.home_building_id) } }
Figure.property.anim_key = { get: function() { return this.__anim_key() } }
Figure.property.overlay = { get: function() { return this.__overlay() } }
Figure.property.params = { get: function() { return city.get_figure_params_by_type(this.type) } }
