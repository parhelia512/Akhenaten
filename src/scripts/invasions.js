log_info("akhenaten: invasions started")

invasions = {
	min_invasion_amount : 1
	max_invasion_amount : 150
}

/** Keep in sync with invasion_data_t::warnings size */
invasions.warning_slots = 101
invasions.get_warning = function(index) {
	return {
		warning_index: index
		__property_getter: function(property) { return __invasion_warning_get_property(this.warning_index, property) }
		@in_use {}
		@handled {}
		@pos {}
		@image_id {}
	}
}
