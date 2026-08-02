#include "widget_figure_follow.h"

#include "city/city_warnings.h"
#include "city/city_figures.h"
#include "core/log.h"
#include "core/profiler.h"
#include "core/system_time.h"
#include "dev/debug.h"
#include "figure/figure.h"
#include "game/game.h"
#include "graphics/elements/lang_text.h"
#include "graphics/elements/panel.h"
#include "graphics/elements/ui.h"
#include "graphics/graphics.h"
#include "graphics/screen.h"
#include "graphics/view/view.h"
#include "graphics/window.h"
#include "js/js_game.h"
#include "widget/widget_city.h"
#include "widget/widget_sidebar.h"

#include <algorithm>

declare_console_var_int(figure_follow_refresh_ms, 200)
declare_console_var_int(figure_follow_size, 160)
declare_console_var_int(figure_follow_offset_x, 0)
declare_console_var_int(figure_follow_offset_y, 0)

namespace {

constexpr int k_panel_pad = 8;
constexpr int k_panel_header = 24;
constexpr int k_panel_footer = 28;
constexpr int k_slow_capture_ms = 8;
constexpr uint32_t k_slow_log_cooldown_ms = 5000;

struct figure_follow_t {
    bool enabled = false;
    int figure_id = 0;
    e_figure_type type = FIGURE_NONE;
    int texture_id = 0;
    time_millis last_capture_ms = 0;
    time_millis last_slow_log_ms = 0;
    vec2i panel_pos = {-1, -1};
    bool panel_pos_set = false;
    int last_sidebar_x = -1;
};

figure_follow_t g_follow;

bool is_city_top_window() {
    return g_window_manager.window_is("window_city")
        || g_window_manager.window_is("window_city_military")
        || g_window_manager.window_is("window_city_warship");
}

vec2i panel_size() {
    const int s = std::max(48, figure_follow_size());
    return {s + 2 * k_panel_pad, s + k_panel_header + k_panel_footer};
}

void ensure_panel_pos() {
    const vec2i sz = panel_size();
    const int sidebar_x = widget_sidebar_city_offset_x();
    if (g_follow.panel_pos_set && g_follow.last_sidebar_x == sidebar_x) {
        return;
    }
    g_follow.panel_pos = {
        sidebar_x - sz.x - 8,
        screen_height() - sz.y - 8
    };
    if (g_follow.panel_pos.x < 8) {
        g_follow.panel_pos.x = 8;
    }
    if (g_follow.panel_pos.y < 40) {
        g_follow.panel_pos.y = 40;
    }
    g_follow.panel_pos_set = true;
    g_follow.last_sidebar_x = sidebar_x;
}

rect panel_rect() {
    ensure_panel_pos();
    const vec2i sz = panel_size();
    return {g_follow.panel_pos, g_follow.panel_pos + sz};
}

rect stop_button_rect() {
    ensure_panel_pos();
    const vec2i sz = panel_size();
    const vec2i pos = g_follow.panel_pos + vec2i{k_panel_pad, sz.y - k_panel_footer + 2};
    return {pos, pos + vec2i{sz.x - 2 * k_panel_pad, 22}};
}

bool target_still_valid() {
    if (!g_follow.enabled || g_follow.figure_id <= 0) {
        return false;
    }
    figure *f = figure_get(g_follow.figure_id);
    if (!f || !f->is_alive() || f->type != g_follow.type) {
        return false;
    }
    return true;
}

int capture_figure_view(int figure_id, int size, int texture_id) {
    figure *f = figure_get(figure_id);
    if (!f || !f->is_alive()) {
        return texture_id;
    }

    const vec2i saved_cam = g_camera.camera_position;
    g_camera.go_to_mappoint(f->tile);

    painter ctx = game.painter();
    g_screen_city.draw_for_figure(ctx, figure_id);

    const vec2i coord = f->main_cached_pos;
    // Outside viewport after snap (map edge / stale sort skip): keep previous texture.
    if (coord.x <= 0 && coord.y <= 0) {
        g_camera.go_to_pixel(saved_cam, false);
        return texture_id;
    }

    const vec2i crop_pos = coord - vec2i{size / 2, size / 2}
        + vec2i{figure_follow_offset_x(), figure_follow_offset_y()};
    texture_id = graphics_save_to_texture(texture_id, crop_pos, {size, size});

    g_camera.go_to_pixel(saved_cam, false);
    return texture_id;
}

void clear_state() {
    if (g_follow.texture_id) {
        graphics_delete_saved_texture(g_follow.texture_id);
        g_follow.texture_id = 0;
    }
    g_follow.enabled = false;
    g_follow.figure_id = 0;
    g_follow.type = FIGURE_NONE;
    g_follow.last_capture_ms = 0;
    g_follow.panel_pos_set = false;
    g_follow.last_sidebar_x = -1;
}

} // namespace

bool figure_follow_enabled() {
    return g_follow.enabled;
}

int figure_follow_figure_id() {
    return g_follow.figure_id;
}

int figure_follow_texture_id() {
    return g_follow.texture_id;
}

void figure_follow_stop(bool toast_lost) {
    if (!g_follow.enabled && !g_follow.texture_id) {
        return;
    }
    clear_state();
    if (toast_lost) {
        events::emit(event_city_warning{"#warning_follow_walker_lost"});
    }
}

bool figure_follow_start(int figure_id) {
    figure *f = figure_get(figure_id);
    if (!f || !f->is_alive()) {
        return false;
    }

    if (g_follow.enabled && g_follow.figure_id == figure_id) {
        figure_follow_stop(false);
        return false;
    }

    figure_follow_stop(false);

    g_follow.enabled = true;
    g_follow.figure_id = figure_id;
    g_follow.type = f->type;
    g_follow.panel_pos_set = false;
    g_follow.last_sidebar_x = -1;
    g_follow.last_capture_ms = 0;
    // Capture only from the city draw path — never during input/onclick
    // (would paint mid-frame before the normal city blit).
    return true;
}

void figure_follow_capture_if_due() {
    if (!g_follow.enabled) {
        return;
    }
    if (!target_still_valid()) {
        figure_follow_stop(true);
        return;
    }

    const time_millis now = time_get_millis();
    const int refresh = std::max(50, figure_follow_refresh_ms());
    if (g_follow.last_capture_ms && (now - g_follow.last_capture_ms) < (time_millis)refresh) {
        return;
    }

    OZZY_PROFILER_FUNCTION();
    const int size = std::max(48, figure_follow_size());
    const time_millis t0 = now;
    g_follow.texture_id = capture_figure_view(g_follow.figure_id, size, g_follow.texture_id);
    g_follow.last_capture_ms = time_get_millis();
    const time_millis dt = g_follow.last_capture_ms - t0;
    if (dt > (time_millis)k_slow_capture_ms
        && (g_follow.last_capture_ms - g_follow.last_slow_log_ms) > k_slow_log_cooldown_ms) {
        logs::info("figure_follow: capture %d ms", (int)dt);
        g_follow.last_slow_log_ms = g_follow.last_capture_ms;
    }
}

void figure_follow_draw_panel() {
    if (!g_follow.enabled || !is_city_top_window()) {
        return;
    }
    if (!target_still_valid()) {
        figure_follow_stop(true);
        return;
    }
    if (!g_follow.texture_id) {
        return;
    }

    ensure_panel_pos();
    const vec2i sz = panel_size();
    const int img = std::max(48, figure_follow_size());

    outer_panel_draw_exact(g_follow.panel_pos, sz);
    graphics_draw_from_texture(g_follow.texture_id,
                               g_follow.panel_pos + vec2i{k_panel_pad, k_panel_header},
                               {img, img});

    figure *f = figure_get(g_follow.figure_id);
    if (f) {
        ui::label(f->name.c_str(), g_follow.panel_pos + vec2i{k_panel_pad, 6}, FONT_NORMAL_BLACK_ON_LIGHT);
    }

    const rect stop = stop_button_rect();
    small_panel_draw(stop.mn, std::max(1, stop.w() / 16), 1);
    ui::label(lang_text_from_key("#stop_following"), stop.mn + vec2i{4, 4}, FONT_NORMAL_BLACK_ON_DARK);
}

bool figure_follow_handle_mouse(const mouse *m) {
    if (!g_follow.enabled || !is_city_top_window() || !m) {
        return false;
    }

    const vec2i mp{m->x, m->y};
    if (!panel_rect().inside(mp)) {
        return false;
    }

    if (m->left.went_up && stop_button_rect().inside(mp)) {
        figure_follow_stop(false);
    }

    // Any pointer activity over the panel must not fall through to the city.
    return true;
}

bool figure_follow_handle_escape(const hotkeys *h) {
    if (!h || !h->escape_pressed || !g_follow.enabled) {
        return false;
    }
    if (!is_city_top_window()) {
        return false;
    }
    figure_follow_stop(false);
    return true;
}

bool __figure_follow_start(int figure_id) { return figure_follow_start(figure_id); }
ANK_FUNCTION_1(__figure_follow_start)

void __figure_follow_stop() { figure_follow_stop(false); }
ANK_FUNCTION(__figure_follow_stop)

bool __figure_follow_enabled() { return figure_follow_enabled(); }
ANK_FUNCTION(__figure_follow_enabled)

int __figure_follow_texture_id() { return figure_follow_texture_id(); }
ANK_FUNCTION(__figure_follow_texture_id)

int __figure_follow_figure_id() { return figure_follow_figure_id(); }
ANK_FUNCTION(__figure_follow_figure_id)

void __figure_follow_capture_if_due() { figure_follow_capture_if_due(); }
ANK_FUNCTION(__figure_follow_capture_if_due)
