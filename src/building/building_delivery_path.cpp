#include "building/building_delivery_path.h"

#include "building/building.h"
#include "building/building_farm.h"
#include "building/building_fishing_wharf.h"
#include "building/building_granary.h"
#include "building/building_storage.h"
#include "building/building_storage_yard.h"
#include "building/construction/build_planner.h"
#include "city/buildings.h"
#include "city/city.h"
#include "city/city_buildings.h"
#include "city/city_industry.h"
#include "city/city_resource.h"
#include "city/object_info.h"
#include "core/calc.h"
#include "figure/figure.h"
#include "figure/route.h"
#include "figuretype/figure_cartpusher.h"
#include "game/game.h"
#include "game/game_config.h"
#include "graphics/color.h"
#include "graphics/elements/ui.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "graphics/view/view.h"
#include "graphics/view/zoom.h"
#include "graphics/window.h"
#include "grid/road_access.h"
#include "grid/road_network.h"
#include "grid/routing/routing.h"
#include "grid/terrain.h"
#include "input/keyboard.h"
#include "widget/widget_city.h"
#include "window/window_info.h"

namespace {

bool destination_is_storage_yard_goto(building &dest) {
    // Match figure::do_gotobuilding warehouse branch (YARD / ROOM only — not YARD_UP).
    return dest.type == BUILDING_STORAGE_YARD || dest.type == BUILDING_STORAGE_ROOM
        || dest.dcast_storage_room();
}

tile2i delivery_finish_tile_for_destination(building &dest) {
    building *main = dest.main();
    if (!main || !main->is_valid()) {
        return tile2i::invalid;
    }

    if (destination_is_storage_yard_goto(dest)) {
        return map_closest_road_within_radius(main->tile, 3, 1);
    }

    if (main->has_road_access && main->road_access.valid()) {
        return main->road_access;
    }

    return map_closest_road_within_radius(dest.tile, dest.size, 1);
}

void resolve_prediction_access_tile(cartpusher_deliveryman_prediction &pred) {
    if (!pred.destination) {
        return;
    }
    building *dest = building_get(pred.destination);
    if (!dest || !dest->is_valid()) {
        return;
    }
    tile2i finish = delivery_finish_tile_for_destination(*dest);
    if (finish.valid()) {
        pred.access_tile = finish;
    }
}

bool apply_farms_deliver_close(building &home, building_id dest_id) {
    if (!dest_id || !game_features::gameplay_change_farms_deliver_close.to_bool()) {
        return true;
    }
    if (!home.dcast_farm() && !home.dcast_fishing_wharf()) {
        return true;
    }
    building *dst = building_get(dest_id);
    if (!dst || !dst->is_valid()) {
        return false;
    }
    const int dist = calc_distance_with_penalty(home.tile, dst->tile, home.distance_from_entry, dst->distance_from_entry);
    return dist < 64;
}

e_delivery_dest_kind kind_from_building(building *b) {
    if (!b || !b->is_valid()) {
        return e_delivery_dest_kind::none;
    }
    if (b->type == BUILDING_STORAGE_YARD || b->type == BUILDING_STORAGE_YARD_UP
        || b->type == BUILDING_STORAGE_ROOM || b->dcast_storage_yard() || b->dcast_storage_room()) {
        return e_delivery_dest_kind::storage_yard;
    }
    if (b->type == BUILDING_GRANARY || b->type == BUILDING_GRANARY_UP || b->dcast_granary()) {
        return e_delivery_dest_kind::granary;
    }
    switch (b->type) {
    case BUILDING_VILLAGE_PALACE:
    case BUILDING_TOWN_PALACE:
    case BUILDING_CITY_PALACE:
        return e_delivery_dest_kind::palace;
    default:
        if (b->is_workshop()) {
            return e_delivery_dest_kind::workshop;
        }
        return e_delivery_dest_kind::none;
    }
}

color color_for_dest_kind(e_delivery_dest_kind kind) {
    switch (kind) {
    case e_delivery_dest_kind::storage_yard:
        return COLOR_MASK_AMBER;
    case e_delivery_dest_kind::granary:
        return COLOR_MASK_GREEN;
    case e_delivery_dest_kind::workshop:
        return COLOR_MASK_BLUE;
    case e_delivery_dest_kind::palace:
        return COLOR_MASK_LEGION_HIGHLIGHT;
    default:
        return COLOR_MASK_NONE;
    }
}

pcstr tooltip_for_reason(e_delivery_path_reason reason) {
    pcstr key = nullptr;
    switch (reason) {
    case e_delivery_path_reason::no_road:
        key = "#delivery_path_no_road";
        break;
    case e_delivery_path_reason::understaffed:
        key = "#delivery_path_understaffed";
        break;
    case e_delivery_path_reason::no_destination:
        key = "#delivery_path_no_destination";
        break;
    default:
        return nullptr;
    }
    return ui::str_from_key(key);
}

pcstr tooltip_no_route() {
    return ui::str_from_key("#delivery_path_no_route");
}

bool is_enroute_delivering_cart_action(int action) {
    switch (action) {
    case ACTION_9_CARTPUSHER_DELIVERING_GOODS:
    case ACTION_10_CARTPUSHER_DELIVERING_FOOD:
    case ACTION_11_CARTPUSHER_DELIVERING_GOLD:
    case ACTION_21_CARTPUSHER_DELIVERING_TO_WAREHOUSE:
    case ACTION_22_CARTPUSHER_DELIVERING_TO_GRANARY:
    case ACTION_23_CARTPUSHER_DELIVERING_TO_WORKSHOP:
    case ACTION_51_CARTPUSHER_DELIVERING_RESOURCE:
        return true;
    default:
        return false;
    }
}

bool tile_is_citizen_road(tile2i tile) {
    return tile.valid() && map_terrain_is(tile, TERRAIN_ROAD);
}

building *resolve_delivery_source_building() {
    if (g_window_manager.window_is("window_building_info")) {
        building_id bid = common_info_window::get_object_info().bid;
        if (bid) {
            building *b = building_get(bid);
            if (b && b->is_valid()) {
                return b->main();
            }
        }
        return nullptr;
    }

    if (!g_screen_city.current_tile.valid()) {
        return nullptr;
    }
    building *b = building_at(g_screen_city.current_tile);
    if (!b || !b->is_valid()) {
        return nullptr;
    }
    return b->main();
}

struct delivery_draw_cache_t {
    building_id from = 0;
    e_resource resource = RESOURCE_NONE;
    building_id to = 0;
    e_delivery_dest_kind kind = e_delivery_dest_kind::none;
    e_delivery_path_reason reason = e_delivery_path_reason::no_destination;
    tile2i from_access;
    tile2i to_access;
    bool has_route = false;
    bool from_live_cart = false;
    int refresh_age = 0;
    uint8_t dirs[MAX_PATH_LENGTH];
    int path_len = 0;
};

delivery_draw_cache_t g_delivery_draw_cache;

void draw_path_dirs(painter &ctx, tile2i start, const uint8_t *dirs, int len, color mask) {
    if (!start.valid() || len <= 0) {
        return;
    }

    const float zoom = g_zoom.get_scale();
    int tx = start.x();
    int ty = start.y();
    vec2i coords = g_camera.lookup_tile_to_pixel(start);
    ctx.img_generic(image_id_from_group(PACK_CUSTOM, 1) + 3, coords * zoom, mask);

    for (int i = 0; i < len; i++) {
        int img_index = 10;
        const int pdir = dirs[i];
        switch (pdir) {
        case 0:
            ty--;
            img_index = 0;
            break;
        case 1:
            tx++;
            ty--;
            break;
        case 2:
            tx++;
            img_index = 1;
            break;
        case 3:
            tx++;
            ty++;
            break;
        case 4:
            ty++;
            img_index = 0;
            break;
        case 5:
            tx--;
            ty++;
            break;
        case 6:
            tx--;
            img_index = 1;
            break;
        case 7:
            tx--;
            ty--;
            break;
        default:
            break;
        }
        coords = g_camera.lookup_tile_to_pixel(tile2i(tx, ty));
        ctx.img_generic(image_id_from_group(PACK_CUSTOM, 1) + img_index, coords * zoom, mask);
    }
}

} // namespace

cartpusher_deliveryman_prediction cartpusher_predict_deliveryman_destination(tile2i from_tile,
                                                                             e_resource resource,
                                                                             building &home,
                                                                             int road_network_id) {
    cartpusher_deliveryman_prediction result;
    if (resource == RESOURCE_NONE || !from_tile.valid()) {
        return result;
    }

    tile2i dst;
    if (road_network_id < 0) {
        // Cart parity: only the network under from_tile (no home fallback).
        road_network_id = map_road_network_get(from_tile);
    }
    int understaffed = 0;

    const auto finish = [&]() {
        result.understaffed = understaffed;
        resolve_prediction_access_tile(result);
        return result;
    };

    // Gold → palace
    if (resource == RESOURCE_GOLD) {
        const int senate_id = g_city.buildings.get_palace_id();
        building *palace = building_get(senate_id);
        if (senate_id && palace && palace->state == BUILDING_STATE_VALID && palace->num_workers >= 5) {
            result.destination = (building_id)senate_id;
            result.access_tile = palace->road_access.valid() ? palace->road_access : palace->tile;
            result.kind = e_delivery_dest_kind::palace;
            return finish();
        }
    }

    // Priority 1: warehouse if resource is on stockpile
    {
        const building_id stockpile_id = building_storage_yard_for_storing(from_tile, resource, home.distance_from_entry,
                                                                          road_network_id, &understaffed, dst);
        if (stockpile_id && g_city.resource.is_stockpiled(resource)) {
            result.destination = stockpile_id;
            result.access_tile = dst;
            result.kind = e_delivery_dest_kind::storage_yard;
            return finish();
        }
    }

    // Priority 2: accepting granary for food
    {
        const int granary_food_id = building_granary_for_storing(from_tile, resource, home.distance_from_entry,
                                                                road_network_id, 0, &understaffed, &dst);
        if (granary_food_id && apply_farms_deliver_close(home, (building_id)granary_food_id)) {
            result.destination = (building_id)granary_food_id;
            result.access_tile = dst;
            result.kind = e_delivery_dest_kind::granary;
            return finish();
        }
    }

    // Priority 3: workshop for raw material
    {
        const int workshop_id = building_get_workshop_for_raw_material_with_room(
            from_tile, resource, home.distance_from_entry, road_network_id, dst);
        if (workshop_id) {
            result.destination = (building_id)workshop_id;
            result.access_tile = dst;
            result.kind = e_delivery_dest_kind::workshop;
            return finish();
        }
    }

    // Priority 4: warehouse
    {
        const building_id warehouse_id = building_storage_yard_for_storing(from_tile, resource, home.distance_from_entry,
                                                                          road_network_id, &understaffed, dst);
        if (warehouse_id) {
            result.destination = warehouse_id;
            result.access_tile = dst;
            result.kind = e_delivery_dest_kind::storage_yard;
            return finish();
        }
    }

    // Priority 5: granary forced when on stockpile
    {
        const int granary_id = building_granary_for_storing(from_tile, resource, home.distance_from_entry, road_network_id,
                                                           1, &understaffed, &dst);
        if (granary_id && apply_farms_deliver_close(home, (building_id)granary_id)) {
            result.destination = (building_id)granary_id;
            result.access_tile = dst;
            result.kind = e_delivery_dest_kind::granary;
            return finish();
        }
    }

    return finish();
}

bool building_shows_delivery_paths(const building &b) {
    const building *m = b.main();
    if (!m || !m->is_valid()) {
        return false;
    }
    if (building_type_any_of(m->type, {
            BUILDING_STORAGE_YARD, BUILDING_STORAGE_YARD_UP, BUILDING_STORAGE_ROOM,
            BUILDING_GRANARY, BUILDING_GRANARY_UP,
            BUILDING_DOCK, BUILDING_DOCK_UP })) {
        return false;
    }
    if (m->type == BUILDING_TAX_COLLECTOR || m->type == BUILDING_TAX_COLLECTOR_UPGRADED) {
        return true;
    }
    return m->output.resource != RESOURCE_NONE;
}

delivery_path_query building_predict_delivery(const building &b, e_resource r) {
    delivery_path_query q;
    const building *m = b.main();
    if (!m || !m->is_valid()) {
        return q;
    }

    q.from = m->id;
    e_resource resource = r;
    if (resource == RESOURCE_NONE) {
        if (m->type == BUILDING_TAX_COLLECTOR || m->type == BUILDING_TAX_COLLECTOR_UPGRADED) {
            resource = RESOURCE_GOLD;
        } else {
            resource = m->output.resource;
        }
    }
    q.resource = resource;
    if (resource == RESOURCE_NONE) {
        return q;
    }

    q.from_access = m->road_access.valid() ? m->road_access : m->tile;

    if (m->has_figure(BUILDING_SLOT_CARTPUSHER)) {
        figure *f = m->get_figure(BUILDING_SLOT_CARTPUSHER);
        if (f && f->is_alive() && f->type == FIGURE_CART_PUSHER && is_enroute_delivering_cart_action(f->action_state)
            && f->has_destination()) {
            building *dest = f->destination();
            if (dest && dest->is_valid()) {
                q.to = dest->id;
                q.kind = kind_from_building(dest);
                q.reason = e_delivery_path_reason::ok;
                // destination_tile can be a footprint / off-road snap — only trust road tiles.
                if (tile_is_citizen_road(f->destination_tile)) {
                    q.to_access = f->destination_tile;
                } else {
                    q.to_access = delivery_finish_tile_for_destination(*dest);
                }
                if (!q.from_access.valid()) {
                    q.from_access = map_closest_road_within_radius(m->tile, m->size, 1);
                }
                q.from_live_cart = true;
                return q;
            }
        }
    }

    if (!m->has_road_access) {
        q.reason = e_delivery_path_reason::no_road;
        return q;
    }
    if (!q.from_access.valid()) {
        q.from_access = map_closest_road_within_radius(m->tile, m->size, 1);
    }
    if (!q.from_access.valid()) {
        q.reason = e_delivery_path_reason::no_road;
        return q;
    }

    int net = map_road_network_get(q.from_access);
    if (net <= 0) {
        net = m->road_network_id;
    }
    auto pred = cartpusher_predict_deliveryman_destination(q.from_access, resource, *const_cast<building *>(m), net);
    q.to = pred.destination;
    q.kind = pred.kind;
    q.to_access = pred.access_tile;
    if (q.to) {
        q.reason = e_delivery_path_reason::ok;
    } else if (pred.understaffed > 0) {
        q.reason = e_delivery_path_reason::understaffed;
    } else {
        q.reason = e_delivery_path_reason::no_destination;
    }
    return q;
}

bool delivery_path_fill_road(delivery_path_query &q, uint8_t *dirs, int *out_len, int max_len) {
    if (out_len) {
        *out_len = 0;
    }
    q.has_route = false;
    if (!q.to || !q.from_access.valid() || !q.to_access.valid() || !dirs || max_len <= 0) {
        return false;
    }

    tile2i to_access = q.to_access;

    // Same tile: can_travel is true (dist=1) but get_path returns 0 (while dist>1 never runs).
    if (q.from_access == to_access) {
        q.has_route = true;
        return true;
    }

    bool can = map_routing_citizen_can_travel_over_road(q.from_access, to_access);

    building *dest = building_get(q.to);
    if (dest && dest->is_valid()) {
        tile2i reach;
        if (destination_is_storage_yard_goto(*dest)) {
            building *main = dest->main();
            if (main && map_closest_reachable_road_within_radius(main->tile, 3, 1, reach)) {
                to_access = reach;
                can = true;
            }
        } else if (!can) {
            // Stale road_access / off-road snap — try perimeter road (cart else-branch).
            if (map_closest_reachable_road_within_radius(dest->tile, dest->size, 1, reach)) {
                to_access = reach;
                can = true;
            }
        }
    }

    if (!can) {
        return false;
    }

    if (q.from_access == to_access) {
        q.has_route = true;
        return true;
    }

    const int len = map_routing_get_path(dirs, q.from_access, to_access, 4);
    if (len <= 0 || len > max_len) {
        return false;
    }
    if (out_len) {
        *out_len = len;
    }
    q.has_route = true;
    return true;
}

void delivery_paths_draw(painter &ctx) {
    if (!game_features::gameui_show_delivery_paths.to_bool()) {
        return;
    }
    if ((keyboard_t::modifiers() & KEY_MOD_ALT) == 0) {
        return;
    }
    if (g_city_planner.build_type != BUILDING_NONE) {
        return;
    }

    building *src = resolve_delivery_source_building();
    if (!src || !building_shows_delivery_paths(*src)) {
        return;
    }

    delivery_path_query q = building_predict_delivery(*src);
    auto &cache = g_delivery_draw_cache;
    const bool need_route = q.to != 0 && q.from_access.valid() && q.to_access.valid();
    const int cache_ttl = (cache.has_route || !cache.to) ? 45 : 8;
    const bool cache_hit = cache.from == q.from && cache.resource == q.resource && cache.to == q.to
        && cache.from_access == q.from_access && cache.to_access == q.to_access
        && cache.reason == q.reason && cache.from_live_cart == q.from_live_cart
        && cache.refresh_age < cache_ttl;

    if (!cache_hit) {
        cache.from = q.from;
        cache.resource = q.resource;
        cache.to = q.to;
        cache.kind = q.kind;
        cache.reason = q.reason;
        cache.from_access = q.from_access;
        cache.to_access = q.to_access;
        cache.from_live_cart = q.from_live_cart;
        cache.refresh_age = 0;
        cache.path_len = 0;
        cache.has_route = false;
        if (need_route) {
            delivery_path_fill_road(q, cache.dirs, &cache.path_len, MAX_PATH_LENGTH);
            cache.has_route = q.has_route;
        }
    } else {
        cache.refresh_age++;
    }

    const color mask = color_for_dest_kind(cache.kind);
    if (cache.to) {
        building *dest = building_get(cache.to);
        const bool dest_ok = dest && dest->is_valid();
        if (dest_ok) {
            building *foot = dest->main();
            if (foot) {
                vec2i coords = g_camera.lookup_tile_to_pixel(foot->tile);
                build_planner::draw_building_ghost(ctx, image_id_from_group(PACK_CUSTOM, 1) + 3, coords, mask);
            }
        }
        if (dest_ok && cache.has_route && cache.path_len > 0) {
            draw_path_dirs(ctx, cache.from_access, cache.dirs, cache.path_len, mask);
        } else if (dest_ok && cache.has_route) {
            // Trivial same-tile route — footprint marker is enough.
        } else if (dest_ok && cache.from_access.valid() && cache.to_access.valid()) {
            vec2i coords = g_camera.lookup_tile_to_pixel(cache.to_access);
            build_planner::draw_building_ghost(ctx, image_id_from_group(PACK_CUSTOM, 1) + 3, coords, mask);
            if (pcstr tip = tooltip_no_route()) {
                ui::set_tooltip(tip);
            }
        } else if (dest_ok && cache.to_access.valid()) {
            vec2i coords = g_camera.lookup_tile_to_pixel(cache.to_access);
            build_planner::draw_building_ghost(ctx, image_id_from_group(PACK_CUSTOM, 1) + 3, coords, mask);
        }
    } else {
        if (cache.from_access.valid()) {
            vec2i pixel = g_camera.lookup_tile_to_pixel(cache.from_access);
            build_planner::draw_building_ghost(ctx, image_id_from_group(GROUP_TERRAIN_OVERLAY_COLORED) + 23, pixel,
                                               COLOR_MASK_RED);
        }
        if (pcstr tip = tooltip_for_reason(cache.reason)) {
            ui::set_tooltip(tip);
        }
    }
}
