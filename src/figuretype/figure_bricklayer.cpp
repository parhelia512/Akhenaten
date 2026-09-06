#include "figure_bricklayer.h"

#include "building/monument_mastaba.h"
#include "building/monument_pyramid.h"
#include "building/monuments.h"
#include "building/building_statue.h"
#include "grid/terrain.h"
#include "grid/grid.h"
#include "js/js_game.h"

REPLICATE_STATIC_PARAMS_FROM_CONFIG(figure_bricklayer);

const figure_bricklayer_action_tokens_t ANK_CONFIG_ENUM(figure_bricklayer_action_tokens)

void figure_bricklayer::figure_action() {
    base.use_cross_country = false;
    base.max_roam_length = 384;
    building* bhome = home();
    building* b_dest = destination();
    e_terrain_usage terrain_usage = TERRAIN_USAGE_ROADS;
    if (!bhome->is_valid() || !b_dest->is_valid()) {
        poof();
        return;
    }

    if (b_dest->is_monument()) {
        terrain_usage = TERRAIN_USAGE_PREFER_ROADS;
    } else {
        terrain_usage = TERRAIN_USAGE_ROADS;
    }

    switch (action_state()) {
    case ACTION_9_BRICKLAYER_DESTROY:
        poof();
        break;

    case ACTION_10_BRICKLAYER_CREATED_ROAMING:
        base.destination_tile = destination()->access_tile();
        advance_action(ACTION_11_BRICKLAYER_GOING_TO_STATUE);
        break;

    case ACTION_0_BRICKLAYER_CREATED:
        // Prefer monument access_point (enter_offset); access_tile alone can miss it.
        if (auto *mm = b_dest->dcast_monument()) {
            base.destination_tile = mm->access_point();
        } else {
            base.destination_tile = destination()->access_tile();
        }
        advance_action(ACTION_1_BRICKLAYER_GOING);
        break;

    case ACTION_11_BRICKLAYER_GOING_TO_STATUE:
        if (do_goto(base.destination_tile, terrain_usage, ACTION_12_BRICKLAYER_WORK_STATUE, ACTION_6_BRICKLAYER_RETURN_HOME)) {
            base.wait_ticks = 0;
            advance_action(ACTION_12_BRICKLAYER_WORK_STATUE);
        }
        break;

    case ACTION_1_BRICKLAYER_GOING:
        if (do_goto(base.destination_tile, terrain_usage, ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE, ACTION_6_BRICKLAYER_RETURN_HOME)) {
            advance_action(ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE);
        }
        break;

    case ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE:
        if (auto *mon = b_dest->dcast_monument(); mon && mon->is_finished()) {
            advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
            break;
        }
        // Mastaba: mastaba-specific sites. Mudbrick pyramids: same 2×2 pattern as
        // stonemasons — without this branch bricklayers freeze on LOOKING forever.
        if (smart_cast<building_mastaba>(b_dest)) {
            tile2i wait_tile = building_small_mastaba_bricks_waiting_tile(b_dest);
            if (!wait_tile.valid()) {
                poof();
                return;
            }

            map_grid_area_foreach(wait_tile.shifted(-1, -1), wait_tile, [] (tile2i t) {
                if (!map_monuments_get_progress(t)) {
                    map_monuments_set_progress(t, 1);
                }
            });

            base.destination_tile = wait_tile;
            advance_action(ACTION_2_BRICKLAYER_GOING_TO_PLACE);
        } else if (smart_cast<building_pyramid>(b_dest)) {
            tile2i wait_tile = building_monument_mason_waiting_tile(b_dest);
            if (!wait_tile.valid()) {
                advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
                break;
            }

            map_grid_area_foreach(wait_tile.shifted(-1, -1), wait_tile, [] (tile2i t) {
                if (!map_monuments_get_progress(t)) {
                    map_monuments_set_progress(t, 1);
                }
            });

            base.destination_tile = wait_tile;
            advance_action(ACTION_2_BRICKLAYER_GOING_TO_PLACE);
        } else {
            advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
        }
        break;

    case ACTION_2_BRICKLAYER_GOING_TO_PLACE:
        base.roam_wander_freely = false;
        if (do_goto(base.destination_tile, TERRAIN_USAGE_ANY, ACTION_3_BRICKLAYER_WAITING_RESOURCES, ACTION_6_BRICKLAYER_RETURN_HOME)) {
            base.wait_ticks = 0;
            runtime_data().idle_wait_count = 0;
            base.direction = 0;
            // Only claim idle tiles — never wipe mid-progress on resume after poof.
            map_grid_area_foreach(tile().shifted(-1, -1), tile(), [&] (tile2i t) {
                if (map_monuments_get_progress(t) == 0) {
                    map_monuments_set_progress(t, 1);
                }
            });
        }
        break;

    case ACTION_3_BRICKLAYER_WAITING_RESOURCES:
        if (auto *mon = b_dest->dcast_monument(); mon && mon->is_finished()) {
            advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
            break;
        }
        base.wait_ticks++;
        if (base.wait_ticks > 30) {
            auto &d = runtime_data();
            base.wait_ticks = 0;
            d.idle_wait_count++;
            int progress = map_monuments_get_progress(tile());
            if (progress >= 2 && progress < 200) {
                advance_action(ACTION_4_BRICKLAYER_LAY_BRICKS);
            } else {
                bool area_ready = true;
                map_grid_area_foreach(tile().shifted(-1, -1), tile(), [&] (tile2i t) { area_ready &= (map_monuments_get_progress(t) == 2); });
                if (area_ready) {
                    advance_action(ACTION_4_BRICKLAYER_LAY_BRICKS);
                } else if (d.idle_wait_count > 20) {
                    auto monument = destination()->dcast_monument();
                    if (monument) {
                        base.destination_tile = monument->access_point();
                        base.destination_tile.shift(1, 1);
                        advance_action(ACTION_7_BRICKLAYER_EXIT_FROM_MONUMENT);
                    }
                }
            }
        }
        break;

    case ACTION_4_BRICKLAYER_LAY_BRICKS: {
            if (auto *mon = b_dest->dcast_monument(); mon && mon->is_finished()) {
                advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
                break;
            }
            int progress = map_monuments_get_progress(tile());
            if (progress < 200) {
                map_grid_area_foreach(tile().shifted(-1, -1), tile(), [&] (tile2i t) {
                    map_monuments_set_progress(t, progress + 1);
                });
            } else {
                advance_action(ACTION_5_BRICKLAYER_LOOKING_FOR_IDLE_TILE);
            }
        }
        break;

    case ACTION_7_BRICKLAYER_EXIT_FROM_MONUMENT:
        base.roam_wander_freely = false;
        do_goto(base.destination_tile, TERRAIN_USAGE_ANY,
                ACTION_6_BRICKLAYER_RETURN_HOME, ACTION_6_BRICKLAYER_RETURN_HOME);
        break;

    case ACTION_12_BRICKLAYER_WORK_STATUE:
        base.wait_ticks++;
        if (base.wait_ticks > simulation_time_t::ticks_in_day * 2) {
            auto statue = smart_cast<building_statue>(building_get(runtime_data().destination_bid));
            if (statue) {
                statue->set_service(100);
            }
            advance_action(ACTION_6_BRICKLAYER_RETURN_HOME);
        }
        break;

    case ACTION_6_BRICKLAYER_RETURN_HOME:
        if (do_gotobuilding(home(), true, TERRAIN_USAGE_PREFER_ROADS,
                            ACTION_9_BRICKLAYER_DESTROY, ACTION_9_BRICKLAYER_DESTROY)) {
            poof();
        }
        break;
    }
}

void figure_bricklayer::update_animation() {
    figure_impl::update_animation();

    switch (action_state()) {
    case ACTION_3_BRICKLAYER_WAITING_RESOURCES:
        image_set_animation(animkeys().idle);
        break;

    case ACTION_4_BRICKLAYER_LAY_BRICKS:
    case ACTION_12_BRICKLAYER_WORK_STATUE:
        image_set_animation(animkeys().work);
        break;

    case ACTION_6_BRICKLAYER_RETURN_HOME:
        image_set_animation(animkeys().walk);
        break;
    }
}

void figure_bricklayer::on_destroy() {
    figure_impl::on_destroy();

    // Clear monument/statue worker slot. destination_bid is set at spawn; fall back
    // to figure destination when missing (older saves / mid-path). Same as stonemason —
    // without remove_worker, mudbrick pyramids leak slots and block further guild sends.
    building *b_dest = building_get(runtime_data().destination_bid);
    if (!b_dest || !b_dest->id) {
        b_dest = destination();
    }
    if (b_dest && b_dest->id) {
        b_dest->remove_figure_by_id(base.id);
        if (auto *mon = b_dest->dcast_monument()) {
            mon->remove_worker(base.id);
        }
    }
}
