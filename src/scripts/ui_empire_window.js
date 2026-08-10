log_info("akhenaten: ui empire window common started")

function window_empire_show_checked() {
    var avail = city.is_empire_available()
    if (avail == 1 || scenario.scmode != e_scenario_normal) {
        emit event_show_window{ id: "empire_window" }
        return
    }
    var text = (avail == 0) ? "#not_available_in_this_assignment" : "#not_available_yet"
    city.warnings.show(text)
}

[es=window]
empire_window {
    trade_column_spacing : 146
    trade_row_spacing : 20
    info_y_traded : -3
    trade_button_offset_x : 0
    info_y_footer_1 : 78
    info_y_city_desc : 28
    trade_resource_size : 18
    trade_resource_offset : 3
    trade_button_offset_y : 10
    start_pos : {x: 16, y: 16}
    finish_pos : {x: 32, y: 136}
    image : {pack:PACK_EMPIRE, id:1}
    bottom_image : {pack:PACK_GENERAL, id:172, offset:3}
    horizontal_bar : {pack:PACK_GENERAL, id:172, offset:1}
    vertical_bar : {pack:PACK_GENERAL, id:172, offset:0}
    cross_bar : {pack:PACK_GENERAL, id:172, offset:2}
    closed_trade_route_hl : {pack:PACK_GENERAL, id:149, offset:211}
    open_trade_route : {pack:PACK_GENERAL, id:149, offset:201}
    open_trade_route_hl : {pack:PACK_GENERAL, id:149, offset:186}

    ui {
        background           : dummy({size[sw(0), sh(0)]})
        city_name            : header({pos[0, 0], size[100, 20], align:"center"})
        button_help          : help_button({pos[0, 0]})
        button_close         : close_button({pos[0, 0]})
        button_advisor       : advisor_button({pos[0, 0]})
        button_pause         : button({pos[0, 0], size:[32, 32], ondraw_event: "draw_pause_button"})

        button_open_trade    : button({pos[0, 0], size:[440, 20]})
        info_tooltip         : text({pos[0, 0], size:[400, 20], font:FONT_NORMAL_BLACK_ON_LIGHT, align:"center"})

        city_sell_title      : text({text[47, 11], pos[0, 0], font: FONT_NORMAL_BLACK_ON_LIGHT })
        city_sell_items      : dummy({pos[0, 0], size[200, 0], ondraw_event: "draw_city_sell_items"})
        city_sell_item       : dummy({size[120, 20], font:FONT_SMALL_PLAIN})

        city_buy_title       : text({text[47, 10], pos[0, 0], font: FONT_NORMAL_BLACK_ON_LIGHT })
        city_buy_items       : dummy({pos[0, 0], size[200, 0], ondraw_event: "draw_city_buy_items"})
        city_buy_item        : dummy({size[120, 20], font:FONT_SMALL_PLAIN})

        city_want_sell_title : text({text[47, 5], pos[0, 0], font: FONT_NORMAL_BLACK_ON_LIGHT })
        city_want_sell_items : dummy({pos[0, 0], ondraw_event: "draw_city_want_sell_items"})
        city_want_sell_item  : dummy({size[110, 0], font:FONT_SMALL_PLAIN})

        city_want_buy_title  : text({text[47, 4], pos[0, 0], font: FONT_NORMAL_BLACK_ON_LIGHT })
        city_want_buy_items  : dummy({pos[0, 0], ondraw_event: "draw_city_want_buy_items"})
        city_want_buy_item   : dummy({size[110, 0], font:FONT_SMALL_PLAIN})

        // After other UI so army travel text paints on top (same as former C++ post-draw call).
        object_info          : dummy({pos[0, 0], size[0, 0], ondraw_event: "draw_object_info"})
    }

    trade_amount_image : function(extraOffset) {
        return get_image({pack:PACK_GENERAL, id:171, offset: extraOffset })
    }

    adjust_scroll : __empire_map_adjust_scroll

    @selected_city {
        get: function() {
            var obj = this.selected_object
            return (obj && obj.type == EMPIRE_OBJECT_CITY) ? obj.city_id : 0
        }
    }
    @selected_object {
        get: function() {
            var pick = __empire_map_selected_object()
            return pick > 0 ? new EmpireObject(pick - 1) : null
        }
    }

    /** Filled each frame in draw_paneling; used by draw_object_info*/
    screen_bounds : null

    /** Selected city's trade route is deferred and drawn last (on top). */
    deferred_route_city_id : -1

    scroll_remainder_x : 0
    scroll_remainder_y : 0
    left_panning : false
    left_pan_travel : 0
    left_pan_last_pos : { x: 0, y: 0 }
    finished_scroll : 0

    route_state : {
        closed : 0
        closed_selected : 1
        open : 2
        open_selected : 3
    }
}

var SCROLL_DRAG_TOUCH = 0
var SCROLL_DRAG_MIDDLE_MOUSE_PAN = 2

function empire_window_draw_trade_resource_row(offset, flags, resource, tradeNow, tradeMax, font) {
    var ox = offset.x
    var oy = offset.y
    ui.resource_icon_flags({ x: ox + 1, y: oy + 1 }, resource, UiFlags_Outline)

    var text = "0"
    if (tradeNow < 0) {
        text = String(tradeMax)
    } else {
        text = String(tradeNow) + " " + __loc(47, 12) + " " + String(tradeMax)
    }

    var clicked = ui.button({text:text, pos[ox - 2, oy - 2], size[105, 24], font:font, flags: UiFlags_NoBody, tooltip:__loc(23, resource)})
    if (clicked == ui.button_clicked) {
        show_trade_resource_settings_window(resource)
    }
    var img = null
    switch (tradeMax) {
    case 1500:
    case 15:
        img = empire_window.trade_amount_image(0)
        if (img) {
            ui.image(img, { x: ox + 21, y: oy - 1 })
        }
        break
    case 2500:
    case 25:
        img = empire_window.trade_amount_image(1)
        if (img) {
            ui.image(img, { x: ox + 17, y: oy - 1 })
        }
        break
    case 4000:
    case 40:
        img = empire_window.trade_amount_image(2)
        if (img) {
            ui.image(img, { x: ox + 13, y: oy - 1 })
        }
        break
    }
}

function empire_window_screen_bounds() {
    return { min_pos: {x: 0, y: 0}, max_pos: {x: screen.width, y: screen.height} }
}

function empire_window_map_scale() {
    var sb = empire_window.screen_bounds
    var viewport_w = Math.max(1, (sb.max_pos.x - sb.min_pos.x) - empire_window.finish_pos.x)
    var viewport_h = Math.max(1, (sb.max_pos.y - sb.min_pos.y) - empire_window.finish_pos.y)
    return Math.max(viewport_w / 1200, viewport_h / 1600)
}

function empire_window_map_clip_origin() {
    var sb = empire_window.screen_bounds
    return {
        x: sb.min_pos.x + empire_window.start_pos.x,
        y: sb.min_pos.y + empire_window.start_pos.y
    }
}

function empire_window_map_area_size() {
    var sb = empire_window.screen_bounds
    return {
        x: Math.max(1, (sb.max_pos.x - sb.min_pos.x) - empire_window.finish_pos.x),
        y: Math.max(1, (sb.max_pos.y - sb.min_pos.y) - empire_window.finish_pos.y)
    }
}

function empire_window_map_base_origin() {
    var clip = empire_window_map_clip_origin()
    var size = empire_window_map_area_size()
    var s = empire_window_map_scale()
    var scaled_w = Math.max(1, Math.round(1200 * s))
    var scaled_h = Math.max(1, Math.round(1600 * s))
    return {
        x: clip.x + Math.max(0, ((size.x - scaled_w) / 2) | 0),
        y: clip.y + Math.max(0, ((size.y - scaled_h) / 2) | 0)
    }
}

function empire_window_map_viewport_size() {
    var size = empire_window_map_area_size()
    var s = Math.max(0.001, empire_window_map_scale())
    return {
        x: Math.min(1200, Math.max(1, Math.round(size.x / s))),
        y: Math.min(1600, Math.max(1, Math.round(size.y / s)))
    }
}

function empire_window_map_draw_origin() {
    var scroll = __empire_map_get_scroll()
    var s = empire_window_map_scale()
    var base = empire_window_map_base_origin()
    return {
        x: base.x - Math.round(scroll.x * s),
        y: base.y - Math.round(scroll.y * s)
    }
}

function empire_window_is_outside_map(x, y) {
    var o = empire_window_map_clip_origin()
    var size = empire_window_map_area_size()
    return x < o.x || x >= o.x + size.x || y < o.y || y >= o.y + size.y
}

function empire_window_map_point(draw_offset, pos) {
    var s = empire_window_map_scale()
    return {
        x: draw_offset.x + Math.round(pos.x * s),
        y: draw_offset.y + Math.round(pos.y * s)
    }
}

function empire_window_consume_scroll_remainder(axis) {
    var rem = (axis == "x") ? empire_window.scroll_remainder_x : empire_window.scroll_remainder_y
    var step = (rem >= 0) ? Math.floor(rem) : Math.ceil(rem)
    if (axis == "x") {
        empire_window.scroll_remainder_x = rem - step
    } else {
        empire_window.scroll_remainder_y = rem - step
    }
    return step
}

function empire_window_apply_map_scroll_delta(dx, dy, scale) {
    var s = Math.max(0.001, scale)
    empire_window.scroll_remainder_x += dx / s
    empire_window.scroll_remainder_y += dy / s
    var map_delta = {
        x: empire_window_consume_scroll_remainder("x"),
        y: empire_window_consume_scroll_remainder("y")
    }
    if (map_delta.x || map_delta.y) {
        __empire_map_scroll_map(map_delta)
    }
}

function empire_window_determine_selected_object() {
    if (!empire_window.screen_bounds) {
        return
    }
    if (!__mouse.left.went_up || empire_window.finished_scroll || empire_window_is_outside_map(__mouse.x, __mouse.y)) {
        return
    }

    var origin = empire_window_map_base_origin()
    var scale = Math.max(0.001, empire_window_map_scale())
    __empire_map_select_object({
        x: Math.max(0, Math.round((__mouse.x - origin.x) / scale)),
        y: Math.max(0, Math.round((__mouse.y - origin.y) / scale))
    })
}

[es=(empire_window, ui_handle_mouse)]
function empire_window_ui_handle_mouse(window) {
    var m = __mouse
    var scale = empire_window_map_scale()

    if (!m.is_touch && m.left.went_down && !empire_window_is_outside_map(m.x, m.y)) {
        empire_window.left_panning = true
        empire_window.left_pan_travel = 0
        empire_window.left_pan_last_pos = { x: m.x, y: m.y }
    }

    if (!m.is_touch && empire_window.left_panning && m.left.is_down) {
        var dx = m.x - empire_window.left_pan_last_pos.x
        var dy = m.y - empire_window.left_pan_last_pos.y
        if (dx || dy) {
            empire_window_apply_map_scroll_delta(-dx, -dy, scale)
            empire_window.left_pan_travel += Math.abs(dx) + Math.abs(dy)
            empire_window.left_pan_last_pos = { x: m.x, y: m.y }
        }
    }

    if (!m.is_touch && m.left.went_up) {
        empire_window.finished_scroll = empire_window.left_pan_travel > __scroll_config.drag_min_delta ? 1 : 0
        empire_window.left_panning = false
        empire_window.left_pan_travel = 0
    }

    if (!empire_window.left_panning) {
        var position = __scroll_get_delta_empire()
        if (position.x || position.y) {
            empire_window_apply_map_scroll_delta(position.x, position.y, scale)
        }
    }

    if (game_features.gameopt_middle_mouse_camera_pan
        && m.middle.went_down && !empire_window_is_outside_map(m.x, m.y)) {
        __scroll_drag_start(SCROLL_DRAG_MIDDLE_MOUSE_PAN)
    }

    if (m.is_touch) {
        var tp = __touch_earliest_current()
        if (!empire_window_is_outside_map(tp.x, tp.y)) {
            if (__touch_earliest_has_started()) {
                __scroll_drag_start(SCROLL_DRAG_TOUCH)
            }
        }
        if (__touch_earliest_has_ended()) {
            empire_window.finished_scroll = __touch_earliest_was_click() ? 0 : 1
            __scroll_drag_end()
        }
    }

    if (m.middle.went_up) {
        __scroll_drag_end()
    }

    empire_window_determine_selected_object()
    empire_window.finished_scroll = 0
}

[es=(empire_window, go_back)]
function empire_window_on_go_back(window) {
    if (empire_window.selected_object) {
        __empire_map_clear_selected_object()
        return
    }
    __scroll_drag_end()
    game.pause_allow = false
    ui.window_city_show()
}

function empire_window_layout_ui(window) {
    var sb = empire_window.screen_bounds
    var centerX = ((sb.min_pos.x + sb.max_pos.x) / 2) | 0
    var width = sb.max_pos.x - sb.min_pos.x
    var infoTop = sb.max_pos.y - 121
    var openTradeTop = sb.max_pos.y - 40
    var infoTooltipTop = sb.max_pos.y - 60
    var sellItemsTop = sb.max_pos.y - 90
    var buyItemsTop = sb.max_pos.y - 90

    window.city_name.pos = { x: sb.min_pos.x, y: infoTop - 1 }
    window.city_name.size = { x: width, y: 20 }

    window.button_help.pos = { x: sb.min_pos.x + 16, y: openTradeTop }
    window.button_close.pos = { x: sb.max_pos.x - 40, y: openTradeTop }
    window.button_advisor.pos = { x: sb.min_pos.x + 16, y: infoTop }
    window.button_pause.pos = { x: sb.max_pos.x - 48, y: infoTop - 2 }

    window.button_open_trade.pos = { x: centerX - 220, y: openTradeTop }
    window.info_tooltip.pos = { x: centerX - 200, y: infoTooltipTop }

    window.city_sell_title.pos = { x: centerX + 250, y: infoTop }
    window.city_sell_items.pos = { x: centerX + 100, y: sellItemsTop }

    window.city_buy_title.pos = { x: centerX - 300, y: infoTop }
    window.city_buy_items.pos = { x: centerX - 430, y: buyItemsTop }

    window.city_want_sell_title.pos = { x: centerX - 220, y: buyItemsTop }
    window.city_want_sell_items.pos = { x: centerX - 170, y: sellItemsTop }

    window.city_want_buy_title.pos = { x: centerX - 220, y: buyItemsTop + 20 }
    window.city_want_buy_items.pos = { x: centerX - 170, y: buyItemsTop + 20 }
}

function empire_window_clear_city_trade_ui(w) {
    w.city_sell_title.enabled = false
    w.city_sell_items.enabled = false
    w.city_buy_title.enabled = false
    w.city_buy_items.enabled = false
    w.city_want_sell_title.enabled = false
    w.city_want_sell_items.enabled = false
    w.city_want_buy_title.enabled = false
    w.city_want_buy_items.enabled = false
}

function empire_window_draw_object_info_none(ev) {
    empire_window_clear_city_trade_ui(ev)
    ev.info_tooltip.text = __loc(47, 9)
}

function empire_window_draw_object_info_ornament(ev) {
    ev.info_tooltip.text = ""
}

function empire_window_draw_object_info_city(ev) {
    ev.info_tooltip.text = ""
    empire_window_clear_city_trade_ui(ev)
    var city = empire.get_city(empire_window.selected_city)
    if (!city) {
        return
    }
    var t = city.type
    var is_open = !!city.is_open
    switch (t) {
    case EMPIRE_CITY_OURS:
        ev.info_tooltip.text = __loc(47, 1)
        break
    case EMPIRE_CITY_PHARAOH:
        ev.info_tooltip.text = __loc(47, 19)
        break
    case EMPIRE_CITY_EGYPTIAN:
        ev.info_tooltip.text = __loc(47, 13)
        break
    case EMPIRE_CITY_FOREIGN:
        ev.info_tooltip.text = __loc(47, 0)
        break
    case EMPIRE_CITY_PHARAOH_TRADING:
    case EMPIRE_CITY_EGYPTIAN_TRADING:
    case EMPIRE_CITY_FOREIGN_TRADING:
        ev.info_tooltip.text = ""
        ev.city_sell_title.enabled = is_open
        ev.city_sell_items.enabled = is_open
        ev.city_buy_title.enabled = is_open
        ev.city_buy_items.enabled = is_open
        ev.city_want_sell_title.enabled = !is_open
        ev.city_want_sell_items.enabled = !is_open
        ev.city_want_buy_title.enabled = !is_open
        ev.city_want_buy_items.enabled = !is_open
        break
    default:
        break
    }
}

function empire_window_draw_object_info_kingdome_army(ev, obj) {
    ev.info_tooltip.text = ""
    var battle = empire.active_battle
    if (battle.egyptian_months_to_travel_back > 0) {
        if (battle.egyptian_months_traveled === obj.distant_battle_travel_months) {
            var sb = empire_window.screen_bounds
            var ox = ((sb.min_pos.x + sb.max_pos.x - 240) / 2) | 0
            var oy = sb.max_pos.y - 68
            var text_id = battle.egyptian_months_to_travel_forth ? 15 : 16
            __lang_text_draw_multiline(47, text_id, ox, oy, 240, FONT_NORMAL_BLACK_ON_LIGHT)
        }
    }
}

function empire_window_draw_object_info_enemy_army(ev, obj) {
    ev.info_tooltip.text = ""
    var battle = empire.active_battle
    if (battle.months_until_battle > 0) {
        // enemy_months_traveled() historically returned egyptian_months_traveled.
        if (battle.egyptian_months_traveled === obj.distant_battle_travel_months) {
            var sb = empire_window.screen_bounds
            var ox = ((sb.min_pos.x + sb.max_pos.x - 240) / 2) | 0
            var oy = sb.max_pos.y - 68
            __lang_text_draw_multiline(47, 14, ox, oy, 240, FONT_NORMAL_BLACK_ON_LIGHT)
        }
    }
}

function empire_window_draw_object_info_other(ev) {
    ev.info_tooltip.text = ""
}

[es=(empire_window, draw_object_info)]
function empire_window_draw_object_info(ev) {
    var obj = empire_window.selected_object
    if (!obj) {
        empire_window_draw_object_info_none(ev)
        return
    }
    switch (obj.type) {
    case EMPIRE_OBJECT_ORNAMENT:
        empire_window_draw_object_info_ornament(ev)
        break
    case EMPIRE_OBJECT_CITY:
        empire_window_draw_object_info_city(ev)
        break
    case EMPIRE_OBJECT_KINGDOME_ARMY:
        empire_window_draw_object_info_kingdome_army(ev, obj)
        break
    case EMPIRE_OBJECT_ENEMY_ARMY:
        empire_window_draw_object_info_enemy_army(ev, obj)
        break
    default:
        empire_window_draw_object_info_other(ev)
        break
    }
}

function empire_window_confirm_open_trade() {
    var city = empire.get_city(empire_window.selected_city)
    if (!city || city.is_sieged) {
        return
    }

    city.is_open = true
    city.empire_object.trade_route_open = 1
    emit event_finance_request{ type: efinance_request_construction, deben: city.cost_to_open }
    emit event_show_window{ id: "trade_opened_window" }
}

[es=(empire_window, init)]
function empire_window_on_init(window) {
    empire_window.scroll_remainder_x = 0
    empire_window.scroll_remainder_y = 0
    empire_window.left_panning = false
    empire_window.left_pan_travel = 0
    empire_window.left_pan_last_pos = { x: 0, y: 0 }
    empire_window.finished_scroll = 0

    game.pause_allow = !!game_features.gameplay_change_empire_map_runs_simulation

    window.button_help.onclick = function() { ui.window_message_dialog_show("message_world_map") }
    window.button_close.onclick = function() {
        game.pause_allow = false
        ui.window_city_show()
    }
    window.button_advisor.onclick = function() {
        game.pause_allow = false
        window_advisors_show_advisor(ADVISOR_TRADE)
    }
    window.button_pause.onclick = function() { emit event_toggle_pause{ value: 0 } }
    window.button_open_trade.onclick = function() {
        ui.show_yesno("#popup_dialog_open_trade", empire_window_confirm_open_trade )
    }
}

[es=(empire_window, draw_pause_button)]
function empire_window_draw_pause_button(window) {
    var btn = window.button_pause
    btn.tooltip = game.paused ? __loc("#TR_BUTTON_RESUME") : __loc("#TR_BUTTON_PAUSE")

    var p = btn.screen_pos
    var sz = btn.size
    if (game.paused) {
        var tri_h = 17
        var tri_w = ((tri_h + 1) / 2) | 0
        var x0 = p.x + (((sz.x - tri_w) / 2) | 0)
        var y0 = p.y + (((sz.y - tri_h) / 2) | 0)
        var half = ((tri_h - 1) / 2) | 0
        for (var r = 0; r < tri_h; r++) {
            var w = (r <= half) ? (r + 1) : (tri_h - r)
            ui.fill_rect({ x: x0, y: y0 + r }, { x: w, y: 1 }, COLOR_BLACK)
        }
    } else {
        var bar_w = 4
        var bar_h = 18
        var gap = 6
        var total_w = 2 * bar_w + gap
        var x0 = p.x + (((sz.x - total_w) / 2) | 0)
        var y0 = p.y + (((sz.y - bar_h) / 2) | 0)
        ui.fill_rect({ x: x0, y: y0 }, { x: bar_w, y: bar_h }, COLOR_BLACK)
        ui.fill_rect({ x: x0 + bar_w + gap, y: y0 }, { x: bar_w, y: bar_h }, COLOR_BLACK)
    }
}

[es=(empire_window, draw_city_want_sell_items)]
function empire_window_es_draw_city_want_sell_items(window) {
    var elm = window[window.active_id]
    var cityId = empire_window.selected_city
    var itemStepX = 110
    var itemStepY = 0
    var rowFont = FONT_SMALL_PLAIN
    var sellIndex = 0

    var city = empire.get_city(cityId)
    for (var r = RESOURCE_GRAIN; r <= RESOURCE_MARBLE; r++) {
        if (!city.city_sells_resource(r)) {
            continue
        }
        var tradeMax = city.trade_route_limit(r)
        tradeMax = __city_resource_stack_proper_quantity(r, tradeMax)
        var pos = { x: elm.screen_pos.x + itemStepX * sellIndex, y: elm.screen_pos.y + itemStepY * sellIndex }
        empire_window_draw_trade_resource_row(pos, window.flags, r, -1, tradeMax, rowFont)
        sellIndex++
    }
 }

[es=(empire_window, draw_city_want_buy_items)]
function empire_window_es_draw_city_want_buy_items(window) {
    var elm = window[window.active_id]
    var cityId = empire_window.selected_city
    var itemStepX = 110
    var itemStepY = 0
    var rowFont = FONT_SMALL_PLAIN
    var buyIndex = 0

    var city = empire.get_city(cityId)
    for (var r = RESOURCE_GRAIN; r <= RESOURCE_MARBLE; r++) {
        if (!city.city_buys_resource(r)) {
            continue
        }
        var tradeMax = city.trade_route_limit(r)
        tradeMax = __city_resource_stack_proper_quantity(r, tradeMax)
        var pos = { x: elm.screen_pos.x + itemStepX * buyIndex, y: elm.screen_pos.y + itemStepY * buyIndex }
        empire_window_draw_trade_resource_row(pos, window.flags, r, -1, tradeMax, rowFont)
        buyIndex++
    }
 }

[es=(empire_window, draw_city_sell_items)]
function empire_window_es_draw_city_sell_items(window) {
    var cityId = empire_window.selected_city
    var itemW = 120
    var itemH = 20
    var rowFont = FONT_SMALL_PLAIN
    var index = 0
    var elm = window[window.active_id]
    var panelW = elm.size.x
    var e_offset_y = elm.screen_pos.y

    var city = empire.get_city(cityId)
    for (var r = RESOURCE_GRAIN; r <= RESOURCE_MARBLE; r++) {
        if (!city.city_sells_resource(r)) {
            continue
        }
        var tradeMax = city.trade_route_limit(r)
        var traded = city.trade_route_traded(r)
        var tradeNow = tradeMax < traded ? tradeMax : traded
        tradeNow = __city_resource_stack_proper_quantity(r, tradeNow)
        tradeMax = __city_resource_stack_proper_quantity(r, tradeMax)

        var local_x = itemW * index
        var pos = { x: elm.screen_pos.x + local_x, y: e_offset_y }
        empire_window_draw_trade_resource_row(pos, window.flags, r, tradeNow, tradeMax, rowFont)
        index++

        if (local_x > panelW) {
            e_offset_y += itemH
            index = 0
        }
    }
 }

[es=(empire_window, draw_city_buy_items)]
function empire_window_es_draw_city_buy_items(window) {
    var cityId = empire_window.selected_city
    var itemW = 120
    var itemH = 20
    var rowFont = FONT_SMALL_PLAIN
    var index = 0
    var elm = window[window.active_id]
    var panelW = elm.size.x
    var e_offset_y = elm.screen_pos.y

    var city = empire.get_city(cityId)
    for (var r = RESOURCE_GRAIN; r <= RESOURCE_MARBLE; r++) {
        if (!city.city_buys_resource(r)) {
            continue
        }

        var tradeMax = city.trade_route_limit(r)
        var traded = city.trade_route_traded(r)
        var tradeNow = tradeMax < traded ? tradeMax : traded

        tradeNow = __city_resource_stack_proper_quantity(r, tradeNow)
        tradeMax = __city_resource_stack_proper_quantity(r, tradeMax)

        var local_x = itemW * index
        var pos = { x: elm.screen_pos.x + local_x, y: e_offset_y }
        empire_window_draw_trade_resource_row(pos, window.flags, r, tradeNow, tradeMax, rowFont)
        index++

        if (local_x > panelW) {
            e_offset_y += itemH
            index = 0
        }
    }
 }

function empire_window_city_image(type) {
    var tid = __empire_city_image_id(type)
    if (tid <= 0) {
        return null
    }
    return get_image({ tid: tid })
}

function empire_window_draw_map_animation(object_index, img, draw_pos, scale) {
    if (!img || !img.animation_speed_id) {
        return
    }
    var frame = __empire_update_map_animation(object_index, img.tid)
    var anim = get_image({ tid: img.tid + frame })
    if (!anim) {
        return
    }
    ui.image_scaled(anim, {
        x: draw_pos.x + Math.round((img.animation_offset_x || 0) * scale),
        y: draw_pos.y + Math.round((img.animation_offset_y || 0) * scale)
    }, scale)
}

function empire_window_draw_city(ev) {
    var obj = empire.get_object(ev.object_index)
    var city = empire.get_city(obj.city_id)
    if (!city || !city.in_use) {
        return
    }

    var img = empire_window_city_image(city.type)
    if (!img) {
        return
    }

    var scale = empire_window_map_scale()
    var draw_pos = empire_window_map_point(ev.draw_offset, obj.map_pos)
    ui.image_scaled(img, draw_pos, scale)

    var scaled_w = Math.max(1, Math.round(img.width * scale))
    var scaled_h = Math.max(1, Math.round(img.height * scale))
    var name = city.name

    if (city.is_sieged) {
        var siege = get_image("pharaoh_general/empire_bits_00001")
        if (siege) {
            ui.image(siege, {
                x: draw_pos.x + ((scaled_w / 2 - siege.width / 2) | 0),
                y: draw_pos.y - siege.height - 5
            })
        }
    }

    empire_window_request_city_trade_route(city, ev.draw_offset, false)

    var letter_h = 11
    var text_pos = {
        x: draw_pos.x + scaled_w,
        y: draw_pos.y + (((scaled_h - letter_h) / 2) | 0)
    }
    ui.label_colored(name, text_pos, FONT_SMALL_PLAIN, COLOR_FONT_DARK_RED)

    if (city.is_sieged) {
        ui.label_colored("under siege", { x: text_pos.x, y: text_pos.y + letter_h + 2 }, FONT_SMALL_PLAIN, COLOR_FONT_RED)
    }

    if (city.type != EMPIRE_CITY_OURS
        && __mouse.x > draw_pos.x && __mouse.y > draw_pos.y
        && __mouse.x < draw_pos.x + scaled_w && __mouse.y < draw_pos.y + scaled_h) {
        ui.set_tooltip(name)
    }

    empire_window_draw_map_animation(ev.object_index, img, draw_pos, scale)
}

function empire_window_draw_text(ev) {
    var obj = empire.get_object(ev.object_index)
    var sp = empire_window_map_point(ev.draw_offset, obj.map_pos)
    ui.label_colored(obj.label, { x: sp.x - 5, y: sp.y }, FONT_SMALL_PLAIN, COLOR_FONT_SHITTY_BROWN)
}

function empire_window_draw_sprite_object(ev, obj) {
    var image_id = obj.map_image_id
    if (!image_id) {
        return
    }
    var img = get_image({ tid: image_id })
    if (!img) {
        return
    }
    var scale = empire_window_map_scale()
    var draw_pos = empire_window_map_point(ev.draw_offset, obj.map_pos)
    ui.image_scaled(img, draw_pos, scale)
    empire_window_draw_map_animation(ev.object_index, img, draw_pos, scale)
}

function empire_window_draw_ornament(ev) {
    empire_window_draw_sprite_object(ev, empire.get_object(ev.object_index))
}

function empire_window_draw_kingdome_army(ev) {
    var obj = empire.get_object(ev.object_index)
    var battle = empire.active_battle
    if (!(battle.egyptian_months_to_travel_forth > 0 || battle.egyptian_months_to_travel_back > 0)) {
        return
    }
    if (battle.egyptian_months_traveled !== obj.distant_battle_travel_months) {
        return
    }
    empire_window_draw_sprite_object(ev, obj)
}

function empire_window_draw_enemy_army(ev) {
    var obj = empire.get_object(ev.object_index)
    var battle = empire.active_battle
    if (battle.months_until_battle <= 0) {
        return
    }
    // Matches distant_battles_t::enemy_months_traveled() (returns egyptian_months_traveled).
    if (battle.egyptian_months_traveled !== obj.distant_battle_travel_months) {
        return
    }
    empire_window_draw_sprite_object(ev, obj)
}

/** Sprites along one segment (spacing matches former C++ trade route / distant battle path). */
function empire_window_route_segment_sprites(img, p1, p2) {
    var dx = p2.x - p1.x
    var dy = p2.y - p1.y
    var len = 0.2 * Math.sqrt(dx * dx + dy * dy)
    if (len <= 0) {
        return
    }
    var scaled_x = dx / len
    var scaled_y = dy / len
    var progress = 1.0
    while (progress < len) {
        ui.image(img, {
            x: p1.x + ((scaled_x * progress) | 0),
            y: p1.y + ((scaled_y * progress) | 0)
        })
        progress += 1.0
    }
}

function empire_window_trade_route_state(city) {
    var rs = empire_window.route_state
    var is_selected = empire_window.selected_city == city.id
    if (city.is_open) {
        return is_selected ? rs.open_selected : rs.open
    }
    return is_selected ? rs.closed_selected : rs.closed
}

function empire_window_request_city_trade_route(city, draw_offset, force) {
    if (!city) {
        return
    }
    if (city.type != EMPIRE_CITY_EGYPTIAN_TRADING
        && city.type != EMPIRE_CITY_FOREIGN_TRADING
        && city.type != EMPIRE_CITY_PHARAOH_TRADING) {
        return
    }

    var rs = empire_window.route_state
    var state = empire_window_trade_route_state(city)
    if ((state == rs.open_selected || state == rs.closed_selected) && !force) {
        empire_window.deferred_route_city_id = city.id
        return
    }

    if (state == rs.closed || empire.trade_route_num_points(city.route_id) <= 0) {
        return
    }

    empire_window_draw_trade_route({
        draw_offset: draw_offset,
        route_id: city.route_id,
        effect: state
    })
}

function empire_window_map_background_image() {
    var cfg = get_mission_config(scenario.campaign_scenario_id)
    if (cfg && cfg.map_background) {
        return get_image(cfg.map_background)
    }
    return get_image(empire_window.image)
}

function empire_window_draw_map_begin(ev) {
    empire_window.deferred_route_city_id = -1

    var img = empire_window_map_background_image()
    if (img) {
        ui.image_scaled(img, ev.draw_offset, empire_window_map_scale())
    }
}

function empire_window_draw_map_objects(ev) {
    var payload = { draw_offset: ev.draw_offset, object_index: 0 }
    for (var i = 0; i < empire.object_slots; i++) {
        var obj = empire.get_object(i)
        if (!obj.in_use) {
            continue
        }
        // LAND/SEA pak route markers skipped: city routes drawn via request_city_trade_route.
        // TRADER slots skipped: live traders drawn from g_empire_traders.
        switch (obj.type) {
        case EMPIRE_OBJECT_LAND_TRADE_ROUTE:
        case EMPIRE_OBJECT_SEA_TRADE_ROUTE:
        case EMPIRE_OBJECT_TRADER:
            break
        case EMPIRE_OBJECT_CITY:
            payload.object_index = i
            empire_window_draw_city(payload)
            break
        case EMPIRE_OBJECT_TEXT:
            payload.object_index = i
            empire_window_draw_text(payload)
            break
        case EMPIRE_OBJECT_ORNAMENT:
            payload.object_index = i
            empire_window_draw_ornament(payload)
            break
        case EMPIRE_OBJECT_KINGDOME_ARMY:
            payload.object_index = i
            empire_window_draw_kingdome_army(payload)
            break
        case EMPIRE_OBJECT_ENEMY_ARMY:
            payload.object_index = i
            empire_window_draw_enemy_army(payload)
            break
        case EMPIRE_OBJECT_BATTLE_ICON:
            payload.object_index = i
            empire_window_draw_battle_icon(payload)
            break
        case EMPIRE_OBJECT_DISTANT_BATTLE_ROUTE:
            payload.object_index = i
            empire_window_draw_distant_battle_path(payload)
            break
        }
    }
}

function empire_window_draw_invasion_warnings(ev) {
    var scale = empire_window_map_scale()
    for (var i = 0; i < invasions.warning_slots; i++) {
        var w = invasions.get_warning(i)
        if (!w.in_use || !w.handled) {
            continue
        }
        var img = get_image({ tid: w.image_id })
        if (!img) {
            continue
        }
        ui.image_scaled(img, empire_window_map_point(ev.draw_offset, w.pos), scale)
    }
}

function empire_window_draw_deferred_trade_route(ev) {
    var cid = empire_window.deferred_route_city_id
    empire_window.deferred_route_city_id = -1
    if (cid < 0) {
        return
    }
    var city = empire.get_city(cid)
    if (!city || !city.in_use) {
        return
    }
    empire_window_request_city_trade_route(city, ev.draw_offset, true)
}

function empire_window_draw_trade_route(ev) {
    var rs = empire_window.route_state
    if (ev.effect == rs.closed) {
        return
    }
    var route_id = ev.route_id
    var n = empire.trade_route_num_points(route_id)
    if (n <= 0) {
        return
    }
    var imgDesc = null
    switch (ev.effect) {
    case rs.closed_selected:
        imgDesc = empire_window.closed_trade_route_hl
        break
    case rs.open:
        imgDesc = empire_window.open_trade_route
        break
    case rs.open_selected:
        imgDesc = empire_window.open_trade_route_hl
        break
    default:
        return
    }
    var img = get_image(imgDesc)
    if (!img) {
        return
    }
    for (var i = 0; i < n; i++) {
        var p = empire.trade_route_point(route_id, i)
        var sp = empire_window_map_point(ev.draw_offset, p)
        ui.image(img, sp)
        if (i < n - 1) {
            var p2 = empire.trade_route_point(route_id, i + 1)
            var sp2 = empire_window_map_point(ev.draw_offset, p2)
            empire_window_route_segment_sprites(img, sp, sp2)
            if (empire.route_debug_points) {
                ui.fill_rect({ x: sp.x - 4, y: sp.y - 4 }, { x: 8, y: 8 }, COLOR_BLACK)
            }
        }
    }
}

function empire_window_draw_trader(ev) {
    var t = empire.get_trader(ev.index)
    if (!t || !t.is_active) {
        return
    }

    var img = get_image({ pack: PACK_GENERAL, id: 179, offset: t.is_ship ? 0 : 1 })
    if (!img) {
        return
    }

    ui.image(img, empire_window_map_point(ev.draw_offset, t.current_position))
}

function empire_window_draw_traders(ev) {
    var payload = { draw_offset: ev.draw_offset, index: 0 }
    for (var i = 0; i < empire.trader_slots; i++) {
        payload.index = i
        empire_window_draw_trader(payload)
    }
}

function empire_window_draw_battle_icon(ev) {
    var obj = empire.get_object(ev.object_index)
    var img = get_image("pharaoh_general/empire_bits_00001")
    if (!img) {
        return
    }

    ui.image(img, empire_window_map_point(ev.draw_offset, obj.map_pos))
}

function empire_window_draw_distant_battle_path(ev) {
    if (!empire.has_distant_battle) {
        return
    }

    var n = empire.active_battle.path_length
    if (n <= 0) {
        return
    }

    var img = get_image(empire_window.open_trade_route)
    if (!img) {
        return
    }

    for (var i = 0; i < n; i++) {
        var p = empire.active_battle.path_point(i)
        var sp = empire_window_map_point(ev.draw_offset, p)
        ui.image(img, sp)
        if (i < n - 1) {
            var p2 = empire.active_battle.path_point(i + 1)
            var sp2 = empire_window_map_point(ev.draw_offset, p2)
            empire_window_route_segment_sprites(img, sp, sp2)
            if (empire.route_debug_points) {
                ui.fill_rect({ x: sp.x - 4, y: sp.y - 4 }, { x: 8, y: 8 }, COLOR_BLACK)
            }
        }
    }
}

function empire_window_draw_distant_battle_icon(ev) {
    if (!empire.has_distant_battle) {
        return
    }

    var ecity = empire.get_city(empire.active_battle.city)
    if (!ecity) {
        return
    }

    var battle_icon = get_image("pharaoh_general/empire_bits_00001")
    if (!battle_icon) {
        return
    }

    var battle_icon_pos = vec2i(empire_window_map_point(ev.draw_offset, ecity.empire_object.pos))
                            .add({x:-battle_icon.width / 2, y:-battle_icon.height / 2})

    ui.image(battle_icon, battle_icon_pos)
}

function empire_window_draw_dispatched_army_icon(ev) {
    if (empire.dispatched_army.state <= 0) {
        return
    }

    var army_icon = get_image("pharaoh_general/empire_bits_00009")
    if (!army_icon) {
        return
    }

    var army_icon_pos = vec2i(empire_window_map_point(ev.draw_offset, empire.dispatched_army.pos))
                            .add({x:-army_icon.width / 2, y:-army_icon.height / 2})

    ui.image(army_icon, army_icon_pos)
}

function empire_window_draw_map(window) {
    var clip = empire_window_map_clip_origin()
    var area = empire_window_map_area_size()
    ui.set_clip_rectangle(clip, area)
    __empire_map_set_viewport(empire_window_map_viewport_size())
    ui.set_tooltip("")

    var draw_offset = empire_window_map_draw_origin()
    empire_window.draw_offset = draw_offset
    var payload = { draw_offset: draw_offset }

    empire_window_draw_map_begin(payload)
    empire_window_draw_map_objects(payload)
    empire_window_draw_invasion_warnings(payload)
    empire_window_draw_traders(payload)
    empire_window_draw_distant_battle_path(payload)
    empire_window_draw_deferred_trade_route(payload)

    var wpos = window.pos || { x: 0, y: 0 }
    ui.begin_widget(wpos)
    empire_window_draw_distant_battle_icon(payload)
    empire_window_draw_dispatched_army_icon(payload)
    ui.end_widget()

    ui.reset_clip_rectangle()
}

function empire_window_update_selection_ui(window) {
    var city = null
    var cityId = empire_window.selected_city
    if (cityId) {
        city = empire.get_city(cityId)
    }

    window.city_name.text = city ? city.name : ""
    window.button_help.enabled = !!city
    window.button_close.enabled = true
    window.button_advisor.enabled = !!city

    var may_open_trade = city && !city.is_open && city.can_trade
    window.button_open_trade.enabled = may_open_trade
    if (may_open_trade) {
        window.button_open_trade.text = __loc("#debens") + " " + city.cost_to_open + " "
            + __loc(47, 6 + (city.is_sea_trade ? 1 : 0))
    }
}

function empire_window_draw_paneling(window) {
    var min_pos = empire_window.screen_bounds.min_pos
    var max_pos = empire_window.screen_bounds.max_pos

    var bottom = get_image(empire_window.bottom_image)
    var hbar = get_image(empire_window.horizontal_bar)
    var vbar = get_image(empire_window.vertical_bar)
    var cross = get_image(empire_window.cross_bar)

    ui.set_clip_rectangle(min_pos, vec2i(max_pos).sub(min_pos))

    for (var x = min_pos.x; x < max_pos.x; x += 70) {
        ui.image(bottom, {x: x, y: max_pos.y - 140})
        ui.image(bottom, {x: x, y: max_pos.y - 100})
        ui.image(bottom, {x: x, y: max_pos.y - 60})
        ui.image(bottom, {x: x, y: max_pos.y - 20})
    }

    for (var x = min_pos.x; x < max_pos.x; x += 86) {
        ui.image(hbar, {x: x, y: min_pos.y})
        ui.image(hbar, {x: x, y: max_pos.y - 140})
        ui.image(hbar, {x: x, y: max_pos.y - 16})
    }

    for (var y = min_pos.y + 16; y < max_pos.y; y += 86) {
        ui.image(vbar, {x: min_pos.x, y: y})
        ui.image(vbar, {x: max_pos.x - 16, y: y})
    }

    ui.image(cross, {x: min_pos.x, y: min_pos.y})
    ui.image(cross, {x: min_pos.x, y: max_pos.y - 140})
    ui.image(cross, {x: min_pos.x, y: max_pos.y - 16})
    ui.image(cross, {x: max_pos.x - 16, y: min_pos.y})
    ui.image(cross, {x: max_pos.x - 16, y: max_pos.y - 140})
    ui.image(cross, {x: max_pos.x - 16, y: max_pos.y - 16})

    ui.reset_clip_rectangle()
}

[es=(empire_window, draw_background)]
function empire_window_draw_background(window) {
    game.pause_allow = !!game_features.gameplay_change_empire_map_runs_simulation
    var bounds = empire_window_screen_bounds()
    empire_window.screen_bounds = bounds
    empire_window_layout_ui(window)

    empire_window_draw_map(window)
    empire_window_update_selection_ui(window)
    empire_window_draw_paneling(window)
}
