#include "core/app.h"

#include "core/profiler.h"
#include "dev/debug.h"
#include "game/game.h"
#include "graphics/clouds.h"
#include "graphics/view/view.h"
#include "graphics/view/zoom.h"
#include "graphics/window.h"
#include "widget/widget_city.h"

#include <algorithm>

static void city_clouds_draw(painter &ctx) {
    OZZY_PROFILER_FUNCTION();

    if (game.paused
        || (!g_window_manager.window_is("window_city")
            && !g_window_manager.window_is("window_city_military")
            && !g_window_manager.window_is("window_city_warship")
            && !g_window_manager.window_is("window_city_transport"))) {
        g_clouds.pause();
    }

    auto mm_view = g_camera.get_scrollable_pixel_limits();
    const vec2i offset = {
        g_camera.camera_position.x - mm_view.min.x,
        g_camera.camera_position.y - mm_view.min.y,
    };

    const float zoom = g_zoom.get_scale();
    const vec2i view_size = {
        std::max(1, (int)(g_camera.size_pixels.x / zoom)),
        std::max(1, (int)(g_camera.size_pixels.y / zoom)),
    };

    g_clouds.draw(ctx, offset, view_size);
    draw_debug_clouds(ctx);
}

void ANK_REGISTER_APPLICATION_MODULE(register_city_clouds_module) {
    g_screen_city.add_screen_space_effect(city_clouds_draw);
}
