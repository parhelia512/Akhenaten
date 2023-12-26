#include "building_bricklayers_guild.h"

#include "building/building.h"
#include "city/object_info.h"
#include "city/resource.h"
#include "core/calc.h"
#include "game/resource.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/lang_text.h"
#include "graphics/view/view.h"
#include "graphics/boilerplate.h"
#include "graphics/text.h"
#include "io/gamefiles/lang.h"
#include "config/config.h"
#include "window/building/common.h"
#include "window/building/figures.h"
#include "sound/sound_building.h"
#include "game/game.h"

static void building_guild_draw_info(object_info& c, const char* type, e_resource resource, e_resource input_resource) {
    auto &meta = building::get_info(type);
    window_building_play_sound(&c, snd::get_building_info_sound(type));

    painter ctx = game.painter();

    outer_panel_draw(c.offset, c.width_blocks, c.height_blocks);
    ImageDraw::img_generic(ctx, image_id_resource_icon(resource), c.offset.x + 10, c.offset.y + 10);
    lang_text_draw_centered(meta.text_id, 0, c.offset.x, c.offset.y + 10, 16 * c.width_blocks, FONT_LARGE_BLACK_ON_LIGHT);

    building* b = building_get(c.building_id);
    int pct_done = calc_percentage<int>(b->data.industry.progress, 400);
    int width = lang_text_draw(meta.text_id, 2, c.offset.x + 32, c.offset.y + 40, FONT_NORMAL_BLACK_ON_LIGHT);
    width += text_draw_percentage(pct_done, c.offset.x + 32 + width, c.offset.y + 40, FONT_NORMAL_BLACK_ON_LIGHT);
    lang_text_draw(meta.text_id, 3, c.offset.x + 32 + width, c.offset.y + 40, FONT_NORMAL_BLACK_ON_LIGHT);

    ImageDraw::img_generic(ctx, image_id_resource_icon(input_resource), c.offset.x + 32, c.offset.y + 56);
    width = lang_text_draw(meta.text_id, 12, c.offset.x + 60, c.offset.y + 60, FONT_NORMAL_BLACK_ON_LIGHT);
    if (b->stored_amount() < 100) {
        lang_text_draw_amount(8, 10, 0, c.offset.x + 60 + width, c.offset.y + 60, FONT_NORMAL_BLACK_ON_LIGHT);
    } else {
        lang_text_draw_amount(8, 10, b->stored_amount(), c.offset.x + 60 + width, c.offset.y + 60, FONT_NORMAL_BLACK_ON_LIGHT);
    }

    if (!c.has_road_access)
        window_building_draw_description_at(c, 86, 69, 25);
    else if (city_resource_is_mothballed(resource))
        window_building_draw_description_at(c, 86, meta.text_id, 4);
    else if (b->num_workers <= 0)
        window_building_draw_description_at(c, 86, meta.text_id, 5);
    else if (!b->guild_has_resources())
        window_building_draw_description_at(c, 86, meta.text_id, 11);
    else if (c.worker_percentage >= 100)
        window_building_draw_description_at(c, 86, meta.text_id, 6);
    else if (c.worker_percentage >= 75)
        window_building_draw_description_at(c, 86, meta.text_id, 7);
    else if (c.worker_percentage >= 50)
        window_building_draw_description_at(c, 86, meta.text_id, 8);
    else if (c.worker_percentage >= 25)
        window_building_draw_description_at(c, 86, meta.text_id, 9);
    else
        window_building_draw_description_at(c, 86, meta.text_id, 10);

    inner_panel_draw(c.offset.x + 16, c.offset.y + 136, c.width_blocks - 2, 4);
    window_building_draw_employment(&c, 142);
}

void building_bricklayers_guild_draw_info(object_info& c) {
    building_guild_draw_info(c, "bricklayers_guild", RESOURCE_BRICKS, RESOURCE_NONE);
}