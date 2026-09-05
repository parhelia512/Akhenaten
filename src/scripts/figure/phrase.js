log_info("akhenaten: figure phrase started")

function figure_apply_phrase(f, key) {
    f.phrase_key = key
    f.phrase_sound = f.sound_path(key)
}

function figure_info_on_phrase_ready(ev) {
    var oi = city.object_info
    if (!oi.can_play_sound) {
        return
    }

    var fid = ev.fid
    var f = city.get_figure(fid)
    if (!f.valid) {
        oi.can_play_sound = false
        return
    }

    if (!f.type || f.params.category == figure_category_animal) {
        oi.can_play_sound = false
        return
    }

    var text = __figure_phrase_text(fid)
    if (ev.phrase) {
        ev.phrase.text = text
    }

    var path = "Voice/Walker/" + f.phrase_sound
    if (!__game_sound.speech_play(path)) {
        emit event_synthesize_figure_phrase{ id: fid, path:path, text:text }
    }

    oi.can_play_sound = false
}

function figure_info_check_phrase(window) {
    var oi = city.object_info
    if (!oi.can_play_sound) {
        return
    }

    var fid = __object_info_figure_id()
    var f = city.get_figure(fid)
    if (!f.valid) {
        oi.can_play_sound = false
        return
    }

    if (!f.phrase_key || f.phrase_key == "waiting_for_phrase" || !f.phrase_sound) {
        return
    }

    figure_info_on_phrase_ready({ fid: fid, phrase: window.phrase })
}

[es=(figure_info_window, phrase_ready)]
function figure_info_window_on_phrase_ready(ev) {
    figure_info_on_phrase_ready(ev)
}

[es=(figure_enemy_info_window, phrase_ready)]
function figure_enemy_info_window_on_phrase_ready(ev) {
    figure_info_on_phrase_ready(ev)
}
