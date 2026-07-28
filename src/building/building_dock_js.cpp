#include "building_dock.h"

#include "city/city_buildings.h"
#include "game/resource.h"
#include "js/js_game.h"
#include "core/profiler.h"

bool __dock_is_trade_accepted(int bid, int resource) {
    auto b = building_get(bid)->dcast_dock();
    return b ? b->is_trade_accepted((e_resource)resource) : false;
}
ANK_FUNCTION_2(__dock_is_trade_accepted)

void __dock_toggle_good_accepted(int bid, int resource) {
    auto b = building_get(bid)->dcast_dock();
    if (b) {
        b->toggle_good_accepted((e_resource)resource);
    }
}
ANK_FUNCTION_2(__dock_toggle_good_accepted)

void __dock_unaccept_all_goods(int bid) {
    auto b = building_get(bid)->dcast_dock();
    if (b) {
        b->unaccept_all_goods();
    }
}
ANK_FUNCTION_1(__dock_unaccept_all_goods)

void __dock_accept_all_goods(int bid) {
    auto b = building_get(bid)->dcast_dock();
    if (b) {
        b->accept_all_goods();
    }
}
ANK_FUNCTION_1(__dock_accept_all_goods)

bool __dock_accepts_any_goods(int bid) {
    auto b = building_get(bid)->dcast_dock();
    return b && b->accepts_any_goods();
}
ANK_FUNCTION_1(__dock_accepts_any_goods)

static void __test_fill_resource_list(resource_list &out, int r0, int r1, int r2) {
    const int rs[3] = { r0, r1, r2 };
    for (int r : rs) {
        if (r > RESOURCE_NONE && r < RESOURCES_MAX) {
            out[(e_resource)r] = 1;
        }
    }
}

/** Test/helper: unweighted any-match count for up to 3 partner goods (0 = unused). */
int __test_dock_match_score(int bid, int r0, int r1, int r2) {
    auto b = building_get(bid)->dcast_dock();
    if (!b) {
        return -1;
    }
    resource_list active;
    __test_fill_resource_list(active, r0, r1, r2);
    resource_list empty;
    return b->count_matching_goods(active, empty);
}
ANK_FUNCTION_4(__test_dock_match_score)

bool __test_dock_any_match(int bid, int r0, int r1, int r2) {
    return __test_dock_match_score(bid, r0, r1, r2) > 0;
}
ANK_FUNCTION_4(__test_dock_any_match)

/** Test: weighted score — imports ×2, exports ×1 (up to 3 each; 0 = unused). */
int __test_dock_trade_score(int bid, int i0, int i1, int i2, int e0, int e1, int e2) {
    auto b = building_get(bid)->dcast_dock();
    if (!b) {
        return -1;
    }
    resource_list imports;
    resource_list exports;
    __test_fill_resource_list(imports, i0, i1, i2);
    __test_fill_resource_list(exports, e0, e1, e2);
    return b->trade_match_score(imports, exports);
}
ANK_FUNCTION_7(__test_dock_trade_score)

int __test_dock_yard_proximity(int bid) {
    auto b = building_get(bid)->dcast_dock();
    return b ? b->yard_proximity_cost() : -1;
}
ANK_FUNCTION_1(__test_dock_yard_proximity)

int __test_dock_match_score_for_ship(int bid, int ship_id) {
    auto b = building_get(bid)->dcast_dock();
    return b ? b->match_score_for_ship(ship_id) : -1;
}
ANK_FUNCTION_2(__test_dock_match_score_for_ship)

int __test_map_get_free_dock(int ship_id) {
    return map_get_free_destination_dock(ship_id).bid;
}
ANK_FUNCTION_1(__test_map_get_free_dock)

bool __dock_has_trade_ship(int bid) {
    auto b = building_get(bid)->dcast_dock();
    return b && b->runtime_data().trade_ship != 0;
}
ANK_FUNCTION_1(__dock_has_trade_ship)

int __dock_count_idle_dockers(int bid) {
    auto b = building_get(bid)->dcast_dock();
    return b ? b->count_idle_dockers() : 0;
}
ANK_FUNCTION_1(__dock_count_idle_dockers)
