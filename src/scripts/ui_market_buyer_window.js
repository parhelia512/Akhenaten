log_info("akhenaten: ui market buyer window started")

[es=ui_window]
figure_market_buyer_window {
    related_figures [FIGURE_MARKET_BUYER]
    ui {
        background     : outer_panel({size [29, 22]})
        inner_panel    : inner_panel({pos [16, 40], size [27, 13] })
        border         : border({border:0, pos : [24, 102], size [px(26), 138] })
        bigimage       : image({pos [30, 108], pack:PACK_UNLOADED, id:25 })
        name           : text({pos [90, 108], text:"${figure.name}", font : FONT_LARGE_BLACK_ON_DARK })
        typename       : text({pos [92, 139], text:"${figure.class_name}", font : FONT_NORMAL_BLACK_ON_DARK })
        phrase         : text({pos [90, 160], font : FONT_NORMAL_BLACK_ON_DARK, wrap:px(21), multiline:true })

        button_figure0 : image_button({pos[60 * 0 + 27, 45], size[52, 52], border:true, param1:0, onclick_event:"select_figure" })
        button_figure1 : image_button({pos[60 * 1 + 27, 45], size[52, 52], border:true, param1:1, onclick_event:"select_figure" })
        button_figure2 : image_button({pos[60 * 2 + 27, 45], size[52, 52], border:true, param1:2, onclick_event:"select_figure" })
        button_figure3 : image_button({pos[60 * 3 + 27, 45], size[52, 52], border:true, param1:3, onclick_event:"select_figure" })
        button_figure4 : image_button({pos[60 * 4 + 27, 45], size[52, 52], border:true, param1:4, onclick_event:"select_figure" })
        button_figure5 : image_button({pos[60 * 5 + 27, 45], size[52, 52], border:true, param1:5, onclick_event:"select_figure" })
        button_figure6 : image_button({pos[60 * 6 + 27, 45], size[52, 52], border:true, param1:6, onclick_event:"select_figure" })

        resource_image : resource_icon({ pos[240, 92] })
        resource_text  : text({ pos[92, 170], text: "${figure.action_tip}", font: FONT_NORMAL_BLACK_ON_DARK })
        // resource_amount: text({ pos[122, 170], text: "${figure.resource_amount}", font: FONT_NORMAL_BLACK_ON_DARK })

        button_help    : help_button({})
        button_close   : close_button({})

        show_path      : button({margin{right:-64, bottom:-40}, size[23, 23]})
        show_overlay   : button({margin{right:-90, bottom:-40}, size[23, 23]})
        show_follow    : button({margin{right:-116, bottom:-40}, size[23, 23], text:"F", tooltip:"#follow_walker"})
    }
}

[es=(figure_market_buyer_window, init)]
function figure_market_buyer_window_init(window) {
    figure_info_window_setup(window, window.figure_id)
}

[es=(figure_market_buyer_window, select_figure)]
function figure_market_buyer_window_on_select_figure(ev) {
    __object_info_select_figure(ev.param1)
}

[es=(figure_market_buyer_window, draw_background)]
function figure_market_buyer_window_draw_background(window) {
    var f = city.get_figure(__object_info_figure_id())

    window.name.text = f.name
    window.typename.text = f.class_name
    window.resource_text.text = f.action_tip

    figure_info_window_update_toolbar(window, f)
    figure_info_window_sync_tab_selection(window)
}