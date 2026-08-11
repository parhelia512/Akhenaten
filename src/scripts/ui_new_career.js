[es=window]
window_new_career {
    pos [(sw(0) - px(24))/2, (sh(0) - px(22))/2]
    allow_rmb_goback : true
    player_name_value : ""
    ui {
        background_image: background({pack:PACK_UNLOADED, id:31})
        background : outer_panel({size[24, 22]})
        title      : text_center({pos[0, 12], size[px(24), 20], font:FONT_LARGE_BLACK_ON_LIGHT, text[31, 0]})
        player_name: input({margin{left:32, top:48}, size[20, 2], font:FONT_NORMAL_WHITE_ON_DARK, max_length:31, allow_punctuation:1, oninput: new_career_on_input})
        names_title: text({pos[32, 84], text[13, 8], font:FONT_NORMAL_BLACK_ON_LIGHT})
        name_list  : scrollable_list({
            margin{left:16, top:104}
            size[20, 12]
            view_items:11
            draw_scrollbar_always:true
            draw_paneling:true
            onclick_item: new_career_on_pick_name
            ondoubleclick_item: new_career_on_double_click_name
        })
        btn_back   : button({margin{left:31, top:312}, size[px(9), 26], text[12, 0], font:FONT_NORMAL_BLACK_ON_LIGHT, onclick: window_go_back})
        btn_ok     : button({margin{centerx:16, top:312}, size[px(9), 26], text[13, 5], font:FONT_NORMAL_BLACK_ON_LIGHT, onclick: new_career_btn_ok})
    }
}

function new_career_on_input(params) {
    window_new_career.player_name_value = params.value || ""
}

function new_career_apply_name(name) {
    if (!name) {
        return
    }
    // Loc strings may include trailing spaces from the original language packs.
    while (name.length > 0 && name.charAt(name.length - 1) === " ") {
        name = name.substring(0, name.length - 1)
    }
    window_new_career.player_name.value = name
    window_new_career.player_name_value = name
}

function new_career_on_pick_name(entry) {
    new_career_apply_name(entry.text)
}

function new_career_on_double_click_name(entry) {
    new_career_apply_name(entry.text)
    new_career_btn_ok()
}

function new_career_btn_ok() {
    var name = window_new_career.player_name_value
    if (!name || name === "") {
        ui.show_ok("#popup_dialog_no_player_name")
        return
    }
    game.dynasty_name = name
    __game_player_data_new(name)
    window_go_back()
    window_show_by_id("window_player_selection")
}

function new_career_fill_egyptian_names(window) {
    window.name_list.clear()
    var i
    for (i = 0; i < 125; i++) {
        var name = __loc(151, i)
        if (name && name !== "" && name.charAt(0) !== "#") {
            window.name_list.add_item(name, i)
        }
    }
}

[es=(window_new_career, init)]
function window_new_career_on_init(window) {
    __scenario_init()
    city.kingdome.campaign_carry_personal_savings = 0
    city.kingdome.personal_savings = 0
    __campaign_carry_clear()

    window.player_name.value = game.dynasty_name || "My Dynasty"
    window_new_career.player_name_value = window.player_name.value
    new_career_fill_egyptian_names(window)
}
