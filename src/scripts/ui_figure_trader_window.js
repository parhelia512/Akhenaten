log_info("akhenaten: ui trader info window started")

function figure_trader_info_window_resource_icons(trade, traded, buy_side) {
    var items = ""
    for (var r = RESOURCE_GRAIN; r < RESOURCE_DEBEN; r++) {
        var show = false
        var amount = 0
        if (traded) {
            amount = buy_side ? trade.bought_amount(r) : trade.sold_amount(r)
            show = amount > 0
        } else {
            show = buy_side ? trade.city.city_buys_resource(r) : trade.city.city_sells_resource(r)
        }
        if (!show) {
            continue
        }
        var icon = __image_id_resource_icon_int(r)
        if (traded) {
            items += " @I" + icon + "& " + amount + "  "
        } else {
            items += "@I" + icon + "&   "
        }
    }
    return items
}

function figure_trader_info_window_fill_trade(window, fid) {
    var trade = city.get_figure_trade(fid)
    if (!trade.valid) {
        return
    }

    var cap_label = trade.per_good ? "#trader_capacity_per_good" : "#trader_capacity"
    window.capacity.text = __loc(cap_label) + " " + trade.capacity

    if (trade.has_traded) {
        window.buy.text = __loc("#trader_bought")
        window.sell.text = __loc("#trader_sold")
        window.buy_text.text = figure_trader_info_window_resource_icons(trade, true, true)
        window.sell_text.text = figure_trader_info_window_resource_icons(trade, true, false)
    } else {
        window.buy.text = __loc("#trader_buys")
        window.sell.text = __loc("#trader_sells")
        window.buy_text.text = figure_trader_info_window_resource_icons(trade, false, true)
        window.sell_text.text = figure_trader_info_window_resource_icons(trade, false, false)
    }
}

function figure_trader_info_window_update_toolbar(window, f) {
    if (!f.valid) {
        return
    }

    window.show_path.text = (f.draw_mode & e_figure_draw_routing) ? "P" : "p"
    var following = __figure_follow_enabled() && __figure_follow_figure_id() == f.id
    window.show_follow.text = following ? "F" : "f"
}

[es=figure_info_window]
figure_trader_info_window {
    related_figures [FIGURE_TRADE_SHIP, FIGURE_TRADE_CARAVAN, FIGURE_TRADE_CARAVAN_DONKEY]

    ui {
        background: outer_panel({ size[29, 19] })
        inner_panel: inner_panel({ pos[16, 40], size[27, 12] })
        bigimage: image({ pos[26, 50], pack: PACK_UNLOADED, id: 25 })
        name: text({ pos[90, 58], text: "${figure.name}", font: FONT_LARGE_BLACK_ON_DARK })
        typename: text({ pos[92, 86], text: "${figure.class_name} @Y${figure.city_name}&", font: FONT_NORMAL_BLACK_ON_DARK, rich: true, scroll: false })

        action: text({ pos[92, 106], text: "(${figure.action_tip})", font: FONT_NORMAL_YELLOW })
        capacity: text({ pos[92, 130], font: FONT_NORMAL_BLACK_ON_DARK })

        buy: text({ pos[92, 150], text: "${loc.trader_bought}", font: FONT_NORMAL_BLACK_ON_DARK })
        buy_text: text({ pos[150, 150], font: FONT_NORMAL_BLACK_ON_DARK, rich: true, scroll: false })

        sell: text({ pos[92, 170], text: "${loc.trader_sold}", font: FONT_NORMAL_BLACK_ON_DARK })
        sell_text: text({ pos[150, 170], font: FONT_NORMAL_BLACK_ON_DARK, rich: true, scroll: false })

        phrase: text({ pos[90, 200], font: FONT_NORMAL_BLACK_ON_DARK, wrap: px(21), multiline: true, scroll: false })

        button_help: help_button({})
        button_close: close_button({})

        show_path: button({ margin{right: -64, bottom: -40}, size[23, 23], onclick_event: "show_path" })
        show_follow: button({ margin{right: -90, bottom: -40}, size[23, 23], text:"F", tooltip:"#follow_walker", onclick_event: "show_follow" })
    }
}

[es=(figure_trader_info_window, init)]
function figure_trader_info_window_init(window) {
    var fid = __object_info_figure_id()
    figure_info_window_setup(window, fid)
    figure_trader_info_window_fill_trade(window, fid)
}

[es=(figure_trader_info_window, show_path)]
function figure_trader_info_window_on_show_path(window) {
    var f = city.get_figure(__object_info_figure_id())
    if (!f.valid) {
        return
    }
    f.draw_mode = f.draw_mode ^ e_figure_draw_routing
}

[es=(figure_trader_info_window, show_follow)]
function figure_trader_info_window_on_show_follow(window) {
    __figure_follow_start(__object_info_figure_id())
}

[es=(figure_trader_info_window, window_info_background)]
function figure_trader_info_window_window_info_background(window) {
    var f = city.get_figure(__object_info_figure_id())

    window.name.text = f.name
    window.typename.text = f.class_name + " @Y" + f.city_name + "&"
    window.action.text = "(" + f.action_tip + ")"

    figure_trader_info_window_update_toolbar(window, f)
    figure_info_check_phrase(window)
}
