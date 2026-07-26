#include "building_zoo.h"

#include "city/city.h"
#include "city/city_warnings.h"
#include "empire/empire.h"
#include "figuretype/figure_entertainer.h"
#include "game/game.h"
#include "game/game_events.h"
#include "game/resource.h"
#include "graphics/graphics.h"
#include "grid/building_tiles.h"
#include "js/js_game.h"
#include "window/popup_dialog.h"

#include <algorithm>
#include <cmath>

REPLICATE_STATIC_PARAMS_FROM_CONFIG(building_zoo);

int building_zoo::resource_consume_amount() {
    switch (game.difficulty()) {
    case DIFFICULTY_VERY_HARD:
        return 60;
    case DIFFICULTY_HARD:
        return 40;
    default:
        return 20;
    }
}

bool building_zoo::add_resource(e_resource resource, int amount) {
    if (resource != RESOURCE_STRAW && resource != RESOURCE_GAMEMEAT) {
        return false;
    }

    verify_no_crash(id() > 0);
    store_resource(resource, amount);
    return true;
}

void building_zoo::spawn_figure() {
    const int need = resource_consume_amount();
    if (stored_amount(RESOURCE_STRAW) < need || stored_amount(RESOURCE_GAMEMEAT) < need) {
        return;
    }

    if (!common_spawn_figure_trigger(100, BUILDING_SLOT_SERVICE)) {
        return;
    }

    consume_resource(RESOURCE_STRAW, need);
    consume_resource(RESOURCE_GAMEMEAT, need);
    // Animals are present once feed is consumed; ACTION_94 roamers never hit update_shows().
    runtime_data().juggler_visited = 32;
    create_roaming_figure(FIGURE_ZOOKEEPER, (e_figure_action)ACTION_94_ENTERTAINER_ROAMING, BUILDING_SLOT_SERVICE);
}

void building_zoo::update_day() {
    building_impl::update_day();

    auto &d = runtime_data();
    d.num_shows = 0;
    if (d.juggler_visited > 0) {
        --d.juggler_visited;
        ++d.num_shows;
    }
}

void building_zoo::update_graphic() {
    // Work frames are animals/water — only play while cages are occupied.
    const bool animals_present = runtime_data().juggler_visited > 0;
    const xstring &animkey = (base.play_animation && animals_present) ? animkeys().work : animkeys().none;
    set_animation(animkey);
    building_impl::update_graphic();
}

bool building_zoo::draw_ornaments_and_animations_height(painter &ctx, vec2i point, tile2i tile, color color_mask) {
    building_impl::draw_ornaments_and_animations_height(ctx, point, tile, color_mask);

    auto draw_stacks = [&](e_resource resource, const xstring &anim_key) {
        int amount = (int)ceil((float)base.stored_amount(resource) / 100.0f) - 1;
        if (amount < 0) {
            return;
        }

        const auto &ranim = anim(anim_key);
        if (!ranim.first_img()) {
            return;
        }

        vec2i pos = ranim.pos;
        const int stacks = std::min(amount, 8);
        for (int i = 0; i < stacks; ++i) {
            auto &command = ImageDraw::create_subcommand(ctx, render_command_t::ert_generic);
            command.image_id = ranim.first_img();
            command.pixel = point + pos;
            command.mask = color_mask;
            pos += {5, -5};
        }
    };

    draw_stacks(RESOURCE_GAMEMEAT, animkeys().gamemeat);
    draw_stacks(RESOURCE_STRAW, animkeys().straw);
    return true;
}

void building_zoo::on_place_checks() {
    building_impl::on_place_checks();

    const bool straw_ok = g_city.can_produce_resource(RESOURCE_STRAW)
        || g_empire.can_import_resource(RESOURCE_STRAW, false);
    const bool meat_ok = g_city.can_produce_resource(RESOURCE_GAMEMEAT)
        || g_empire.can_import_resource(RESOURCE_GAMEMEAT, false);

    if (straw_ok && meat_ok) {
        return;
    }

    // Original Cleopatra: zoo cannot work without access to both inputs — remove it.
    events::emit(event_construction_warning{"#building_removed_zoo"});
    popup_dialog::show(textid{5, 139}, textid{5, 140}, e_popup_btns_ok, [](bool) {});

    map_building_tiles_remove(id(), tile());
    base.clear_related_data();
    base.state = BUILDING_STATE_DELETED_BY_GAME;
}
