log_info("akhenaten: city local cults started")

var LOCAL_CULT_NONE = 0
var LOCAL_CULT_ANUBIS = 1
var LOCAL_CULT_THOTH = 2
var LOCAL_CULT_HATHOR = 3

var FESTIVAL_THEME_NONE = 0
var FESTIVAL_THEME_HARVEST = 1
var FESTIVAL_THEME_WAR = 2
var FESTIVAL_THEME_CRAFT = 3

city.local_cults = extend(__city_local_cults, {
	refresh: __city_local_cults_refresh
	unlock_all: __city_local_cults_unlock_all
	set_unlocked: __city_local_cults_set_unlocked
	is_unlocked: __city_local_cults_is_unlocked
	is_active: __city_local_cults_is_active
	appeased_months: __city_local_cults_appeased_months
	name: __city_local_cults_name
	host_god: __city_local_cults_host_god
	building_type: __city_local_cults_building_type
	count: __city_local_cults_count
	apply_theme: __city_local_cults_apply_theme
	apply_cult_festival: __city_local_cults_apply_cult_festival
	advance_month: __city_local_cults_advance_month
	next_rite_id: __city_local_cults_next_rite_id
	next_rite_theme: __city_local_cults_next_rite_theme
})

city.local_cults.ids = [LOCAL_CULT_ANUBIS, LOCAL_CULT_THOTH, LOCAL_CULT_HATHOR]

city.local_cults.loc_key = function(cult_id) {
	return "#local_cult_" + city.local_cults.name(cult_id)
}
