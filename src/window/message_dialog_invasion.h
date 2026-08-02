#pragma once

#include "message_dialog_new.h"

namespace ui {
    struct message_dialog_invasion : public message_dialog_base {
        message_dialog_invasion() : message_dialog_base("message_dialog_window_invasion") {}
        virtual void draw_city_message_text(const lang_message& msg) override;
        virtual void init_data(xstring text_id, int message_id, void (*background_callback)(void)) override;

        void button_go_to_problem();
    };
}
