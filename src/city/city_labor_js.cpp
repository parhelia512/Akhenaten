#include "city/city_labor.h"

#include "city/city.h"
#include "js/js_game.h"
#include "js/js_global_object.h"

void __city_labor_set_priority(int category, int new_priority) {
    g_city.labor.set_priority(category, new_priority);
}
ANK_FUNCTION_2(__city_labor_set_priority)

int __city_labor_max_selectable_priority(int category) {
    return g_city.labor.max_selectable_priority(category);
}
ANK_FUNCTION_1(__city_labor_max_selectable_priority)

int __city_labor_calc_fixed_workers_available(int num_plebs) {
    return city_labor_t::calc_fixed_workers_available(num_plebs);
}
ANK_FUNCTION_1(__city_labor_calc_fixed_workers_available)

void __city_labor_calculate_workers(int num_plebs, int num_patricians) {
    g_city.labor.calculate_workers(num_plebs, num_patricians);
}
ANK_FUNCTION_2(__city_labor_calculate_workers)

ANK_GLOBAL_OBJECT(g_city.labor, __city_labor,
    workers_available,
    workers_employed,
    workers_unemployed,
    workers_needed,
    unemployment_percentage_for_goverment,
    unemployment_percentage);
