#pragma once

#include <string>
#include <algorithm>

#include "autoconfig_window.h"
#include "core/xstring.h"
#include "core/bstring.h"
#include "graphics/elements/rich_text.h"
#include "graphics/elements/lang_text.h"
#include "graphics/elements/image_button.h"
#include "io/gamefiles/lang.h"
#include "city/constants.h"
#include "scenario/scenario.h"
#include "game/gods.h"
#include "core/vec2i.h"

struct text_tag_substitution {
    xstring tag;
    xstring content;
};

namespace ui {
    // Forward declarations
    struct message_dialog_disaster;
    struct message_dialog_imperial;
    struct message_dialog_emigration;
    struct message_dialog_tutorial;
    struct message_dialog_trade_change;
    struct message_dialog_price_change;
    struct message_dialog_invasion;
    struct message_dialog_god;
    struct message_dialog_image;
    struct message_dialog_troop_request;

    // Base class with common functionality
    struct message_dialog_base : public autoconfig_window {
        message_dialog_base(pcstr config_name);

        virtual int handle_mouse(const mouse *m) override;
        virtual int get_tooltip_text() override { return 0; }
        virtual void draw_foreground(UiFlags flags) override;
        virtual int draw_background(UiFlags flags) override;
        virtual void ui_draw_foreground(UiFlags flags) override {}
        virtual int ui_handle_mouse(const mouse *m) override;
        virtual bool is_modal() const override { return true; }
        virtual void init() override;

        void show(xstring text_id, int message_id, void (*background_callback)(void));
        void show_city_message(xstring text_id, int message_id, int year, int month, int param1, int param2, int message_advisor, bool use_popup);
        void show_with_video(pcstr video_path, pcstr title);
        void setup_help_id(xstring helpid);

        xstring debug_text_id;

        uint16_t text_id;
        int message_id;
        bool is_eventmsg;
        xstring title_text;
        bstring1024 body_text;
        bstring256 phrase_text;
        xstring body_template;
        xstring phrase_template;

        void (*background_callback)();
        bool show_video;
        bstring256 video_override;
        bstring256 video_override_title;

        int text_height_blocks;
        int text_width_blocks;
        xstring help_id;
        xstring subtitle_text;

        struct player_message {
            int year;
            int month;
            int param1;
            int param2;
            int message_advisor;
            bool use_popup;
        } player_msg;

        virtual void init_data(xstring text_id, int message_id, void (*background_callback)(void));
        void set_city_message(int year, int month, int param1, int param2, int message_advisor, bool use_popup);

        template<typename T>
        void eventmsg_template_combine(pcstr template_ptr, T& out_ptr, bool phrase_modifier);
        void cleanup();

        virtual void draw_background_content();
        virtual void draw_background_video();

        virtual void draw_foreground_content();
        virtual void draw_foreground_video();

        void draw_image(const lang_message& msg);
        virtual void draw_content(const lang_message& msg);
        virtual void draw_city_message_text(const lang_message& msg);

        xstring resolve_message_body(const lang_message& msg) const;
        static bstring256 normalize_video_path(pcstr raw);
        static bstring256 resolve_message_video_path(const lang_message& msg);
        void apply_video_ui(bool enabled);
        void format_city_message_header(bstring1024& out) const;

        virtual bool handle_input_normal(const mouse* m_dialog, const lang_message& msg);
        bool handle_input_video(const mouse* m_dialog, const lang_message& msg);

        void button_close();
        void button_help();
        void button_advisor(int advisor);

        int resource_image(int resource);
        int get_message_image_id(const lang_message& msg);

        virtual xstring section() const override;

    protected:
        xstring config_name;
    };

    struct message_dialog_general : public message_dialog_base {
        message_dialog_general() : message_dialog_base("message_dialog_window_general") {}
    };
}

void window_message_dialog_show(xstring text_id, int message_id, void (*background_callback)(void));
void window_message_dialog_show_city_message(xstring text_id, int message_id, int year, int month, int param1, int param2, int message_advisor, bool use_popup);
void window_message_dialog_show_with_video(pcstr video_path, pcstr title);
void window_message_setup_help_id(xstring helpid);
void window_show_help();

template<typename R, typename T>
void text_fill_in_tags(pcstr src, R& result, const T &tag_templates) {
    if (!src) {
        result = "";
        return;
    }

    size_t pos = 0;
    constexpr size_t max_tag_length = 200;
    result = src;

    while ((pos = result.find('[', pos)) != std::string::npos) {
        size_t end_pos = result.find(']', pos);
        if (end_pos == std::string::npos || (end_pos - pos) > max_tag_length) {
            ++pos;
            continue;
        }

        auto full_tag = result.substr(pos, end_pos - pos + 1);
        auto tag_it = std::find_if(std::begin(tag_templates), std::end(tag_templates), 
                                    [&full_tag](const auto& tag_template) { return tag_template.tag == full_tag; });

        if (tag_it != std::end(tag_templates)) {
            pcstr content_cstr = tag_it->content.c_str();
            if (content_cstr) {
                result.replace_str(full_tag.c_str(), content_cstr);
                pos += tag_it->content.size();
            } else {
                pos = end_pos + 1;
            }
        } else {
            pos = end_pos + 1;
        }
    }
}

