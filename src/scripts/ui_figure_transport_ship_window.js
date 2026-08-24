log_info("akhenaten: ui figure transport ship window started")

function figure_transport_ship_company_on_board_text_id(figure_type) {
    if (figure_type == FIGURE_ARCHER) { return 31 }
    if (figure_type == FIGURE_FCHARIOTEER) { return 32 }
    if (figure_type == FIGURE_INFANTRY) { return 33 }
    return 33
}

function figure_transport_ship_onboard_text(ship) {
    if (!ship.valid || !ship.has_troops) {
        return ""
    }

    var form = city.get_formation(ship.transported_formation)
    if (!form || !form.in_use) {
        return ""
    }

    var company_id = figure_transport_ship_company_on_board_text_id(form.figure_type)
    return __loc(184, company_id) + " " + __loc(184, 34)
}

function figure_transport_ship_action_text(ship) {
    if (!ship.has_troops) {
        return { header: __loc(184, 23), text: __loc(184, 24) }
    }
    if (ship.phase == 1) {
        return { header: __loc(184, 23), text: __loc(184, 24) }
    }
    return { header: __loc(184, 25), text: __loc(184, 26) }
}

[es=figure_info_window]
figure_transport_ship_info_window {
    related_figures [FIGURE_TRANSPORT_SHIP]

    ui {
        background       : outer_panel({size: [29, 23]}),
        name             : text_center({pos: [16, 16], size: [px(27), 20], text:"${figure.class_name}", font : FONT_LARGE_BLACK_ON_DARK }),
        onboard          : text({pos: [102, 58], text:"", font : FONT_NORMAL_BLACK_ON_DARK }),

        embark           : button({pos:[48, 134], size:[160, 25], text:"${184.23}", onclick_event: "embark" }),
        disembark        : button({pos:[248, 134], size:[160, 25], text:"${184.25}", onclick_event: "disembark" }),

        inner_panel      : inner_panel({pos : [16, 220], size: [27, 6],
            ui : {
                action_header: text({pos: [10, 10], font : FONT_NORMAL_WHITE_ON_DARK }),
                action_text : text({pos: [10, 30], font : FONT_NORMAL_BLACK_ON_DARK, wrap:px(21), multiline:true }),
            }
        }),

        button_help      : help_button({}),
        button_close     : close_button({}),
        show_follow      : button({margin:{right:-64, bottom:-40}, size:[23, 23], text:"F", tooltip:"#follow_walker", onclick_event: "show_follow"}),
    }
}

[es=(figure_transport_ship_info_window, init)]
function figure_transport_ship_info_window_init(window) {
    var fid = __object_info_figure_id()
    __figure_info_set_help(fid)
    window.name.text = city.get_figure(fid).class_name
}

[es=(figure_transport_ship_info_window, embark)]
function figure_transport_ship_info_window_on_embark(window) {
    var ship = city.get_transport_ship(__object_info_figure_id())
    if (!ship.valid || !ship.can_embark) {
        return
    }
    __window_city_transport_show(ship.id, TRANSPORT_PICK_FORMATION)
}

[es=(figure_transport_ship_info_window, disembark)]
function figure_transport_ship_info_window_on_disembark(window) {
    var ship = city.get_transport_ship(__object_info_figure_id())
    if (!ship.valid || !ship.has_troops) {
        return
    }
    __window_city_transport_show(ship.id, TRANSPORT_PICK_LANDING)
}

[es=(figure_transport_ship_info_window, show_follow)]
function figure_transport_ship_info_window_on_show_follow(window) {
    __figure_follow_start(__object_info_figure_id())
}

[es=(figure_transport_ship_info_window, window_info_background)]
function figure_transport_ship_info_window_window_info_background(window) {
    var f = city.get_figure(__object_info_figure_id())
    var ship = city.get_transport_ship(f.id)

    window.name.text = f.class_name
    window.embark.darkened = ship.can_embark ? 0 : UiFlags_Grayscale
    window.disembark.darkened = ship.has_troops ? 0 : UiFlags_Grayscale
    window.onboard.text = figure_transport_ship_onboard_text(ship)

    var action = figure_transport_ship_action_text(ship)
    window.action_header.text = action.header
    window.action_text.text = action.text

    var following = __figure_follow_enabled() && __figure_follow_figure_id() == f.id
    window.show_follow.text = following ? "F" : "f"
}
