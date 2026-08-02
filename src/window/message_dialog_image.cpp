#include "message_dialog_image.h"

#include "city/city_message.h"
#include "graphics/image.h"
#include "io/gamefiles/lang.h"
#include "game/game.h"

void ui::message_dialog_image::init_data(xstring text_id, int message_id, void (*background_callback)(void)) {
    message_dialog_base::init_data(text_id, message_id, background_callback);

    background_img = 0;
    if (message_id != -1) {
        const city_message& city_msg = city_message_get(message_id);
        if (city_msg.background_img) {
            background_img = city_msg.background_img;
        }
    }
}

void ui::message_dialog_image::draw_background_content() {
    painter ctx = game.painter();
    const lang_message& msg = lang_get_message(text_id);
    pos = { 32, 28 };

    const image_t *img = nullptr;
    if (background_img) {
        int image_id = background_img;
        if (image_id == messages::IMAGE_FROM_SCHEME) {
            int16_t img_pack = msg.image.pack > 0 ? msg.image.pack : PACK_UNLOADED;
            img = image_get({ img_pack, msg.image.id, msg.image.offset });
        } else {
            img = image_get(image_id);
        }
    }

    if (img) {
        int current_x = (500 - img->width) / 2;
        ctx.img_generic(img, vec2i{ current_x, 96 });
    }

    draw_content(msg);
}
