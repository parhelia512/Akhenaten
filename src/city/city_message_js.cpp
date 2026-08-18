#include "city/city_message.h"

#include "core/tokenum.h"
#include "js/js_game.h"
#include "js/js_global_object.h"

using e_mesage_category_tokens_t = token_holder<e_mesage_category, MESSAGE_CAT_RIOT, MESSAGE_CAT_SIZE>;
const e_mesage_category_tokens_t ANK_CONFIG_ENUM(e_mesage_category_tokens);

ANK_GLOBAL_OBJECT(g_message_manager, __city_messages,
    consecutive_message_delay,
    next_message_sequence,
    total_messages,
    current_message_id,
    problem_count,
    problem_index);

void __city_message_post_with_popup_delay(int category, bool force_popup, xstring text, int param1, int param2, bool hide_img) {
    city_message &msg = city_message_post_with_popup_delay((e_mesage_category)category, force_popup, text, param1, (short)param2);
    msg.hide_img = hide_img;
}
ANK_FUNCTION_6(__city_message_post_with_popup_delay)
