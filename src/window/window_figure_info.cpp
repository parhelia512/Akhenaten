#include "window_figure_info.h"

#include "figure/figure.h"
#include "graphics/window.h"
#include "grid/figure.h"
#include "graphics/graphics.h"
#include "graphics/image.h"
#include "game/game.h"
#include "dev/debug.h"
#include "window/message_dialog.h"
#include "window/building/figures.h"
#include "window/window_city.h"
#include "game/game.h"
#include "widget/widget_city.h"
#include "widget/widget_figure_follow.h"
#include "js/js_game.h"
#include "js/js_struct.h"
#include "graphics/elements/ui_js.h"

declare_console_var_int(figure_small_image_x, -32)
declare_console_var_int(figure_small_image_y, -48)

struct figure_small_image {
    int image_id = 0;

    figure_small_image(figure_small_image &o) {
        this->image_id = o.image_id;
        o.image_id = 0;
    }

    figure_small_image(int figure_id) {
        painter ctx = game.painter();

        g_screen_city.draw_for_figure(ctx, figure_id);
        const vec2i coord = figure_get(figure_id)->main_cached_pos;

        image_id = graphics_save_to_texture(-1, coord + vec2i{ figure_small_image_x(), figure_small_image_y()}, {48, 48});
    }

    ~figure_small_image() {
        if (image_id) {
            graphics_delete_saved_texture(image_id);
        }
    }
};

struct figures_data_t {
    svector<figure_small_image, 7> figure_images;
};

figures_data_t g_figures_data;

struct figure_info_window_init { int figure_id; };
ANK_REGISTER_STRUCT_WRITER(figure_info_window_init, figure_id);

void figure_info_window::prepare_figures(object_info &c) {
    if (c.nfigure.ids.size() <= 0) {
        return;
    }

    auto &data = g_figures_data;

    data.figure_images.clear();

    for (const auto &id: c.nfigure.ids) {
        data.figure_images.emplace_back(id);
    }

    painter ctx = game.painter();
    g_screen_city.draw(ctx);
}

figure_info_window::figure_info_window() {
    window_figure_register_handler(this);
}

void figure_info_window::window_info_foreground(object_info &c) {
    common_info_window::window_info_foreground(c);

    figure *f = c.figure_get();
    g_debug_figure_id = c.figure_get_id();

    bool custom_window = f->dcast()->window_info_background(c);
    if (custom_window) {
        return;
    }

    c.nfigure.drawn = 1;
}

void figure_info_window::archive_load(archive arch) {
    common_info_window::archive_load(arch);
    assert(elements.size() > 0);
    arch.r("related_figures", related_figures);
}

void figure_info_window::window_info_background(object_info &c) {
    common_info_window::window_info_background(c);

    figure *f = c.nfigure.get();

    ui.format_all(f->dcast());

    ui.check_errors = false;
    ui["show_path"] = ( !!(f->draw_mode & e_figure_draw_routing) ? "P" : "p");

    if (ui.contains("show_follow")) {
        ui["show_follow"] = (figure_follow_enabled() && figure_follow_figure_id() == f->id) ? "F" : "f";
    }

    for (int i = 0; i < c.nfigure.ids.size(); i++) {
        ui[{"button_figure", i}].select(i == c.nfigure.selected_index);
    }

    e_overlay foverlay = f->dcast()->get_overlay();
    ui["show_overlay"].enabled = (foverlay != OVERLAY_NONE);
    ui["show_overlay"] = (g_city.overlay_is(foverlay) ? "V" : "v");
}

int figure_info_window::window_info_handle_mouse(const mouse *m, object_info &c) {
    return 0;
}

void figure_info_window::init(object_info &c) {
    const int figure_id = c.nfigure.ids[c.nfigure.selected_index];

    prepare_figures(c);

    ui.check_errors = false;
    for (int i = 0; i < c.nfigure.ids.size(); i++) {
        xstring btn_id;
        btn_id.printf("button_figure%d", i);
        if (!ui.contains(btn_id)) {
            break;
        }

        ui[btn_id].select(i == c.nfigure.selected_index);
        ui[btn_id].onclick([index = i, &c] {
            c.nfigure.selected_index = index;
            c.can_play_sound = true;
            events::emit(event_update_tile_info { true });
        });

        auto screen_opt = ui[btn_id].dcast_image_button();
        if (screen_opt) {
            screen_opt->texture_id = g_figures_data.figure_images[i].image_id;
        }
    }

    ui.event(figure_info_window_init{ figure_id }, get_section(), __func__);
}

bool figure_info_window::check(object_info &c) {
    return figure_type_any_of(c.figure_get(), related_figures);
}

figure *figure_info_window::figure_get(object_info &c) {
    int figure_id = c.grid_offset ? map_figure_id_get(c.grid_offset) : 0;
    return ::figure_get(figure_id);
}