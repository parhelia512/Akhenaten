#include "figure/figure.h"

#include "building/building.h"
#include "city/emperor.h"
#include "core/random.h"
#include "empire/city.h"
#include "figure/name.h"
#include "figure/route.h"
#include "figure/trader.h"
#include "map/figure.h"
#include "map/grid.h"

#include <string.h>
#include <game/io/io_buffer.h>

static struct {
    int created_sequence;
    bool initialized;
    figure *figures[5000];
} data = {0, false};

figure *figure_get(int id) {
    return data.figures[id];
}
figure *figure_create(int type, int x, int y, int dir) {
    int id = 0;
    for (int i = 1; i < MAX_FIGURES[GAME_ENV]; i++) {
        if (figure_get(i)->available()) {
            id = i;
            break;
        }
    }
    if (!id)
        return figure_get(0);

    figure *f = figure_get(id);
    f->state = FIGURE_STATE_ALIVE;
    f->faction_id = 1;
    f->type = type;
    f->use_cross_country = false;
    f->terrain_usage = -1;
    f->is_friendly = true;
    f->created_sequence = data.created_sequence++;
    f->direction = dir;
//    f->direction = DIR_FIGURE_NONE;
    f->roam_length = 0;
    f->source_x = f->destination_x = f->previous_tile_x = f->tile_x = x;
    f->source_y = f->destination_y = f->previous_tile_y = f->tile_y = y;
    f->destination_x = 0;
    f->destination_y = 0;
    f->grid_offset_figure = map_grid_offset(x, y);
    f->cross_country_x = 15 * x;
    f->cross_country_y = 15 * y;
    f->progress_on_tile = 14;
    f->phrase_sequence_city = f->phrase_sequence_exact = random_byte() & 3;
    f->name = figure_name_get(type, 0);
    f->map_figure_add();
    if (type == FIGURE_TRADE_CARAVAN || type == FIGURE_TRADE_SHIP)
        f->trader_id = trader_create();

    return f;
}
void figure::figure_delete_UNSAFE() {
    if (has_home()) {
        building *b = home();
        if (b->has_figure(0, id))
            b->remove_figure(0);
        if (b->has_figure(1, id))
            b->remove_figure(1);
    }

    switch (type) {
        case FIGURE_BALLISTA:
            if (has_home())
                home()->remove_figure(3);
            break;
        case FIGURE_DOCKER:
            if (has_home()) {
                building *b = home();
                for (int i = 0; i < 3; i++) {
                    if (b->data.dock.docker_ids[i] == id)
                        b->data.dock.docker_ids[i] = 0;
                }
            }
            break;
        case FIGURE_ENEMY_CAESAR_LEGIONARY:
            city_emperor_mark_soldier_killed();
            break;
    }
    if (empire_city_id)
        empire_city_remove_trader(empire_city_id, id);

    if (has_immigrant_home())
        immigrant_home()->remove_figure(2);

    route_remove();
    map_figure_remove();

    int figure_id = id;
    state = FIGURE_STATE_NONE;
    memset(this, 0, sizeof(figure));
    id = figure_id;
}

bool figure::is_dead() {
    return state != FIGURE_STATE_ALIVE || action_state == FIGURE_ACTION_149_CORPSE;
}
bool figure::is_enemy() {
    return type >= FIGURE_ENEMY43_SPEAR && type <= FIGURE_ENEMY_CAESAR_LEGIONARY;
}
bool figure::is_legion() {
    return type >= FIGURE_FORT_JAVELIN && type <= FIGURE_FORT_LEGIONARY;
}
bool figure::is_herd() {
    return type >= FIGURE_SHEEP && type <= FIGURE_ZEBRA;
}

building *figure::home() {
    return building_get(home_building_id);
};
building *figure::destination() {
    return building_get(destination_building_id);
};
building *figure::immigrant_home() {
    return building_get(immigrant_home_building_id);
};
void figure::set_home(int _id) {
    home_building_id = _id;
};
void figure::set_immigrant_home(int _id) {
    immigrant_home_building_id = _id;
};
void figure::set_destination(int _id) {
    destination_building_id = _id;
};
void figure::set_home(building *b) {
    home_building_id = b->id;
};
void figure::set_immigrant_home(building *b) {
    immigrant_home_building_id = b->id;
};
void figure::set_destination(building *b) {
    destination_building_id = b->id;
};
bool figure::has_home(int _id) {
    if (_id == -1)
        return  (home_building_id != 0);
    return (home_building_id == _id);
}
bool figure::has_home(building *b) {
    return (b == home());
}
bool figure::has_immigrant_home(int _id) {
    if (_id == -1)
        return (immigrant_home_building_id != 0);
    return (immigrant_home_building_id == _id);
}
bool figure::has_immigrant_home(building *b) {
    return (b == immigrant_home());
}
bool figure::has_destination(int _id) {
    if (_id == -1)
        return (destination_building_id != 0);
    return (destination_building_id == _id);
}
bool figure::has_destination(building *b) {
    return (b == destination());
}

//bool figure::is_roamer() {
//    switch (action_state) {
//        case ACTION_1_ROAMING:
//        case ACTION_10_GOING:
//        case FIGURE_ACTION_125_ROAMING:
//        case FIGURE_ACTION_42_TAX_COLLECTOR_ROAMING:
//        case FIGURE_ACTION_62_ENGINEER_ROAMING:
//        case FIGURE_ACTION_72_PREFECT_ROAMING:
//        case FIGURE_ACTION_94_ENTERTAINER_ROAMING:
//            return true;
//    }
//    return false;
//}

void init_figures() {
    if (!data.initialized) {
        for (int i = 0; i < MAX_FIGURES[GAME_ENV]; i++)
            data.figures[i] = new figure(i);
        data.initialized = true;
    }
}
void figure_init_scenario(void) {
    init_figures();
    data.created_sequence = 0;
}
void figure_kill_all() {
    for (int i = 1; i < MAX_FIGURES[GAME_ENV]; i++)
        figure_get(i)->poof();
}
void figure::bind(io_buffer *iob) {
    figure *f = this;
    iob->bind(BIND_SIGNATURE_UINT8, &f->alternative_location_index);
    iob->bind(BIND_SIGNATURE_UINT8, &f->anim_frame);
    iob->bind(BIND_SIGNATURE_UINT8, &f->is_enemy_image);
    iob->bind(BIND_SIGNATURE_UINT8, &f->flotsam_visible);

//    f->sprite_image_id = buf->read_i16() + 18;
    f->sprite_image_id -= 18;
    iob->bind(BIND_SIGNATURE_INT16, &f->sprite_image_id);
    f->sprite_image_id += 18;

    if (GAME_ENV == ENGINE_ENV_C3)
        iob->bind(BIND_SIGNATURE_INT16, &f->cart_image_id);
    else if (GAME_ENV == ENGINE_ENV_PHARAOH)
        iob->bind____skip(2);
    iob->bind(BIND_SIGNATURE_INT16, &f->next_figure);
    iob->bind(BIND_SIGNATURE_UINT8, &f->type);
    iob->bind(BIND_SIGNATURE_UINT8, &f->resource_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->use_cross_country);
    iob->bind(BIND_SIGNATURE_UINT8, &f->is_friendly);
    iob->bind(BIND_SIGNATURE_UINT8, &f->state);
    iob->bind(BIND_SIGNATURE_UINT8, &f->faction_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->action_state_before_attack);
    iob->bind(BIND_SIGNATURE_INT8, &f->direction);
    iob->bind(BIND_SIGNATURE_INT8, &f->previous_tile_direction);
    iob->bind(BIND_SIGNATURE_INT8, &f->attack_direction);
    if (GAME_ENV == ENGINE_ENV_PHARAOH) {
        iob->bind(BIND_SIGNATURE_UINT16, &f->tile_x);
        iob->bind(BIND_SIGNATURE_UINT16, &f->tile_y);
        iob->bind(BIND_SIGNATURE_UINT16, &f->previous_tile_x);
        iob->bind(BIND_SIGNATURE_UINT16, &f->previous_tile_y);
        iob->bind(BIND_SIGNATURE_UINT16, &f->missile_damage);
        iob->bind(BIND_SIGNATURE_UINT16, &f->damage);
    } else {
        iob->bind(BIND_SIGNATURE_UINT8, &f->tile_x);
        iob->bind(BIND_SIGNATURE_UINT8, &f->tile_y);
        iob->bind(BIND_SIGNATURE_UINT8, &f->previous_tile_x);
        iob->bind(BIND_SIGNATURE_UINT8, &f->previous_tile_y);
        iob->bind(BIND_SIGNATURE_UINT8, &f->missile_damage);
        iob->bind(BIND_SIGNATURE_UINT8, &f->damage);
    }
    if (GAME_ENV == ENGINE_ENV_PHARAOH) {
        iob->bind(BIND_SIGNATURE_INT32, &f->grid_offset_figure);
        iob->bind(BIND_SIGNATURE_UINT16, &f->destination_x);
        iob->bind(BIND_SIGNATURE_UINT16, &f->destination_y);
        iob->bind(BIND_SIGNATURE_INT32, &f->destination_grid_offset);
        iob->bind(BIND_SIGNATURE_UINT16, &f->source_x);
        iob->bind(BIND_SIGNATURE_UINT16, &f->source_y);
        iob->bind(BIND_SIGNATURE_UINT16, &f->formation_position_x.soldier);
        iob->bind(BIND_SIGNATURE_UINT16, &f->formation_position_y.soldier);
    } else {
        iob->bind(BIND_SIGNATURE_INT16, &f->grid_offset_figure);
        iob->bind(BIND_SIGNATURE_UINT8, &f->destination_x);
        iob->bind(BIND_SIGNATURE_UINT8, &f->destination_y);
        iob->bind(BIND_SIGNATURE_INT16, &f->destination_grid_offset);
        iob->bind(BIND_SIGNATURE_UINT8, &f->source_x);
        iob->bind(BIND_SIGNATURE_UINT8, &f->source_y);
        iob->bind(BIND_SIGNATURE_UINT8, &f->formation_position_x.soldier);
        iob->bind(BIND_SIGNATURE_UINT8, &f->formation_position_y.soldier);
    }
    iob->bind(BIND_SIGNATURE_INT16, &f->__unused_24); // 0
    iob->bind(BIND_SIGNATURE_INT16, &f->wait_ticks); // 0
    iob->bind(BIND_SIGNATURE_UINT8, &f->action_state); // 9
    iob->bind(BIND_SIGNATURE_UINT8, &f->progress_on_tile); // 11
    iob->bind(BIND_SIGNATURE_INT16, &f->routing_path_id); // 12
    iob->bind(BIND_SIGNATURE_INT16, &f->routing_path_current_tile); // 4
    iob->bind(BIND_SIGNATURE_INT16, &f->routing_path_length); // 28
    iob->bind(BIND_SIGNATURE_UINT8, &f->in_building_wait_ticks); // 0
    iob->bind(BIND_SIGNATURE_UINT8, &f->outside_road_ticks); // 1
    iob->bind(BIND_SIGNATURE_INT16, &f->max_roam_length);
    iob->bind(BIND_SIGNATURE_INT16, &f->roam_length);
    iob->bind(BIND_SIGNATURE_UINT8, &f->roam_wander_freely);
    iob->bind(BIND_SIGNATURE_UINT8, &f->roam_random_counter);
    iob->bind(BIND_SIGNATURE_INT8, &f->roam_turn_direction);
    iob->bind(BIND_SIGNATURE_INT8, &f->roam_ticks_until_next_turn); // 0 ^^^^
    iob->bind(BIND_SIGNATURE_INT16, &f->cross_country_x);
    iob->bind(BIND_SIGNATURE_INT16, &f->cross_country_y);
    iob->bind(BIND_SIGNATURE_INT16, &f->cc_destination_x);
    iob->bind(BIND_SIGNATURE_INT16, &f->cc_destination_y);
    iob->bind(BIND_SIGNATURE_INT16, &f->cc_delta_x);
    iob->bind(BIND_SIGNATURE_INT16, &f->cc_delta_y);
    iob->bind(BIND_SIGNATURE_INT16, &f->cc_delta_xy);
    iob->bind(BIND_SIGNATURE_UINT8, &f->cc_direction);
    iob->bind(BIND_SIGNATURE_UINT8, &f->speed_multiplier);
    iob->bind(BIND_SIGNATURE_INT16, &f->home_building_id);
    iob->bind(BIND_SIGNATURE_INT16, &f->immigrant_home_building_id);
    iob->bind(BIND_SIGNATURE_INT16, &f->destination_building_id);
    iob->bind(BIND_SIGNATURE_INT16, &f->formation_id); // formation: 10
    iob->bind(BIND_SIGNATURE_UINT8, &f->index_in_formation); // 3
    iob->bind(BIND_SIGNATURE_UINT8, &f->formation_at_rest);
    iob->bind(BIND_SIGNATURE_UINT8, &f->migrant_num_people);
    iob->bind(BIND_SIGNATURE_UINT8, &f->is_ghost);
    iob->bind(BIND_SIGNATURE_UINT8, &f->min_max_seen);
    iob->bind(BIND_SIGNATURE_UINT8, &f->__unused_57);
    iob->bind(BIND_SIGNATURE_INT16, &f->leading_figure_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->attack_image_offset);
    iob->bind(BIND_SIGNATURE_UINT8, &f->wait_ticks_missile);
    iob->bind(BIND_SIGNATURE_INT8, &f->x_offset_cart);
    iob->bind(BIND_SIGNATURE_INT8, &f->y_offset_cart);
    iob->bind(BIND_SIGNATURE_UINT8, &f->empire_city_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->trader_amount_bought);
    iob->bind(BIND_SIGNATURE_INT16, &f->name); // 6
    iob->bind(BIND_SIGNATURE_UINT8, &f->terrain_usage);
    iob->bind(BIND_SIGNATURE_UINT8, &f->is_boat);
    iob->bind(BIND_SIGNATURE_UINT16, &f->resource_amount_full); // 4772 >>>> 112 (resource amount! 2-bytes)
    iob->bind(BIND_SIGNATURE_UINT8, &f->height_adjusted_ticks);
    iob->bind(BIND_SIGNATURE_UINT8, &f->current_height);
    iob->bind(BIND_SIGNATURE_UINT8, &f->target_height);
    iob->bind(BIND_SIGNATURE_UINT8, &f->collecting_item_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->trade_ship_failed_dock_attempts);
    iob->bind(BIND_SIGNATURE_UINT8, &f->phrase_sequence_exact);
    iob->bind(BIND_SIGNATURE_INT8, &f->phrase_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->phrase_sequence_city);
    iob->bind(BIND_SIGNATURE_UINT8, &f->__unused_6f);
    iob->bind(BIND_SIGNATURE_UINT8, &f->trader_id);
    iob->bind(BIND_SIGNATURE_UINT8, &f->wait_ticks_next_target);
    iob->bind(BIND_SIGNATURE_INT16, &f->target_figure_id);
    iob->bind(BIND_SIGNATURE_INT16, &f->targeted_by_figure_id);
    iob->bind(BIND_SIGNATURE_UINT16, &f->created_sequence);
    iob->bind(BIND_SIGNATURE_UINT16, &f->target_figure_created_sequence);
//    iob->bind(BIND_SIGNATURE_UINT8, &f->figures_sametile_num);
    iob->bind____skip(1);
    iob->bind(BIND_SIGNATURE_UINT8, &f->num_attackers);
    iob->bind(BIND_SIGNATURE_INT16, &f->attacker_id1);
    iob->bind(BIND_SIGNATURE_INT16, &f->attacker_id2);
    iob->bind(BIND_SIGNATURE_INT16, &f->opponent_id);
    if (GAME_ENV == ENGINE_ENV_PHARAOH) {
//        iob->bind____skip(239);
        iob->bind____skip(7);
        iob->bind(BIND_SIGNATURE_INT16, &f->unk_ph1_269); // 269
        iob->bind(BIND_SIGNATURE_INT16, &f->unk_ph2_00); // 0
        iob->bind(BIND_SIGNATURE_INT32, &f->market_lady_resource_image_offset); // 03 00 00 00
        iob->bind____skip(12); // FF FF FF FF FF ...
        iob->bind(BIND_SIGNATURE_INT16, &f->market_lady_returning_home_id); // 26
        iob->bind____skip(14); // 00 00 00 00 00 00 00 ...
        iob->bind(BIND_SIGNATURE_INT16, &f->market_lady_bought_amount); // 200
        iob->bind____skip(115);
        iob->bind(BIND_SIGNATURE_INT8, &f->unk_ph3_6); // 6
        iob->bind(BIND_SIGNATURE_INT16, &f->unk_ph4_ffff); // -1
        iob->bind____skip(48);
        iob->bind(BIND_SIGNATURE_INT8, &f->festival_remaining_dances);
        iob->bind____skip(27);

        f->cart_image_id -= 18;
        iob->bind(BIND_SIGNATURE_INT16, &f->cart_image_id);
        f->cart_image_id += 18;

        iob->bind____skip(2);
    }
}
io_buffer *iob_figures = new io_buffer([](io_buffer *iob) {
    init_figures();
    for (int i = 0; i < MAX_FIGURES[GAME_ENV]; i++) {
        figure_get(i)->bind(iob); // doing this because some members are PRIVATE.
        figure_get(i)->id = i;
    }
});
io_buffer *iob_figure_sequence = new io_buffer([](io_buffer *iob) {
    iob->bind(BIND_SIGNATURE_INT32, &data.created_sequence);
});