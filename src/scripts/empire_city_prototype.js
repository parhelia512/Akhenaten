log_info("akhenaten: empire_city_prototype started")

EmpireCity.property.empire_object = { get: function() { return new EmpireCityObject(this.id) } }
EmpireCity.property.is_sieged = { get: function() { return this.months_under_siege > 0 } }
EmpireCity.property.name = {
    get: function() {
        return __loc(game_features.gameui_empire_city_old_names ? 195 : 21, this.name_id)
    }
}
EmpireCity.property.can_trade = {
    get: function() {
        if (this.is_sieged) {
            return false
        }
        switch (this.type) {
        case EMPIRE_CITY_PHARAOH_TRADING:
        case EMPIRE_CITY_EGYPTIAN_TRADING:
        case EMPIRE_CITY_FOREIGN_TRADING:
            return true
        }
        return false
    }
}

EmpireCityObject.property.pos = { }

EmpireObject.property.pos = { }
EmpireObject.property.expanded_pos = { }
EmpireObject.property.expanded_image_id = { }
EmpireObject.property.city_id = { }
EmpireObject.property.label = {
    get: function() {
        if (this.text_key) {
            return __loc(this.text_key)
        }
        return __loc(196, this.city_name_id)
    }
}
EmpireObject.property.map_pos = {
    get: function() { return empire.is_expanded ? this.expanded_pos : this.pos }
}
EmpireObject.property.map_image_id = {
    get: function() { return empire.is_expanded ? this.expanded_image_id : this.image_id }
}