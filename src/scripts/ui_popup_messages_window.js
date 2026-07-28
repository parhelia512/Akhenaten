log_info("akhenaten: ui popup messages window started")

// OG group 310 ids 2..13 → e_popup_message_category 0..11
var POPUP_MSG_COUNT = 12

function popup_messages_pending_want(window, cat) {
    var mask = Math.round(window.pending_mask)
    return (mask & (1 << cat)) != 0
}

function popup_messages_refresh_row_fonts(window) {
    for (var i = 0; i < POPUP_MSG_COUNT; i++) {
        var btn = window["cat_" + i]
        if (!btn) {
            continue
        }
        btn.font = popup_messages_pending_want(window, i) ? FONT_NORMAL_YELLOW : FONT_NORMAL_BLACK_ON_LIGHT
    }
}

function popup_messages_toggle_cat(cat) {
    var w = popup_messages_window
    var bit = 1 << cat
    var mask = Math.round(w.pending_mask)
    if ((mask & bit) != 0) {
        mask = mask & ~bit
    } else {
        mask = mask | bit
    }
    w.pending_mask = mask
    popup_messages_refresh_row_fonts(w)
}

function popup_messages_on_ok() {
    var w = popup_messages_window
    game_features.gameopt_popup_messages = Math.round(w.pending_mask)
    window_go_back()
}

function popup_messages_on_cancel() {
    // Discard pending; live gameopt unchanged.
    window_go_back()
}

[es=(popup_messages_window, init)]
function popup_messages_window_es_init(window) {
    var cur = Math.round(game_features.gameopt_popup_messages)
    window.original_mask = cur
    window.pending_mask = cur
    popup_messages_refresh_row_fonts(window)
}

[es=(popup_messages_window, ui_draw_foreground)]
function popup_messages_window_es_draw(window) {
    popup_messages_refresh_row_fonts(window)
}

[es=modal_window]
popup_messages_window {
    allow_rmb_goback : true
    draw_underlying : true
    pos [(sw(0) - px(24))/2, (sh(0) - px(22))/2]
    original_mask : 0
    pending_mask : 0

    ui {
        background : outer_panel({size[24, 22]})
        title      : header({pos[0, 12], size[px(24), 20], text[310, 0], align:"center"})
        subtitle   : text({pos[16, 40], size[px(22), 20], text[310, 1], font: FONT_NORMAL_BLACK_ON_LIGHT, align:"center"})

        cat_0  : button({pos[48, 70],  size[288, 22], text[310, 2],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(0) } })
        cat_1  : button({pos[48, 92],  size[288, 22], text[310, 3],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(1) } })
        cat_2  : button({pos[48, 114], size[288, 22], text[310, 4],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(2) } })
        cat_3  : button({pos[48, 136], size[288, 22], text[310, 5],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(3) } })
        cat_4  : button({pos[48, 158], size[288, 22], text[310, 6],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(4) } })
        cat_5  : button({pos[48, 180], size[288, 22], text[310, 7],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(5) } })
        cat_6  : button({pos[48, 202], size[288, 22], text[310, 8],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(6) } })
        cat_7  : button({pos[48, 224], size[288, 22], text[310, 9],  hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(7) } })
        cat_8  : button({pos[48, 246], size[288, 22], text[310, 10], hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(8) } })
        cat_9  : button({pos[48, 268], size[288, 22], text[310, 11], hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(9) } })
        cat_10 : button({pos[48, 290], size[288, 22], text[310, 12], hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(10) } })
        cat_11 : button({pos[48, 312], size[288, 22], text[310, 13], hbody:false, border:false, font: FONT_NORMAL_BLACK_ON_LIGHT, onclick: function() { popup_messages_toggle_cat(11) } })

        // OG group 310 id 14/15 — text Ok/Cancel (not image check/X used by other options).
        btnok     : button({margin{left:px(24)/2 - 60, bottom:-36}, size[100, 25], text[310, 14], onclick: popup_messages_on_ok })
        btncancel : button({margin{left:px(24)/2 + 20, bottom:-36}, size[100, 25], text[310, 15], onclick: popup_messages_on_cancel })
    }
}
