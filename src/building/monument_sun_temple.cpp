#include "monument_sun_temple.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_figures.h"
#include "city/city_message.h"
#include "city/city_resource.h"
#include "city/city_warnings.h"
#include "construction/build_planner.h"
#include "figure/figure.h"
#include "game/game_events.h"
#include "game/resource.h"
#include "graphics/image.h"
#include "grid/building_tiles.h"
#include "grid/grid.h"
#include "io/io_buffer.h"
#include "js/js_game.h"

#include <algorithm>

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_sun_temple);

static monument g_monument_sun_temple{BUILDING_SUN_TEMPLE};

void building_sun_temple::static_params::rebuild_construction() {
    monument &m = g_monument_sun_temple;
    m.btype = BUILDING_SUN_TEMPLE;
    m.phases.clear();

    // Phase 0–1: site clearing (no materials — update_day leveling, not progress()).
    m.phases.push_back({0, monument_phase_resource{ARCHITECTS, 1}});
    m.phases.push_back({1, monument_phase_resource{ARCHITECTS, 1}});

    // Phase 2: carpenter scaffolding.
    const uint16_t timber = !timber_loads.empty() ? timber_loads[0] : 100;
    if (timber > 0) {
        m.phases.push_back({2, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, timber}});
    } else {
        m.phases.push_back({2, monument_phase_resource{ARCHITECTS, 1}});
    }

    // Phase 3: stonemasons carve obelisk (no new stone).
    m.phases.push_back({3, monument_phase_resource{ARCHITECTS, 1}});

    // Phase 4: vestibule / foretemple — +160 sandstone.
    uint16_t sand = build_sandstone > 0 ? build_sandstone : 160;
    if (!sandstone_loads.empty() && sandstone_loads[0] > 0) {
        sand = sandstone_loads[0];
    }
    m.phases.push_back({4, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_SANDSTONE, sand}});

    // Terminal.
    m.phases.push_back({5, monument_phase_resource{RESOURCE_NONE, 0}});
}

void building_sun_temple::static_params::archive_load(archive /*arch*/) {
    rebuild_construction();
}

const monument &building_sun_temple::config() const {
    return g_monument_sun_temple;
}

int building_sun_temple::placement_amount(e_resource r) const {
    const auto &bp = current_params();
    for (const auto &pr : bp.placement_resources) {
        if (pr.resource == r) {
            return pr.count;
        }
    }
    return 0;
}

int building_sun_temple::yards_available(e_resource r) {
    return g_city.resource.yards_stored_staffed(r);
}

bool building_sun_temple::has_unfinished_sun_temple() {
    for (building *b = building_begin(); b != building_end(); ++b) {
        if (!b || !b->is_valid() || b->type != BUILDING_SUN_TEMPLE) {
            continue;
        }
        auto *m = b->dcast_monument();
        if (m && m->is_unfinished()) {
            return true;
        }
    }
    return false;
}

int building_sun_temple::art_stage() const {
    const auto &bp = current_params();
    const int max_stage = bp.art_stages > 0 ? bp.art_stages : 4;
    const int p = runtime_data().phase;
    if (p == MONUMENT_FINISHED || p >= max_stage) {
        return max_stage;
    }
    if (p <= 1) {
        return 1;
    }
    return std::min(p, max_stage);
}

xstring building_sun_temple::anim_key_for(int stage) const {
    const auto &bp = current_params();
    const int max_stage = bp.art_stages > 0 ? bp.art_stages : 4;
    if (stage < 1) {
        stage = 1;
    }
    if (stage > max_stage) {
        stage = max_stage;
    }
    bstring32 key;
    key.printf("s%c", 'a' + (stage - 1));
    return xstring(key.c_str());
}

bool building_sun_temple::needs_resources() const {
    if (is_finished()) {
        return false;
    }
    auto &d = runtime_data();
    for (e_resource r = RESOURCES_MIN; r < RESOURCES_MAX; ++r) {
        if (needs_resource(r) <= 0) {
            continue;
        }
        if (d.resources_pct[r] < 100) {
            return true;
        }
    }
    return false;
}

bool building_sun_temple::need_stonemason() {
    if (is_finished()) {
        return false;
    }
    const int p = runtime_data().phase;
    return p >= 3 && p <= 4;
}

bool building_sun_temple::need_workers() const {
    if (is_finished()) {
        return false;
    }
    if (runtime_data().phase >= 2) {
        return false;
    }
    const auto &w = runtime_data().workers;
    return std::find(w.begin(), w.end(), 0) != w.end();
}

void building_sun_temple::add_workers(figure_id fid) {
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == 0) {
            wid = fid;
            return;
        }
    }
}

void building_sun_temple::remove_worker(figure_id fid) {
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == fid) {
            wid = 0;
            return;
        }
    }
}

int building_sun_temple::building_image_get() const {
    const xstring key = anim_key_for(art_stage());
    const auto &params = current_params();
    int img = params.first_img(key);
    if (img > 0) {
        return img;
    }
    return params.first_img("preview");
}

int building_sun_temple::preview::can_place(build_planner &p, tile2i /*tile*/, tile2i /*end*/, int state) const {
    if (state != CAN_PLACE) {
        return state;
    }
    if (has_unfinished_sun_temple()) {
        p.set_warning("#only_one_sun_temple_at_a_time");
        return CAN_NOT_PLACE;
    }
    int sandstone_need = 0;
    for (const auto &pr : building_sun_temple::current_params().placement_resources) {
        if (pr.resource == RESOURCE_SANDSTONE) {
            sandstone_need = pr.count;
            break;
        }
    }
    if (sandstone_need > 0 && yards_available(RESOURCE_SANDSTONE) < sandstone_need) {
        return CAN_NOT_PLACE;
    }
    return CAN_PLACE;
}

int building_sun_temple::preview::finalize_check(build_planner &p, tile2i tile, tile2i end, int state) const {
    state = building_planer_renderer::finalize_check(p, tile, end, state);
    if (state != CAN_PLACE) {
        return state;
    }
    if (has_unfinished_sun_temple()) {
        p.set_warning("#only_one_sun_temple_at_a_time");
        return CAN_NOT_PLACE;
    }
    int sandstone_need = 0;
    for (const auto &pr : building_sun_temple::current_params().placement_resources) {
        if (pr.resource == RESOURCE_SANDSTONE) {
            sandstone_need = pr.count;
            break;
        }
    }
    if (sandstone_need > 0 && yards_available(RESOURCE_SANDSTONE) < sandstone_need) {
        // gr19:88 has fixed wording — no printf substitution.
        p.set_warning("#need_220_blocks_of_sandstone_for_sun_temple");
        g_warning_manager.show("#need_220_blocks_of_sandstone_for_sun_temple");
        return CAN_NOT_PLACE;
    }
    return CAN_PLACE;
}

void building_sun_temple::preview::ghost_preview(build_planner &planer, painter &ctx, tile2i /*start*/, tile2i end, vec2i pixel) const {
    const auto &params = building_static_params::get(planer.build_type);
    const int size = params.building_size > 0 ? params.building_size : 10;
    blocked_tile_vec blocked;
    const bool is_blocked = !!planer.is_blocked_for_building(end, size, blocked);
    const int preview = params.first_img("preview");
    const int img = params.first_img("sa");
    if (is_blocked) {
        planer.draw_partially_blocked(ctx, false, blocked);
        return;
    }
    planer.draw_building_ghost(ctx, img > 0 ? img : preview, pixel);
}

void building_sun_temple::on_place(int orientation, int variant) {
    building_impl::on_place(orientation, variant);

    const int need = placement_amount(RESOURCE_SANDSTONE);
    if (need > 0) {
        events::emit(event_city_remove_resource{RESOURCE_SANDSTONE, need, /*staffed_only*/true});
    }
}

void building_sun_temple::on_place_update_tiles(int /*orientation*/, int /*variant*/) {
    const int size = base.size > 0 ? base.size : 10;
    map_building_tiles_add(id(), tile(), size, building_image_get(), TERRAIN_BUILDING);
}

void building_sun_temple::on_destroy() {
    building_monument_remove_all_deliveries(id());
}

void building_sun_temple::on_phase_changed(int /*old_phase*/, int current) {
    // Always refresh body art (core visible from place — no flat wipe).
    const int size = base.size > 0 ? base.size : 10;
    map_building_tiles_add(id(), tile(), size, building_image_get(), TERRAIN_BUILDING);

    if (current != MONUMENT_FINISHED) {
        auto &d = runtime_data();
        for (e_resource resource = RESOURCE_NONE; resource < RESOURCES_MAX; ++resource) {
            d.resources_pct[resource] = 0;
        }
    }

    // Drop laborer slots when leaving leveling — free guild/work-camp capacity.
    if (current >= 2) {
        auto &d = runtime_data();
        for (auto &wid : d.workers) {
            wid = 0;
        }
    }
}

void building_sun_temple::update_day() {
    building_impl::update_day();
    if (is_finished()) {
        return;
    }

    auto &d = runtime_data();
    if (d.phase < 2) {
        grid_tiles tiles = map_grid_get_tiles(&base, 0);
        tile2i tile2works = map_grid_area_first(tiles, [](tile2i t) {
            return map_monuments_get_progress(t) < 200;
        });
        const bool all_tiles_finished = (tile2works == tile2i{-1, -1});
        if (all_tiles_finished) {
            map_grid_area_foreach(tiles, [](tile2i t) { map_monuments_set_progress(t, 0); });
            set_phase(d.phase + 1);
        }
        return; // never progress() same day as leveling advance
    }

    const int phase_before = d.phase;
    progress();
    if (phase_before != MONUMENT_FINISHED && is_finished()) {
        city_message &message = city_message_post_with_popup_delay(
            MESSAGE_CAT_MONUMENTS, true, "sun_temple_congratulations", type(), tile().grid_offset());
        message.hide_img = true;
    }
}

void building_sun_temple::update_map_orientation(int /*map_orientation*/) {
    const int size = base.size > 0 ? base.size : 10;
    map_building_tiles_add(id(), tile(), size, building_image_get(), TERRAIN_BUILDING);
}

tile2i building_sun_temple::center_point() const {
    const int s = base.size > 0 ? base.size : 10;
    return tile().shifted(s / 2, s / 2);
}

tile2i building_sun_temple::access_point() const {
    return tile();
}

grid_area building_sun_temple::get_area() const {
    const int s = base.size > 0 ? base.size : 10;
    tile2i start = tile();
    tile2i end = start.shifted(s - 1, s - 1);
    return {start, end};
}

void building_sun_temple::bind_dynamic(io_buffer *iob, size_t /*version*/) {
    auto &monumentd = runtime_data();

    iob->bind____skip(38);
    iob->bind(BIND_SIGNATURE_UINT8, &base.orientation);
    for (int i = 0; i < 5; i++) {
        iob->bind(BIND_SIGNATURE_UINT16, &monumentd.workers[i]);
    }
    iob->bind(BIND_SIGNATURE_UINT8, &monumentd.phase);
    iob->bind(BIND_SIGNATURE_UINT8, &monumentd.funeral_done);
    iob->bind(BIND_SIGNATURE_UINT8, &monumentd.preexisting);
    iob->bind(BIND_SIGNATURE_UINT8, &monumentd.variant);

    for (int i = 0; i < RESOURCES_MAX; i++) {
        iob->bind(BIND_SIGNATURE_UINT8, &monumentd.resources_pct[i]);
    }
}
