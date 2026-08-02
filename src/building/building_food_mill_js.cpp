#include "building_food_mill.h"

#include "building/building_storage.h"
#include "core/profiler.h"
#include "js/js_game.h"

int __food_mill_get_amount(int bid, int resource) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    return mill ? mill->amount((e_resource)resource) : 0;
}
ANK_FUNCTION_2(__food_mill_get_amount)

int __food_mill_get_total_stored(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    return mill ? mill->total_stored() : 0;
}
ANK_FUNCTION_1(__food_mill_get_total_stored)

int __food_mill_get_freespace(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    return mill ? mill->freespace() : 0;
}
ANK_FUNCTION_1(__food_mill_get_freespace)

int __food_mill_food_variety(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    return mill ? mill->food_variety() : 0;
}
ANK_FUNCTION_1(__food_mill_food_variety)

bool __building_is_food_mill(int bid) {
    building *b = building_get(bid);
    return b && b->is_valid() && b->dcast_food_mill();
}
ANK_FUNCTION_1(__building_is_food_mill)

int __food_mill_resource_state(int bid, int resource) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    const storage_t *s = mill ? mill->storage() : nullptr;
    return s ? s->resource_state[resource] : 0;
}
ANK_FUNCTION_2(__food_mill_resource_state)

int __food_mill_resource_max_accept(int bid, int resource) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    const storage_t *s = mill ? mill->storage() : nullptr;
    return s ? s->resource_max_accept[resource] : 0;
}
ANK_FUNCTION_2(__food_mill_resource_max_accept)

int __food_mill_resource_max_get(int bid, int resource) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    const storage_t *s = mill ? mill->storage() : nullptr;
    return s ? s->resource_max_get[resource] : 0;
}
ANK_FUNCTION_2(__food_mill_resource_max_get)

bool __food_mill_is_empty_all(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    return mill ? mill->is_empty_all() : false;
}
ANK_FUNCTION_1(__food_mill_is_empty_all)

void __food_mill_toggle_empty_all(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    if (mill) {
        building_storage_toggle_empty_all(mill->storage_id());
    }
}
ANK_FUNCTION_1(__food_mill_toggle_empty_all)

void __food_mill_accept_none(int bid) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    if (mill) {
        building_storage_accept_none(mill->storage_id());
    }
}
ANK_FUNCTION_1(__food_mill_accept_none)

void __food_mill_cycle_resource_state(int bid, int resource) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    if (mill) {
        building_storage_cycle_resource_state(mill->storage_id(), resource, false);
    }
}
ANK_FUNCTION_2(__food_mill_cycle_resource_state)

void __food_mill_increase_decrease_resource_state(int bid, int resource, bool increase) {
    building_food_mill *mill = building_get(bid)->dcast_food_mill();
    if (mill) {
        building_storage_increase_decrease_resource_state(mill->storage_id(), resource, increase);
    }
}
ANK_FUNCTION_3(__food_mill_increase_decrease_resource_state)
