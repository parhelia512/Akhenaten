log_info("akhenaten: ui figure carrier window started")

var DIR_FIGURE_REROUTE = 9
var DIR_FIGURE_CAN_NOT_REACH = 10

function figure_carrier_carrying_line_text(fid) {
    if (!fid) {
        return ""
    }

    var f = city.get_figure(fid)
    if (!f.valid) {
        return ""
    }
    if (!f.resource || f.resource_amount <= 0) {
        return ""
    }

    var icon = __image_id_resource_icon_int(f.resource)
    return "@I" + icon + " " + f.resource_amount + " " + __loc(129, 20) + " " + __loc(23, f.resource)
}

function figure_carrier_stuck_action_name(action) {
    switch (action) {
    case 8: return "Recalculating route"
    case 9: return "Delivering goods"
    case 10: return "Delivering food"
    case 11: return "Delivering gold"
    case 20: return "Initial state"
    case 21: return "To warehouse"
    case 22: return "To granary"
    case 23: return "To workshop"
    case 27: return "Returning home"
    case 51: return "Delivering resource"
    case 53: return "Returning empty"
    case 54: return "Getting food"
    case 57: return "Getting resource"
    default: return "Unknown action"
    }
}

function figure_carrier_stuck_reason(fid) {
    var direction = __figure_get_direction(fid)
    if (direction == DIR_FIGURE_REROUTE) {
        return "Rerouting..."
    }
    if (direction == DIR_FIGURE_CAN_NOT_REACH) {
        return "Cannot reach destination"
    }
    if (__figure_is_on_previous_tile(fid) && __figure_get_wait_ticks(fid) > 0) {
        return "Waiting at building"
    }
    if (__figure_is_on_previous_tile(fid)) {
        return "Blocked - no movement"
    }
    return ""
}

function figure_carrier_stuck_debug_text(fid) {
    if (!fid) {
        return ""
    }

    var f = city.get_figure(fid)
    if (!f.valid) {
        return ""
    }

    var watchdog = __figure_get_movement_watchdog(fid)
    var direction = __figure_get_direction(fid)
    if (watchdog <= 0 && direction != DIR_FIGURE_CAN_NOT_REACH) {
        return ""
    }

    return "STUCK: Watchdog=" + watchdog
        + " | " + figure_carrier_stuck_action_name(f.action_state)
        + " | " + figure_carrier_stuck_reason(fid)
}

[es=figure_info_window]
figure_carrier_info_window {
    related_figures [FIGURE_CART_PUSHER, FIGURE_STORAGEYARD_CART]

    ui : baseui(figure_info_window, {
        typename    : text({pos: [92, 139], text:"${figure.class_name} ( @Y${figure.home}& )", font : FONT_NORMAL_BLACK_ON_DARK, rich:true, scroll:false }),
        items       : text({pos: [102, 158], size:[px(29), 20], font : FONT_NORMAL_BLACK_ON_DARK, rich:true, scroll:false }),
        phrase      : text({pos: [90, 180], font : FONT_NORMAL_BLACK_ON_DARK, wrap:px(22), multiline:true }),
        debug_stuck : text({pos: [120, 260], font : FONT_NORMAL_BLACK_ON_DARK, wrap:px(22), multiline:true }),
    })
}

[es=(figure_carrier_info_window, init)]
function figure_carrier_info_window_init(window) {
    var fid = __object_info_figure_id()
    figure_info_window_setup(window, fid)
    window.items.text = figure_carrier_carrying_line_text(fid)
    window.debug_stuck.text = figure_carrier_stuck_debug_text(fid)
}

[es=(figure_carrier_info_window, window_info_background)]
function figure_carrier_info_window_window_info_background(window) {
    var fid = __object_info_figure_id()
    var f = city.get_figure(fid)

    window.name.text = f.name
    window.typename.text = f.class_name + " ( @Y" + f.home + "& )"
    window.items.text = figure_carrier_carrying_line_text(fid)
    window.debug_stuck.text = figure_carrier_stuck_debug_text(fid)

    figure_info_window_update_toolbar(window, f)
    figure_info_window_sync_tab_selection(window)
}
