#include "monument_abu_simbel.h"

#include "building/monuments.h"
#include "building/rotation.h"
#include "city/city.h"
#include "city/city_message.h"
#include "city/city_warnings.h"
#include "construction/build_planner.h"
#include "core/direction.h"
#include "figure/figure.h"
#include "game/undo.h"
#include "graphics/color.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "graphics/view/lookup.h"
#include "graphics/view/view.h"
#include "grid/building.h"
#include "grid/building_tiles.h"
#include "grid/figure.h"
#include "grid/grid.h"
#include "grid/image.h"
#include "grid/property.h"
#include "grid/road_access.h"
#include "grid/terrain.h"
#include "io/io_buffer.h"
#include "js/js_game.h"

#include <algorithm>

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_abu_simbel);

static monument g_monument_abu_simbel{BUILDING_ABU_SIMBEL};

// Cliff bulk may sit on elevation / access ramps (not plain rock quarry tiles).
static constexpr uint32_t k_cliff_terrain = TERRAIN_ELEVATION | TERRAIN_ACCESS_RAMP;

void building_abu_simbel::static_params::rebuild_construction() {
    monument &m = g_monument_abu_simbel;
    m.btype = BUILDING_ABU_SIMBEL;
    m.phases.clear();

    const int stages = art_stages > 0 ? art_stages : 8;
    for (int i = 0; i < stages; ++i) {
        const uint16_t timber = (i < (int)timber_loads.size()) ? timber_loads[i] : 0;
        if (timber > 0) {
            m.phases.push_back({(uint8_t)i, monument_phase_resource{ARCHITECTS, 1}, {RESOURCE_TIMBER, timber}});
        } else {
            m.phases.push_back({(uint8_t)i, monument_phase_resource{ARCHITECTS, 1}});
        }
    }
    m.phases.push_back({(uint8_t)stages, monument_phase_resource{RESOURCE_NONE, 0}});
}

void building_abu_simbel::static_params::archive_load(archive /*arch*/) {
    rebuild_construction();
}

const monument &building_abu_simbel::config() const {
    return g_monument_abu_simbel;
}

static vec2i abu_entrance_size(vec2i entrance) {
    if (entrance.x > 0 && entrance.y > 0) {
        return entrance;
    }
    const vec2i cfg = building_abu_simbel::current_params().entrance_size;
    return (cfg.x > 0 && cfg.y > 0) ? cfg : vec2i{3, 3};
}

static vec2i abu_oriented_bulk(vec2i init_tiles, int rotation) {
    if (init_tiles.x <= 0 || init_tiles.y <= 0) {
        const vec2i cfg = building_abu_simbel::current_params().init_tiles;
        init_tiles = (cfg.x > 0 && cfg.y > 0) ? cfg : vec2i{9, 21};
    }
    rotation %= 4;
    if (rotation == 1 || rotation == 3) {
        return {init_tiles.y, init_tiles.x};
    }
    return init_tiles;
}

static vec2i abu_total_size(vec2i bulk, vec2i entrance) {
    entrance = abu_entrance_size(entrance);
    return {bulk.x, bulk.y + entrance.y};
}

// Cursor `end` → map-NW of the axis-aligned place rectangle.
// Requires setup_preview_graphics to swap size.x/y for east/west camera so
// tile_coord_cache covers the same W×H region that construction_place claims.
static tile2i abu_map_origin(tile2i end, vec2i total) {
    switch (g_camera.orientation) {
    case DIR_2_BOTTOM_RIGHT: // east (size swapped in preview)
        return end.shifted(-total.x + 1, 0);
    case DIR_4_BOTTOM_LEFT: // south
        return end.shifted(-total.x + 1, -total.y + 1);
    case DIR_6_TOP_LEFT: // west (size swapped in preview)
        return end.shifted(0, -total.y + 1);
    default:
        return end;
    }
}

static void abu_local_xy(tile2i tile, tile2i origin, int *dx, int *dy) {
    *dx = tile.x() - origin.x();
    *dy = tile.y() - origin.y();
}

static void abu_preview_geometry(vec2i *bulk_out, vec2i *ent_out, vec2i *total_out) {
    const auto &bp = building_abu_simbel::current_params();
    const int rot = building_rotation_global_rotation() % 4;
    *bulk_out = abu_oriented_bulk(bp.init_tiles, rot);
    *ent_out = abu_entrance_size(bp.entrance_size);
    *total_out = abu_total_size(*bulk_out, *ent_out);
}

bool building_abu_simbel::is_bulk_local(int dx, int dy, vec2i bulk) {
    return dx >= 0 && dy >= 0 && dx < bulk.x && dy < bulk.y;
}

bool building_abu_simbel::is_entrance_local(int dx, int dy, vec2i bulk, vec2i entrance) {
    entrance = abu_entrance_size(entrance);
    if (entrance.x > bulk.x) {
        return false;
    }
    // Outside bulk, centered on width, on the far depth edge after footprint rotate.
    const int x0 = (bulk.x - entrance.x) / 2;
    const int y0 = bulk.y;
    return dx >= x0 && dx < x0 + entrance.x && dy >= y0 && dy < y0 + entrance.y;
}

bool building_abu_simbel::is_padding_local(int dx, int dy, vec2i bulk, vec2i entrance) {
    entrance = abu_entrance_size(entrance);
    if (dy < bulk.y || dy >= bulk.y + entrance.y) {
        return false;
    }
    return !is_entrance_local(dx, dy, bulk, entrance);
}

bool building_abu_simbel::has_abu_simbel_on_map() {
    for (building *b = building_begin(); b != building_end(); ++b) {
        if (b && b->is_valid() && b->type == BUILDING_ABU_SIMBEL) {
            return true;
        }
    }
    return false;
}

vec2i building_abu_simbel::bulk_size() const {
    return abu_oriented_bulk(current_params().init_tiles, base.orientation);
}

vec2i building_abu_simbel::total_size() const {
    return abu_total_size(bulk_size(), current_params().entrance_size);
}

int building_abu_simbel::art_stage() const {
    const auto &bp = current_params();
    const int max_stage = bp.art_stages > 0 ? bp.art_stages : 8;
    const int p = runtime_data().phase;
    if (p == MONUMENT_FINISHED || p >= max_stage) {
        return max_stage;
    }
    if (p < 1) {
        return 1;
    }
    return std::min(p + 1, max_stage);
}

int building_abu_simbel::art_orient_pair() const {
    // 2statue / midcut store two camera variants as even/odd frames.
    return (g_camera.orientation / 2) % 2;
}

xstring building_abu_simbel::anim_key_for(int stage) const {
    const auto &bp = current_params();
    const int max_stage = bp.art_stages > 0 ? bp.art_stages : 8;
    if (stage < 1) {
        stage = 1;
    }
    if (stage > max_stage) {
        stage = max_stage;
    }
    bstring32 key;
    // Finished art uses dedicated finish1/finish2 (last 2statue pair).
    if (is_finished()) {
        key.printf("finish%d", art_orient_pair() + 1);
        return xstring(key.c_str());
    }
    key.printf("s%c%d", 'a' + (stage - 1), art_orient_pair() + 1);
    return xstring(key.c_str());
}

bool building_abu_simbel::needs_resources() const {
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

bool building_abu_simbel::need_carpenter_phase() const {
    return !is_finished() && needs_resource(RESOURCE_TIMBER) > 0;
}

bool building_abu_simbel::has_stonemason_worker() const {
    for (auto wid : runtime_data().workers) {
        figure *f = wid > 0 ? figure_get(wid) : nullptr;
        if (f && f->is_alive() && f->type == FIGURE_STONEMASON) {
            return true;
        }
    }
    return false;
}

bool building_abu_simbel::has_carpenter_worker() const {
    for (auto wid : runtime_data().workers) {
        figure *f = wid > 0 ? figure_get(wid) : nullptr;
        if (f && f->is_alive() && f->type == FIGURE_CARPENTER) {
            return true;
        }
    }
    return false;
}

bool building_abu_simbel::need_stonemason() {
    if (is_finished() || need_carpenter_phase()) {
        return false;
    }
    // Free worker slot required — same gate as need_carpenter (avoid silent add_workers drops).
    if (!need_workers()) {
        return false;
    }
    return !has_stonemason_worker();
}

bool building_abu_simbel::need_workers() const {
    if (is_finished()) {
        return false;
    }
    for (auto wid : runtime_data().workers) {
        if (!wid) {
            return true;
        }
        figure *f = figure_get(wid);
        if (!f || !f->is_alive()) {
            return true;
        }
    }
    return false;
}

void building_abu_simbel::add_workers(figure_id fid) {
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == 0) {
            wid = fid;
            return;
        }
        figure *f = figure_get(wid);
        if (!f || !f->is_alive()) {
            wid = fid;
            return;
        }
    }
}

void building_abu_simbel::remove_worker(figure_id fid) {
    auto &d = runtime_data();
    for (auto &wid : d.workers) {
        if (wid == fid) {
            wid = 0;
            return;
        }
    }
}

int building_abu_simbel::building_image_get() const {
    const auto &params = current_params();
    const xstring key = anim_key_for(art_stage());
    int img = params.first_img(key);
    if (img > 0) {
        return img;
    }
    // Fallback: unoriented sa..sh keys removed — use preview / first 2statue.
    img = params.first_img("preview");
    if (img > 0) {
        return img;
    }
    return params.first_img("sa1");
}

bool building_abu_simbel::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    // Layered draw only from the main (NW) tile — other footprint tiles hold image_id 0.
    if (tile != base.tile) {
        return false;
    }

    const auto &params = current_params();
    const int stage = art_stage();
    const int cam = art_orient_pair();
    const vec2i bulk = bulk_size();

    auto draw_layer = [&](int image_id, vec2i pixel) {
        if (image_id <= 0) {
            return;
        }
        auto &command = ImageDraw::create_command(ctx, render_command_t::ert_drawtile);
        command.image_id = image_id;
        command.pixel = pixel;
        command.mask = color_mask;
    };

    // Niche / midcut: 14 frames = 7 progress × 2 orients (height shrinks as carve advances).
    const int mid_progress = std::min(std::max(stage - 1, 0), 6);
    const int mid_off = mid_progress * 2 + cam;
    const int mid_back = params.first_img("midcut_back");
    const int mid_front = params.first_img("midcut_front");
    if (mid_back > 0) {
        draw_layer(mid_back + mid_off, point);
    }
    if (mid_front > 0) {
        draw_layer(mid_front + mid_off, point);
    }

    // Second colossi pair — 2statue sprite is already two figures (~6 tiles wide);
    // place a twin along the bulk width for the full four-colossi façade.
    const int twin_dx = std::min(3, std::max(bulk.x - 1, 0));
    if (twin_dx > 0) {
        const vec2i twin_px = g_camera.lookup_tile_to_pixel(base.tile.shifted(twin_dx, 0));
        draw_layer(building_image_get(), twin_px);
    }

    // Scaffold ornaments during timber / early carve (matches GIF scaffold-heavy mid frames).
    if (!is_finished() && stage <= 6) {
        const char *scaffold_keys[] = {"scaffold_a", "scaffold_b", "scaffold_c", "scaffold_d"};
        const int n_scaf = (stage <= 2) ? 2 : 4;
        for (int i = 0; i < n_scaf; ++i) {
            const int scaf = params.first_img(scaffold_keys[i]);
            if (scaf <= 0) {
                continue;
            }
            // Spread along façade: feet of left/right statue pairs.
            const int sx = (bulk.x * (i + 1)) / (n_scaf + 1);
            const int sy = std::max(bulk.y - 2, 0);
            const vec2i scaf_px = g_camera.lookup_tile_to_pixel(base.tile.shifted(sx, sy));
            // Small overlay — nudge up so ladders sit against the cliff face.
            draw_layer(scaf, scaf_px + vec2i{8, -48});
        }
    }

    // Entrance stairs on the clear-land protrusion.
    if (stage >= 2) {
        const int stairs_base = params.first_img("stairs");
        if (stairs_base > 0) {
            const int stairs_off = std::min(stage - 2, 5);
            const vec2i stairs_px = g_camera.lookup_tile_to_pixel(access_point());
            draw_layer(stairs_base + stairs_off, stairs_px);
        }
    }

    return true;
}

static void abu_simbel_add_tile(int building_id, tile2i origin, int dx, int dy, int image_id, bool is_main) {
    tile2i t = origin.shifted(dx, dy);
    if (!map_grid_is_inside(t, 1)) {
        return;
    }
    const int grid_offset = t.grid_offset();
    map_terrain_remove(grid_offset, TERRAIN_CLEARABLE);
    map_terrain_add(grid_offset, TERRAIN_BUILDING);
    map_building_set(grid_offset, building_id);
    map_property_clear_constructing(grid_offset);
    map_property_set_multi_tile_size(grid_offset, 1);
    map_image_set(grid_offset, is_main ? image_id : 0);
    map_property_set_multi_tile_xy(grid_offset, dx, dy, is_main);
}

static void abu_simbel_place_tiles(int building_id, tile2i origin, vec2i bulk, vec2i entrance, int image_id) {
    entrance = abu_entrance_size(entrance);
    for (int dy = 0; dy < bulk.y; dy++) {
        for (int dx = 0; dx < bulk.x; dx++) {
            abu_simbel_add_tile(building_id, origin, dx, dy, image_id, dx == 0 && dy == 0);
        }
    }
    const int x0 = (bulk.x - entrance.x) / 2;
    for (int dy = 0; dy < entrance.y; dy++) {
        for (int dx = 0; dx < entrance.x; dx++) {
            abu_simbel_add_tile(building_id, origin, x0 + dx, bulk.y + dy, image_id, false);
        }
    }
    map_property_mark_draw_tile(origin.grid_offset());
}

static bool abu_tile_is_cliff(tile2i t) {
    return map_terrain_is(t, k_cliff_terrain);
}

static int abu_validate_cliff_entrance(tile2i origin, vec2i bulk, vec2i entrance, xstring *warning_out) {
    entrance = abu_entrance_size(entrance);
    const vec2i total = abu_total_size(bulk, entrance);
    bool missing_cliff = false;
    bool bad_entrance = false;
    for (int dy = 0; dy < total.y; dy++) {
        for (int dx = 0; dx < total.x; dx++) {
            if (building_abu_simbel::is_padding_local(dx, dy, bulk, entrance)) {
                continue; // side pads beside protrusion are not claimed / not validated
            }
            tile2i t = origin.shifted(dx, dy);
            if (!map_grid_is_inside(t, 1)) {
                if (building_abu_simbel::is_entrance_local(dx, dy, bulk, entrance)) {
                    bad_entrance = true;
                } else {
                    missing_cliff = true;
                }
                continue;
            }
            if (building_abu_simbel::is_entrance_local(dx, dy, bulk, entrance)) {
                if (map_terrain_is(t, TERRAIN_NOT_CLEAR)) {
                    bad_entrance = true;
                }
            } else if (!abu_tile_is_cliff(t)) {
                missing_cliff = true;
            }
        }
    }
    if (missing_cliff) {
        if (warning_out) {
            *warning_out = "#must_be_over_cliffs";
        }
        return CAN_NOT_PLACE;
    }
    if (bad_entrance) {
        if (warning_out) {
            *warning_out = "#entrance_on_clear_land";
        }
        return CAN_NOT_PLACE;
    }
    return CAN_PLACE;
}

void building_abu_simbel::preview::setup_preview_graphics(build_planner &planer) const {
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    // East/west camera rotate row/col vs map axes — swap planner size so
    // tile_coord_cache spans the same W×H rectangle construction_place uses.
    const int cam = g_camera.orientation / 2;
    if (cam == 1 || cam == 3) {
        planer.init_tiles(total.y, total.x);
    } else {
        planer.init_tiles(total.x, total.y);
    }
}

uint32_t building_abu_simbel::preview::ghost_ignore_terrain(build_planner &p, tile2i tile) const {
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    const tile2i origin = abu_map_origin(p.end, total);
    int dx = 0, dy = 0;
    abu_local_xy(tile, origin, &dx, &dy);
    if (dx < 0 || dy < 0 || dx >= total.x || dy >= total.y) {
        return 0;
    }
    if (building_abu_simbel::is_padding_local(dx, dy, bulk, ent)) {
        // Planner AABB includes side pads; ignore all terrain so they do not block.
        return TERRAIN_NOT_CLEAR;
    }
    if (building_abu_simbel::is_entrance_local(dx, dy, bulk, ent)) {
        return 0; // entrance must stay clear land
    }
    // Bulk: allow cliff elevation under the footprint.
    return k_cliff_terrain;
}

bool building_abu_simbel::preview::ghost_allow_tile(build_planner &p, tile2i tile) const {
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    const tile2i origin = abu_map_origin(p.end, total);
    int dx = 0, dy = 0;
    abu_local_xy(tile, origin, &dx, &dy);
    if (dx >= 0 && dy >= 0 && dx < total.x && dy < total.y
        && building_abu_simbel::is_padding_local(dx, dy, bulk, ent)) {
        return true; // padding not claimed — figures there must not block place
    }
    return building_planer_renderer::ghost_allow_tile(p, tile);
}

int building_abu_simbel::preview::construction_place(build_planner &planer, tile2i /*start*/, tile2i end, int orientation, int variant) const {
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    end = abu_map_origin(end, total);

    planer.last_created_building = nullptr;
    building *b = building_create(planer.build_type, end, orientation);
    game_undo_add_building(b);
    if (b->id <= 0) {
        return 0;
    }

    add_building(b, orientation, variant);
    planer.last_created_building = b;
    return 1;
}

int building_abu_simbel::preview::can_place(build_planner &p, tile2i /*tile*/, tile2i end, int state) const {
    if (state != CAN_PLACE && state != CAN_NOT_BUT_GREEN) {
        return state;
    }
    if (has_abu_simbel_on_map()) {
        p.set_warning("#only_one_building_of_this_type");
        return CAN_NOT_PLACE;
    }
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    const tile2i origin = abu_map_origin(end, total);
    xstring warn;
    const int cliff_state = abu_validate_cliff_entrance(origin, bulk, ent, &warn);
    if (cliff_state != CAN_PLACE) {
        if (!warn.empty()) {
            p.set_warning(warn);
        }
        return CAN_NOT_PLACE;
    }
    if (state == CAN_NOT_BUT_GREEN) {
        return state;
    }
    return CAN_PLACE;
}

int building_abu_simbel::preview::finalize_check(build_planner &p, tile2i tile, tile2i end, int state) const {
    state = building_planer_renderer::finalize_check(p, tile, end, state);
    // Do not promote CAN_NOT_BUT_GREEN → CAN_PLACE (occupied / blocked tiles).
    if (state != CAN_PLACE) {
        return state;
    }
    if (has_abu_simbel_on_map()) {
        p.set_warning("#only_one_building_of_this_type");
        g_warning_manager.show("#only_one_building_of_this_type");
        return CAN_NOT_PLACE;
    }
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    const tile2i origin = abu_map_origin(end, total);
    xstring warn;
    const int cliff_state = abu_validate_cliff_entrance(origin, bulk, ent, &warn);
    if (cliff_state != CAN_PLACE) {
        if (!warn.empty()) {
            p.set_warning(warn);
            g_warning_manager.show(warn.c_str());
        }
        return CAN_NOT_PLACE;
    }
    return CAN_PLACE;
}

void building_abu_simbel::preview::ghost_preview(build_planner &planer, painter &ctx, tile2i /*start*/, tile2i end, vec2i /*pixel*/) const {
    // Do not use blocked_tile_vec (cap 36) — footprint is ~9×24 claimed tiles.
    const auto &params = building_static_params::get(planer.build_type);
    vec2i bulk, ent, total;
    abu_preview_geometry(&bulk, &ent, &total);
    tile2i origin = abu_map_origin(end, total);

    const bool force_red = has_abu_simbel_on_map();
    bool any_blocked = force_red;
    for (int dy = 0; dy < total.y && !any_blocked; dy++) {
        for (int dx = 0; dx < total.x; dx++) {
            if (building_abu_simbel::is_padding_local(dx, dy, bulk, ent)) {
                continue;
            }
            tile2i t = origin.shifted(dx, dy);
            const bool is_ent = building_abu_simbel::is_entrance_local(dx, dy, bulk, ent);
            bool tile_blocked = !map_grid_is_inside(t, 1) || map_has_figure_at(t);
            if (is_ent) {
                tile_blocked = tile_blocked || map_terrain_is(t, TERRAIN_NOT_CLEAR);
            } else {
                const uint32_t mask = TERRAIN_NOT_CLEAR & ~k_cliff_terrain;
                tile_blocked = tile_blocked || map_terrain_is(t, mask) || !abu_tile_is_cliff(t);
            }
            if (tile_blocked) {
                any_blocked = true;
                break;
            }
        }
    }

    if (any_blocked) {
        for (int dy = 0; dy < total.y; dy++) {
            for (int dx = 0; dx < total.x; dx++) {
                if (building_abu_simbel::is_padding_local(dx, dy, bulk, ent)) {
                    continue;
                }
                tile2i t = origin.shifted(dx, dy);
                vec2i px = g_camera.lookup_tile_to_pixel(t);
                const bool is_ent = building_abu_simbel::is_entrance_local(dx, dy, bulk, ent);
                bool tile_blocked = force_red || !map_grid_is_inside(t, 1) || map_has_figure_at(t);
                if (!force_red) {
                    if (is_ent) {
                        tile_blocked = tile_blocked || map_terrain_is(t, TERRAIN_NOT_CLEAR);
                    } else {
                        const uint32_t mask = TERRAIN_NOT_CLEAR & ~k_cliff_terrain;
                        tile_blocked = tile_blocked || map_terrain_is(t, mask) || !abu_tile_is_cliff(t);
                    }
                }
                build_planner::draw_flat_tile(ctx, px, tile_blocked ? COLOR_MASK_RED_30 : COLOR_MASK_GREEN_30);
            }
        }
        return;
    }

    const int preview = params.first_img("preview");
    const int img = params.first_img("sa1");
    // Anchor sprite on map-NW (same as place), not the raw cursor tile.
    const vec2i origin_pixel = g_camera.lookup_tile_to_pixel(origin);
    planer.draw_building_ghost(ctx, img > 0 ? img : preview, origin_pixel);
}

void building_abu_simbel::on_place_update_tiles(int /*orientation*/, int /*variant*/) {
    // Footprint orientation must match preview (global_rotation), not camera-absolute.
    base.orientation = (uint8_t)(building_rotation_global_rotation() % 4);
    const vec2i total = total_size();
    // Square size = max edge so rubble/remove defensive sweep covers the full AABB.
    // Road access uses rectangular lookup (city_maintenance + on_place_checks).
    base.size = (uint8_t)std::max({total.x, total.y, 1});
    abu_simbel_place_tiles(id(), tile(), bulk_size(), current_params().entrance_size, building_image_get());
    // Clear leftover monument progress on claimed tiles (not padding).
    const vec2i bulk = bulk_size();
    const vec2i ent = abu_entrance_size(current_params().entrance_size);
    for (int dy = 0; dy < total.y; dy++) {
        for (int dx = 0; dx < total.x; dx++) {
            if (is_padding_local(dx, dy, bulk, ent)) {
                continue;
            }
            map_monuments_set_progress(tile().shifted(dx, dy), 0);
        }
    }
}

void building_abu_simbel::on_place_checks() {
    construction_warnings warnings;
    const vec2i total = total_size();
    int min_value = 12;
    int min_go = tile().grid_offset();
    const bool has_road = map_road_find_minimum_tile_xy(tile(), total.x, total.y, &min_value, &min_go) && min_value < 12;
    warnings.add_if(!has_road, "#needs_road_access");
}

xstring building_abu_simbel::demolish_blocked_message() const {
    return "#abu_simbel_not_demolishable";
}

void building_abu_simbel::on_destroy() {
    building_monument_remove_all_deliveries(id());
    // Clear claimed tiles by building_id over the total AABB (padding never claimed).
    const vec2i total = total_size();
    for (int dy = 0; dy < total.y; dy++) {
        for (int dx = 0; dx < total.x; dx++) {
            tile2i t = tile().shifted(dx, dy);
            if (!map_grid_is_inside(t, 1)) {
                continue;
            }
            map_monuments_set_progress(t, 0);
            if (map_building_at(t) == id()) {
                map_building_tile_clear_at(t.grid_offset(), type());
            }
        }
    }
}

void building_abu_simbel::on_phase_changed(int /*old_phase*/, int current) {
    abu_simbel_place_tiles(id(), tile(), bulk_size(), current_params().entrance_size, building_image_get());

    if (current != MONUMENT_FINISHED) {
        auto &d = runtime_data();
        for (e_resource resource = RESOURCE_NONE; resource < RESOURCES_MAX; ++resource) {
            d.resources_pct[resource] = 0;
        }
    }
}

void building_abu_simbel::update_day() {
    building_impl::update_day();
    if (is_finished()) {
        return;
    }
    // Drop stale worker ids so dead figures do not block need_workers / progress.
    for (auto &wid : runtime_data().workers) {
        if (!wid) {
            continue;
        }
        figure *f = figure_get(wid);
        if (!f || !f->is_alive()) {
            wid = 0;
        }
    }
    // Sequential: timber phases need carpenter on-site; mason phases need stonemason.
    if (need_carpenter_phase()) {
        if (!has_carpenter_worker()) {
            return;
        }
    } else if (!has_stonemason_worker()) {
        return;
    }
    progress();
}

void building_abu_simbel::update_map_orientation(int /*map_orientation*/) {
    abu_simbel_place_tiles(id(), tile(), bulk_size(), current_params().entrance_size, building_image_get());
}

tile2i building_abu_simbel::center_point() const {
    const vec2i bulk = bulk_size();
    return tile().shifted(bulk.x / 2, bulk.y / 2);
}

tile2i building_abu_simbel::access_point() const {
    // Mid-tile of the clear-land entrance protrusion (outside bulk).
    const vec2i bulk = bulk_size();
    const vec2i ent = abu_entrance_size(current_params().entrance_size);
    const int x0 = (bulk.x - ent.x) / 2;
    return tile().shifted(x0 + ent.x / 2, bulk.y + ent.y / 2);
}

grid_area building_abu_simbel::get_area() const {
    const vec2i total = total_size();
    tile2i start = tile();
    tile2i end = start.shifted(total.x - 1, total.y - 1);
    return {start, end};
}

void building_abu_simbel::bind_dynamic(io_buffer *iob, size_t /*version*/) {
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
