#pragma once

#include "message_dialog_new.h"

namespace ui {
    struct message_dialog_emigration : public message_dialog_base {
        message_dialog_emigration() : message_dialog_base("message_dialog_window_emigration") {}
        virtual void draw_city_message_text(const lang_message& msg) override;
    };
}
