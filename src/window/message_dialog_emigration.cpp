#include "message_dialog_emigration.h"

#include "city/city.h"
#include "io/gamefiles/lang.h"
#include "core/string.h"

void ui::message_dialog_emigration::draw_city_message_text(const lang_message& msg) {
    xstring text = resolve_message_body(msg);
    if (!text) {
        return;
    }

    bstring1024 header;
    format_city_message_header(header);

    bstring1024 full_text;
    int city_sentiment = g_city.sentiment.low_mood_cause;
    if (city_sentiment >= 1 && city_sentiment <= 5) {
        full_text.printf("%s @P%s @P%s", header.c_str(), text.c_str(), ui::str(12, city_sentiment + 2));
    } else {
        full_text.printf("%s @P%s", header.c_str(), text.c_str());
    }

    ui["content_text"] = full_text;
}
