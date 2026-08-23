#include "figure_info_thumbnails.h"

#include "city/city_figures.h"
#include "dev/debug.h"
#include "figure/figure.h"
#include "game/game.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "js/js_game.h"
#include "widget/widget_city.h"
#include "window/window_city.h"

declare_console_var_int(figure_small_image_x, -32)
declare_console_var_int(figure_small_image_y, -48)

figure_info_thumbnails g_figure_info_thumbnails;

figure_info_thumbnails::figure_small_image::figure_small_image(figure_small_image &o) {
    image_id = o.image_id;
    o.image_id = 0;
}

figure_info_thumbnails::figure_small_image::figure_small_image(figure_id figure_id) {
    painter ctx = game.painter();

    g_screen_city.draw_for_figure(ctx, figure_id);
    const vec2i coord = figure_get(figure_id)->main_cached_pos;

    image_id = graphics_save_to_texture(-1, coord + vec2i{ figure_small_image_x(), figure_small_image_y()}, {48, 48});
}

figure_info_thumbnails::figure_small_image::~figure_small_image() {
    if (image_id) {
        graphics_delete_saved_texture(image_id);
    }
}

void figure_info_thumbnails::clear() {
    figure_images_.clear();
}

void figure_info_thumbnails::prepare_thumbnail(int index, figure_id id) {
    if (index < 0 || index >= 7) {
        return;
    }

    assert((int)figure_images_.size() == index);
    figure_images_.emplace_back(id);
}

void figure_info_thumbnails::finish() {
    painter ctx = game.painter();
    g_screen_city.draw(ctx);
}

int figure_info_thumbnails::texture(int index) const {
    if (index < 0 || index >= (int)figure_images_.size()) {
        return 0;
    }
    return figure_images_[index].image_id;
}

int __figure_info_tab_texture(int index) {
    return g_figure_info_thumbnails.texture(index);
}
ANK_FUNCTION_1(__figure_info_tab_texture)
