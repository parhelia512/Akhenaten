#include <widget/city/building_ghost.h>
#include <widget/overlays/city_overlay.h>
#include "city.h"

#include "building/construction/build_planner.h"
#include "building/properties.h"
#include "city/finance.h"
#include "city/view.h"
#include "city/warning.h"
#include "core/calc.h"
#include "core/config.h"
#include "core/direction.h"
#include "core/string.h"
#include "figure/formation_legion.h"
#include "game/cheats.h"
#include "game/settings.h"
#include "game/state.h"
#include "graphics/graphics.h"
#include "graphics/menu.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "input/scroll.h"
#include "input/zoom.h"
#include "input/touch.h"
#include "map/building.h"
#include "map/grid.h"
#include "scenario/property.h"
#include "sound/city.h"
#include "sound/speech.h"
#include "sound/effect.h"
#include "widget/city/tile_draw.h"
#include "widget/minimap.h"
#include "window/building_info.h"
#include "window/city.h"

static struct {
    map_tile current_tile;
    map_tile selected_tile;
    int new_start_grid_offset;
    int capture_input;
} data;

static void set_city_scaled_clip_rectangle(void) {
    int x, y, width, height;
    city_view_get_scaled_viewport(&x, &y, &width, &height);
    graphics_set_clip_rectangle(x, y, width, height);
}
static void set_city_unscaled_clip_rectangle(void) {
    int x, y, width, height;
    city_view_get_unscaled_viewport(&x, &y, &width, &height);
    graphics_set_clip_rectangle(x, y, width, height);
}

static void update_zoom_level(void) {
    int zoom = city_view_get_scale();
    pixel_coordinate offset;
    city_view_get_camera_position(&offset.x, &offset.y);
    if (zoom_update_value(&zoom, &offset)) {
        city_view_set_scale(zoom);
        city_view_go_to_position(offset.x, offset.y, true);
        sound_city_decay_views();
    }
}
static void scroll_map(const mouse *m) {
    pixel_coordinate delta;
    if (scroll_get_delta(m, &delta, SCROLL_TYPE_CITY)) {
        city_view_scroll(delta.x, delta.y);
        sound_city_decay_views();
    }
}
static void update_city_view_coords(int x, int y, map_tile *tile) {
    view_tile view;
    if (city_view_pixels_to_view_tile(x, y, &view)) {
        tile->grid_offset = city_view_tile_to_grid_offset(&view);
        city_view_set_selected_view_tile(&view);
        tile->x = map_grid_offset_to_x(tile->grid_offset);
        tile->y = map_grid_offset_to_y(tile->grid_offset);
    } else
        tile->grid_offset = tile->x = tile->y = 0;
}
static int input_coords_in_city(int x, int y) {
    int x_offset, y_offset, width, height;
    city_view_get_unscaled_viewport(&x_offset, &y_offset, &width, &height);

    x -= x_offset;
    y -= y_offset;

    return (x >= 0 && x < width && y >= 0 && y < height);
}
static int adjust_offset_for_orientation(int grid_offset, int size) {
    switch (city_view_orientation()) {
        case DIR_0_TOP_RIGHT:
            return map_grid_add_delta(grid_offset, -size + 1, -size + 1);
        case DIR_2_BOTTOM_RIGHT:
            return map_grid_add_delta(grid_offset, 0, -size + 1);
        case DIR_6_TOP_LEFT:
            return map_grid_add_delta(grid_offset, -size + 1, 0);
        default:
            return grid_offset;
    }
}

void widget_city_draw_without_overlay(int selected_figure_id, pixel_coordinate *figure_coord, const map_tile *tile) {
    int highlighted_formation = 0;
    if (config_get(CONFIG_UI_HIGHLIGHT_LEGIONS)) {
        highlighted_formation = formation_legion_at_grid_offset(tile->grid_offset);
        if (highlighted_formation > 0 && formation_get(highlighted_formation)->in_distant_battle)
            highlighted_formation = 0;
    }
    init_draw_context(selected_figure_id, figure_coord, highlighted_formation);

//    city_view_foreach_map_tile(draw_outside_map);
//    int x;
//    int y;
//    city_view_get_camera_scrollable_viewspace_clip(&x, &y);
//    graphics_set_clip_rectangle(x - 30, y, map_grid_width() * 30 - 60, map_grid_height() * 15 - 30);


    // do this for EVERY tile (not just valid ones)
    // to recalculate the pixel lookup offsets
    city_view_foreach_map_tile(draw_empty_tile_debug);

    if (!city_building_ghost_mark_deleting(tile)) {
        city_view_foreach_valid_map_tile(draw_footprint); // this needs to be done in a separate loop to avoid bleeding over figures
        city_view_foreach_valid_map_tile(
                draw_top,
                draw_ornaments,
                draw_figures);
        if (!selected_figure_id) {
            Planner.update(tile);
            Planner.draw();
        }
    } else {
        city_view_foreach_valid_map_tile(draw_footprint); // this needs to be done in a separate loop to avoid bleeding over figures
        city_view_foreach_valid_map_tile(
                deletion_draw_top,
                deletion_draw_figures_animations,
                draw_elevated_figures);
    }

    // finally, draw these on top of everything else
    city_view_foreach_valid_map_tile(
            draw_debug,
            draw_debug_figures);
}
void widget_city_draw_with_overlay(const map_tile *tile) {
    if (!select_city_overlay())
        return;

    // do this for EVERY tile (not just valid ones)
    // to recalculate the pixel lookup offsets
    city_view_foreach_map_tile(draw_empty_tile_debug);

    if (!city_building_ghost_mark_deleting(tile)) {
        city_view_foreach_valid_map_tile(draw_footprint_overlay); // this needs to be done in a separate loop to avoid bleeding over figures
        city_view_foreach_valid_map_tile(
                draw_top_overlay,
                draw_ornaments_overlay,
                draw_figures_overlay);
        city_view_foreach_map_tile(draw_elevated_figures);
        Planner.update(tile);
        Planner.draw();
    } else {
        city_view_foreach_valid_map_tile(draw_footprint_overlay); // this needs to be done in a separate loop to avoid bleeding over figures
        city_view_foreach_valid_map_tile(
                deletion_draw_top,
                deletion_draw_figures_animations,
                draw_elevated_figures_overlay);
    }
}

void widget_city_draw(void) {
    if (config_get(CONFIG_UI_ZOOM)) {
        update_zoom_level();
        graphics_set_active_canvas(CANVAS_CITY);
    }
    set_city_scaled_clip_rectangle();

    if (game_state_overlay())
        widget_city_draw_with_overlay(&data.current_tile);
    else
        widget_city_draw_without_overlay(0, 0, &data.current_tile);

    graphics_set_active_canvas(CANVAS_UI);
}
void widget_city_draw_for_figure(int figure_id, pixel_coordinate *coord) {
    set_city_scaled_clip_rectangle();

    widget_city_draw_without_overlay(figure_id, coord, &data.current_tile);

    graphics_reset_clip_rectangle();
}
bool widget_city_draw_construction_cost_and_size(void) {
    if (!Planner.in_progress)
        return false;

    if (scroll_in_progress())
        return false;

    int size_x, size_y;
    int cost = Planner.total_cost;
    int has_size = Planner.get_total_drag_size(&size_x, &size_y);
    if (!cost && !has_size)
        return false;

    set_city_unscaled_clip_rectangle();
    int x, y;
    city_view_get_selected_tile_pixels(&x, &y);
    int inverted_scale = calc_percentage(100, city_view_get_scale());
    x = calc_adjust_with_percentage(x, inverted_scale);
    y = calc_adjust_with_percentage(y, inverted_scale);

    if (cost) {
        color_t color;
        if (cost <= city_finance_treasury()) // Color blind friendly
            color = scenario_property_climate() == CLIMATE_DESERT ? COLOR_FONT_ORANGE : COLOR_FONT_ORANGE_LIGHT;
        else
            color = COLOR_FONT_RED;
        text_draw_number_colored(cost, '@', " ", x + 58 + 1, y + 1, FONT_NORMAL_PLAIN, COLOR_BLACK);
        text_draw_number_colored(cost, '@', " ", x + 58, y, FONT_NORMAL_PLAIN, color);
    }
    if (has_size) {
        int width = -text_get_width(string_from_ascii("  "), FONT_SMALL_PLAIN);
        width += text_draw_number_colored(size_x, '@', "x", x - 15 + 1, y + 25 + 1, FONT_SMALL_PLAIN, COLOR_BLACK);
        text_draw_number_colored(size_x, '@', "x", x - 15, y + 25, FONT_SMALL_PLAIN, COLOR_FONT_YELLOW);
        text_draw_number_colored(size_y, '@', " ", x - 15 + width + 1, y + 25 + 1, FONT_SMALL_PLAIN, COLOR_BLACK);
        text_draw_number_colored(size_y, '@', " ", x - 15 + width, y + 25, FONT_SMALL_PLAIN, COLOR_FONT_YELLOW);
    }
    graphics_reset_clip_rectangle();
    return true;
}

// INPUT HANDLING

static void build_start(const map_tile *tile) {
    if (tile->grid_offset) // Allow building on paused
        Planner.construction_start(tile->x, tile->y, tile->grid_offset);
}
static void build_move(const map_tile *tile) {
    if (!Planner.in_progress)
        return;
    Planner.construction_update(tile->x, tile->y, tile->grid_offset);
}
static void build_end(void) {
    if (Planner.in_progress) {
        if (Planner.build_type != BUILDING_NONE)
            sound_effect_play(SOUND_EFFECT_BUILD);

        Planner.construction_finalize();
    }
}

static int has_confirmed_construction(int ghost_offset, int tile_offset, int range_size) {
    tile_offset = adjust_offset_for_orientation(tile_offset, range_size);
    int x = map_grid_offset_to_x(tile_offset);
    int y = map_grid_offset_to_y(tile_offset);
    if (ghost_offset <= 0 || !map_grid_is_inside(x, y, range_size))
        return 0;

    for (int dy = 0; dy < range_size; dy++) {
        for (int dx = 0; dx < range_size; dx++) {
            if (ghost_offset == tile_offset + map_grid_delta(dx, dy))
                return 1;

        }
    }
    return 0;
}

static bool handle_right_click_allow_building_info(const map_tile *tile) {
    int allow = true;
    if (!window_is(WINDOW_CITY))
        allow = false;

    window_city_show();

    if (!tile->grid_offset)
        allow = false;

    if (allow && city_has_warnings()) {
        city_warning_clear_all();
        allow = false;
    }
    return allow;
}
static bool handle_legion_click(const map_tile *tile) {
    if (tile->grid_offset) {
        int formation_id = formation_legion_at_grid_offset(tile->grid_offset);
        if (formation_id > 0 && !formation_get(formation_id)->in_distant_battle) {
            window_city_military_show(formation_id);
            return true;
        }
    }
    return false;
}
static bool handle_cancel_construction_button(const touch *t) {
    if (!Planner.build_type)
        return false;

    int x, y, width, height;
    city_view_get_unscaled_viewport(&x, &y, &width, &height);
    int box_size = 5 * 16;
    width -= box_size;

    if (t->current_point.x < width || t->current_point.x >= width + box_size ||
        t->current_point.y < 24 || t->current_point.y >= 40 + box_size) {
        return false;
    }
    Planner.construction_cancel();
    return true;
}
void widget_city_clear_current_tile(void) {
    data.selected_tile.x = -1;
    data.selected_tile.y = -1;
    data.selected_tile.grid_offset = 0;
    data.current_tile.grid_offset = 0;
}

static void handle_touch_scroll(const touch *t) {
    if (Planner.build_type) {
        if (t->has_started) {
            int x_offset, y_offset, width, height;
            city_view_get_unscaled_viewport(&x_offset, &y_offset, &width, &height);
            scroll_set_custom_margins(x_offset, y_offset, width, height);
        }
        if (t->has_ended)
            scroll_restore_margins();

        return;
    }
    scroll_restore_margins();

    if (!data.capture_input)
        return;
    int was_click = touch_was_click(get_latest_touch());
    if (t->has_started || was_click) {
        scroll_drag_start(1);
        return;
    }
    if (!touch_not_click(t))
        return;
    if (t->has_ended)
        scroll_drag_end();

}
static void handle_touch_zoom(const touch *first, const touch *last) {
    if (touch_not_click(first))
        zoom_update_touch(first, last, city_view_get_scale());

    if (first->has_ended || last->has_ended)
        zoom_end_touch();

}
static void handle_first_touch(map_tile *tile) {
    const touch *first = get_earliest_touch();
    int type = Planner.build_type;

    if (touch_was_click(first)) {
        if (handle_cancel_construction_button(first) || handle_legion_click(tile))
            return;
        if (type == BUILDING_NONE && handle_right_click_allow_building_info(tile)) {
            scroll_drag_end();
            data.capture_input = 0;
            window_building_info_show(tile->grid_offset);
            return;
        }
    }

    handle_touch_scroll(first);

    if (!input_coords_in_city(first->current_point.x, first->current_point.y) || type == BUILDING_NONE)
        return;

    if (Planner.has_flag_set(PlannerFlags::Draggable)) {
        if (!Planner.in_progress) {
            if (first->has_started) {
                build_start(tile);
                data.new_start_grid_offset = 0;
            }
        } else {
            if (first->has_started) {
                if (data.selected_tile.grid_offset != tile->grid_offset)
                    data.new_start_grid_offset = tile->grid_offset;

            }
            if (touch_not_click(first) && data.new_start_grid_offset) {
                data.new_start_grid_offset = 0;
                data.selected_tile.grid_offset = 0;
                Planner.construction_cancel();
                build_start(tile);
            }
            build_move(tile);
            if (data.selected_tile.grid_offset != tile->grid_offset)
                data.selected_tile.grid_offset = 0;

            if (first->has_ended) {
                if (data.selected_tile.grid_offset == tile->grid_offset) {
                    build_end();
                    widget_city_clear_current_tile();
                    data.new_start_grid_offset = 0;
                } else {
                    data.selected_tile.grid_offset = tile->grid_offset;
                }
            }
        }
        return;
    }

    int size = building_properties_for_type(type)->size;
    if (type == BUILDING_WAREHOUSE)
        size = 3;


    if (touch_was_click(first) && first->has_ended && data.capture_input &&
        has_confirmed_construction(data.selected_tile.grid_offset, tile->grid_offset, size)) {
        build_start(&data.selected_tile);
        build_move(&data.selected_tile);
        build_end();
        widget_city_clear_current_tile();
    } else if (first->has_ended)
        data.selected_tile = *tile;

}
static void handle_last_touch(void) {
    const touch *last = get_latest_touch();
    if (!last->in_use)
        return;
    if (touch_was_click(last)) {
        Planner.construction_cancel();
        return;
    }
    if (touch_not_click(last))
        handle_touch_zoom(get_earliest_touch(), last);

}
static void handle_touch(void) {
    const touch *first = get_earliest_touch();
    if (!first->in_use) {
        scroll_restore_margins();
        return;
    }

    map_tile *tile = &data.current_tile;
    if (!Planner.in_progress || input_coords_in_city(first->current_point.x, first->current_point.y))
        update_city_view_coords(first->current_point.x, first->current_point.y, tile);


    if (first->has_started && input_coords_in_city(first->current_point.x, first->current_point.y)) {
        data.capture_input = 1;
        scroll_restore_margins();
    }

    handle_last_touch();
    handle_first_touch(tile);

    if (first->has_ended)
        data.capture_input = 0;

    Planner.draw_as_constructing = false;
}
int widget_city_has_input(void) {
    return data.capture_input;
}
static void handle_mouse(const mouse *m) {
    map_tile *tile = &data.current_tile;
    update_city_view_coords(m->x, m->y, tile);
    zoom_map(m);
    Planner.draw_as_constructing = false;
    if (m->left.went_down) {
        if (handle_legion_click(tile))
            return;
        if (!Planner.in_progress)
            build_start(tile);

        build_move(tile);
    } else if (m->left.is_down || Planner.in_progress)
        build_move(tile);

    if (m->left.went_up)
        build_end();

    if (m->middle.went_down && input_coords_in_city(m->x, m->y) && !Planner.build_type)
        scroll_drag_start(0);

    if (m->right.went_up) {
        if (!Planner.build_type) {
            if (handle_right_click_allow_building_info(tile))
                window_building_info_show(tile->grid_offset);
        } else
            Planner.construction_cancel();
    }

    if (m->middle.went_up)
        scroll_drag_end();
}
static void military_map_click(int legion_formation_id, const map_tile *tile) {
    if (!tile->grid_offset) {
        window_city_show();
        return;
    }
    formation *m = formation_get(legion_formation_id);
    if (m->in_distant_battle || m->cursed_by_mars)
        return;
    int other_formation_id = formation_legion_at_building(tile->grid_offset);
    if (other_formation_id && other_formation_id == legion_formation_id)
        formation_legion_return_home(m);
    else {
        formation_legion_move_to(m, tile->x, tile->y);
        sound_speech_play_file("wavs/cohort5.wav");
    }
    window_city_show();
}
void widget_city_handle_input(const mouse *m, const hotkeys *h) {
    scroll_map(m);

    if (m->is_touch)
        handle_touch();
    else
        handle_mouse(m);

    if (h->escape_pressed) {
        if (Planner.build_type)
            Planner.construction_cancel();
        else
            hotkey_handle_escape();
    }
}
void widget_city_handle_input_military(const mouse *m, const hotkeys *h, int legion_formation_id) {
    map_tile *tile = &data.current_tile;
    update_city_view_coords(m->x, m->y, tile);
    if (!city_view_is_sidebar_collapsed() && widget_minimap_handle_mouse(m))
        return;
    if (m->is_touch) {
        const touch *t = get_earliest_touch();
        if (!t->in_use)
            return;
        if (t->has_started)
            data.capture_input = 1;

        handle_touch_scroll(t);
        if (t->has_ended)
            data.capture_input = 0;

    }
    scroll_map(m);
    if (m->right.went_up || h->escape_pressed) {
        data.capture_input = 0;
        city_warning_clear_all();
        window_city_show();
    } else {
        update_city_view_coords(m->x, m->y, tile);
        if ((!m->is_touch && m->left.went_down) ||
            (m->is_touch && m->left.went_up && touch_was_click(get_earliest_touch())))
            military_map_click(legion_formation_id, tile);

    }
}

void widget_city_get_tooltip(tooltip_context *c) {
    if (setting_tooltips() == TOOLTIPS_NONE)
        return;
    if (!window_is(WINDOW_CITY))
        return;
    if (data.current_tile.grid_offset == 0)
        return;
    int grid_offset = data.current_tile.grid_offset;
    int building_id = map_building_at(grid_offset);
    int overlay = game_state_overlay();
    // cheat tooltips
    if (overlay == OVERLAY_NONE && game_cheat_tooltip_enabled()) {
        c->type = TOOLTIP_TILES;
        c->high_priority = 1;
        return;
    }
    // regular tooltips
    if (overlay == OVERLAY_NONE && building_id && building_get(building_id)->type == BUILDING_SENATE_UPGRADED) {
        c->type = TOOLTIP_SENATE;
        c->high_priority = 1;
        return;
    }
    // overlay tooltips
    if (overlay != OVERLAY_NONE) {
        c->text_group = 66;
        c->text_id = widget_city_overlay_get_tooltip_text(c, grid_offset);
        if (c->text_id) {
            c->type = TOOLTIP_OVERLAY;
            c->high_priority = 1;
        }
    }
}
