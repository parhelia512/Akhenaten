log_info("akhenaten: figure ostrich_hunter started")

[es=(figure_ostrich_hunter, setup_phrase)]
function figure_ostrich_hunter_setup_phrase(ev) {
    var f = city.get_figure(ev.fid)
    if (!f || !f.valid) {
        return
    }

    var state = f.action_state
    if (state == ACTION_16_OSTRICH_HUNTER_INVESTIGATE
        || state == ACTION_9_OSTRICH_HUNTER_CHASE_PREY
        || state == ACTION_15_OSTRICH_HUNTER_HUNT) {
        var keys = ["hunter_hunting", "hunter_test_1", "hunter_test_2", "hunter_test_3"]
        figure_apply_phrase(f, keys[Math.floor(Math.random() * keys.length)])
        return
    }

    if (state == ACTION_8_OSTRICH_HUNTER_RECALCULATE && city.sentiment.value > 40) {
        figure_apply_phrase(f, "hunter_city_is_good")
        return
    }

    figure_apply_phrase(f, "hunter_back")
}
