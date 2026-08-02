#include "message_dialog_tutorial.h"

#include "io/gamefiles/lang.h"

void ui::message_dialog_tutorial::draw_city_message_text(const lang_message& msg) {
    xstring text = resolve_message_body(msg);
    if (!text) {
        return;
    }
    ui["content_text"] = text;
}
