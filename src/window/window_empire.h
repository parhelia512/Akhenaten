#pragma once

#include "window/autoconfig_window.h"

struct empire_window : public autoconfig_window_t<empire_window> {
    vec2i min_pos, max_pos;
    vec2i draw_offset;
    vec2i last_mouse_pos;
    float scroll_remainder_x = 0.f;
    float scroll_remainder_y = 0.f;
    int is_scrolling;
    int finished_scroll;
    bool left_panning = false;
    int left_pan_travel = 0;
    vec2i left_pan_last_pos;
    vec2i start_pos, finish_pos;
    image_desc image;

    virtual int handle_mouse(const mouse *m) override { return 0; }
    virtual int get_tooltip_text() override { return 0; }
    virtual void draw_foreground(UiFlags flags) override {}
    virtual void ui_draw_foreground(UiFlags flags) override;
    virtual int ui_handle_mouse(const mouse *m) override;
    virtual void init() override;

    virtual void archive_load(archive arch) override;

    void draw_map();
    void draw_paneling();
    bool is_outside_map(int x, int y);
    vec2i map_clip_origin() const;
    vec2i map_area_size_pixels() const;
    vec2i map_viewport_size() const;
    vec2i map_base_origin() const;
    vec2i map_draw_origin() const;
    float map_scale() const;
    vec2i map_to_screen(vec2i map_pos) const;
};


void window_empire_show();
