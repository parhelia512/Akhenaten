log_info("akhenaten: ui figure hunter window started")

function figure_hunter_carrying_line_text(fid) {
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

[es=figure_info_window]
figure_hunter_info_window {
    related_figures [FIGURE_OSTRICH_HUNTER, FIGURE_ANTELOPE_HUNTER, FIGURE_BIRDS_HUNTER]

    ui : baseui(figure_info_window, {
        typename : text({pos: [92, 139], text:"${figure.class_name} ( @Y${figure.home}& )", font : FONT_NORMAL_BLACK_ON_DARK, rich:true, scroll:false }),
        items    : text({pos: [102, 158], size:[px(29), 20], font : FONT_NORMAL_BLACK_ON_DARK, rich:true, scroll:false }),
        phrase   : text({pos: [90, 180], font : FONT_NORMAL_BLACK_ON_DARK, wrap:px(22), multiline:true }),
    })
}

[es=(figure_hunter_info_window, init)]
function figure_hunter_info_window_init(window) {
    var fid = __object_info_figure_id()
    figure_info_window_setup(window, fid)
    window.items.text = figure_hunter_carrying_line_text(fid)
}

[es=(figure_hunter_info_window, window_info_background)]
function figure_hunter_info_window_window_info_background(window) {
    var f = city.get_figure(__object_info_figure_id())

    window.name.text = f.name
    window.typename.text = f.class_name + " ( @Y" + f.home + "& )"
    window.items.text = figure_hunter_carrying_line_text(fid)

    figure_info_window_update_toolbar(window, f)
    figure_info_window_sync_tab_selection(window)
}
