log_info("akhenaten: building monument prototype started")

function Monument(building_id) {
    this.id = building_id
}

Monument.prototype = Object.create(Building.prototype)
Monument.prototype.constructor = Monument

// runtime_data_t fields (ANK_CONFIG_PROPERTY on building_monument::runtime_data_t)
Monument.prototype.__property_getter = function(property) {
    return __monument_get_property(this.id, property)
}
Monument.property.variant = { }

Monument.prototype.need_workers = function() {
    return __monument_need_workers(this.id)
}

Monument.prototype.phase = function() {
    return __monument_phase_code(this.id)
}

Monument.prototype.phases_total = function() {
    return __monument_phases_total(this.id)
}

Monument.prototype.material_pct_min = function() {
    return __monument_material_pct_min(this.id)
}

Monument.prototype.weight_btype = function() {
    return monument_weight_btype(this.id)
}
