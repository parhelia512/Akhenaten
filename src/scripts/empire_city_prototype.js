log_info("akhenaten: empire_city_prototype started")

EmpireCity.property.empire_object = { get: function() { return new EmpireCityObject(this.id) } }
EmpireCity.property.is_sieged = { get: function() { return this.months_under_siege > 0 } }
EmpireCity.property.name = {
    get: function() {
        return __loc(game_features.gameui_empire_city_old_names ? 195 : 21, this.name_id)
    }
}

EmpireCityObject.property.pos = { }