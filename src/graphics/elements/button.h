#pragma once

#include "input/mouse.h"
#include "core/string.h"
#include "core/vec2i.h"

void button_none(int param1, int param2);
bool mouse_inside_clip(vec2i);

// Hit-test: true if the mouse is over [context + item.pos(), item.size()).
// Works for any type with pos()/size() — buttons, images, probes, etc.
template<class T>
bool is_mouse_over(const T &item, vec2i context) {
    const mouse& m = mouse::get();
    vec2i ipos = context + item.pos();
    vec2i isize = item.size();

    if (!mouse_inside_clip(m)) {
        return false;
    }

    return (   ipos.x <= m.x && ipos.x + isize.x > m.x
            && ipos.y <= m.y && ipos.y + isize.y > m.y);
}

namespace ui {
    namespace textured {
        void button_border_draw(vec2i pos, vec2i size, bool has_focus);
    }
}

extern void (*button_border_draw)(vec2i, vec2i, bool);

