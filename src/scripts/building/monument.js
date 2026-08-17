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

Monument.prototype.need_stonemason = function() {
    return __monument_need_stonemason(this.id)
}

Monument.prototype.tile_progress = function(tile) {
    return __map_monuments_get_progress(tile)
}

Monument.prototype.set_tile_progress = function(tile, progress) {
    __monument_set_tile_progress(this.id, tile, progress)
}

// Walks the multipart chain (same coverage as map_grid_get_tiles(main, 0)).
Monument.prototype.foreach_tile = function(fn) {
    var part = city.get_building(this.id)
    var guard = 0
    while (part && guard < 128) {
        guard++
        var origin = part.tile
        var size = part.size
        for (var dx = 0; dx < size; dx++) {
            for (var dy = 0; dy < size; dy++) {
                fn({ x: origin.x + dx, y: origin.y + dy })
            }
        }
        if (!part.next_part_building_id) {
            break
        }
        part = city.get_building(part.next_part_building_id)
    }
}

// Dev cheat: set incomplete tiles to 200. max_count <= 0 = all.
Monument.prototype.boost_incomplete_tiles = function(max_count) {
    if (this.phase() === -1) {
        return 0
    }
    var limit = max_count || 0
    var bumped = 0
    var done = false
    var self = this
    this.foreach_tile(function(tile) {
        if (done || (limit > 0 && bumped >= limit)) {
            done = true
            return
        }
        var incomplete = self.tile_progress(tile) < 200
        if (limit > 0 && !incomplete) {
            return
        }
        if (incomplete) {
            bumped++
        }
        self.set_tile_progress(tile, 200)
    })
    return bumped
}

Monument.prototype.weight_btype = function() {
    return monument_weight_btype(this.id)
}
