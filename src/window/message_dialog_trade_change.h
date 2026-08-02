#pragma once

#include "message_dialog_new.h"

namespace ui {
    struct message_dialog_trade_change : public message_dialog_base {
        message_dialog_trade_change() : message_dialog_base("message_dialog_window_trade_change") {}
        virtual void draw_city_message_text(const lang_message& msg) override;
    };
}
