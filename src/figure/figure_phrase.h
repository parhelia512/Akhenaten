#pragma once

#include "core/xstring.h"
#include "figure/figure_type.h"

using sound_key = xstring;

struct event_synthesize_figure_phrase {
    int id;
    xstring path;
    xstring text;
};

inline constexpr pcstr FIGURE_PHRASE_WAITING = "waiting_for_phrase";

struct sound_key_state {
    sound_key prefix;
    bool valid;
};
