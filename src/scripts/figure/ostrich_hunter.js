log_info("akhenaten: figure ostrich_hunter started")

function figure_ostrich_hunter_gods_are_angry() {
    for (var i = 0; i < gods.length; i++) {
        var god = gods[i]
        if (!city.gods.is_known(god.type)) {
            continue
        }
        if (city.gods.at(god.type).mood < 51) {
            return true
        }
    }
    return false
}

function figure_ostrich_hunter_city_phrase_key() {
    var keys = []
    var mood_cause = city.sentiment.low_mood_cause

    if (city.health_rating < 30) {
        keys.push("hunter_disease_risk")
    }

    if (mood_cause == 1) { // LOW_MOOD_NO_FOOD
        keys.push("hunter_no_food_in_city")
    }

    if (city.num_forts < 1) {
        keys.push("hunter_city_have_no_army")
    }

    if (city.labor.workers_needed >= 10) {
        keys.push("hunter_need_workers")
    }

    if (figure_ostrich_hunter_gods_are_angry()) {
        keys.push("hunter_gods_are_angry")
    }

    if (city.kingdome.rating < 30) {
        keys.push("hunter_city_is_bad")
    }

    if (mood_cause == 2) { // LOW_MOOD_NO_JOBS
        keys.push("hunter_much_unemployment")
    }

    if (__city_festival.months_since_festival > 6) {
        keys.push("hunter_low_entertainment")
    }

    var sentiment = city.sentiment.value
    if (sentiment > 50) {
        keys.push("hunter_city_is_good")
    }

    if (sentiment > 90) {
        keys.push("hunter_city_is_amazing")
    }

    if (keys.length == 0) {
        return "hunter_back"
    }

    return keys[Math.floor(Math.random() * keys.length)]
}

[es=(figure_ostrich_hunter, setup_phrase)]
function figure_ostrich_hunter_setup_phrase(ev) {
    var f = city.get_figure(ev.fid)
    if (!f || !f.valid) {
        return
    }

    var state = f.action_state
    switch (state) {
    case ACTION_16_OSTRICH_HUNTER_INVESTIGATE:
        figure_apply_phrase(f, "hunter_investigate")
        return
    case ACTION_9_OSTRICH_HUNTER_CHASE_PREY:
        figure_apply_phrase(f, "hunter_chase")
        return
    case ACTION_15_OSTRICH_HUNTER_HUNT:
        figure_apply_phrase(f, "hunter_hunting")
        return
    case ACTION_12_OSTRICH_HUNTER_MOVE_PACKED:
        figure_apply_phrase(f, "hunter_back")
        return
    case ACTION_12_OSTRICH_HUNTER_MOVE_RANDOM_PACKED:
        figure_apply_phrase(f, "hunter_reroute_packed")
        return
    case ACTION_12_OSTRICH_HUNTER_LOOK_RANDOM_PACKED:
        figure_apply_phrase(f, "hunter_look_packed")
        return
    case ACTION_14_OSTRICH_HUNTER_UNLOADING:
        figure_apply_phrase(f, "hunter_unloading")
        return
    }

    figure_apply_phrase(f, figure_ostrich_hunter_city_phrase_key())
}
