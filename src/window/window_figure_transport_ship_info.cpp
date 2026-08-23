#include "window/window_figure_info.h"
#include "window/window_city_transport.h"

#include "figuretype/figure_transport_ship.h"
#include "figure/formation.h"
#include "figure/figure_type.h"
#include "graphics/elements/ui.h"

struct figure_transport_ship_info_window : public figure_info_window_t<figure_transport_ship_info_window> {
    virtual void init(object_info &c) override;
    virtual void window_info_background(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.figure_get<figure_transport_ship>();
    }
};

figure_transport_ship_info_window figure_transport_ship_infow;

static int company_on_board_text_id(e_figure_type type) {
    switch (type) {
    case FIGURE_ARCHER: return 31;
    case FIGURE_FCHARIOTEER: return 32;
    case FIGURE_INFANTRY: return 33;
    default: return 33;
    }
}

void figure_transport_ship_info_window::init(object_info &c) {
    figure_info_window::init(c);

    figure_transport_ship *ship = c.figure_get<figure_transport_ship>();
    if (!ship) {
        return;
    }

    const int ship_id = ship->id();

    ui["embark"].onclick([ship_id] (int, int) {
        figure *f = ::figure_get(ship_id);
        figure_transport_ship *s = f ? smart_cast<figure_transport_ship>(f) : nullptr;
        if (!s || !s->can_embark()) {
            return;
        }
        window_city_transport_show(ship_id, TRANSPORT_PICK_FORMATION);
    });

    ui["disembark"].onclick([ship_id] (int, int) {
        figure *f = ::figure_get(ship_id);
        figure_transport_ship *s = f ? smart_cast<figure_transport_ship>(f) : nullptr;
        if (!s || !s->has_troops()) {
            return;
        }
        window_city_transport_show(ship_id, TRANSPORT_PICK_LANDING);
    });
}

void figure_transport_ship_info_window::window_info_background(object_info &c) {
    common_info_window::window_info_background(c);

    figure_transport_ship *ship = c.figure_get<figure_transport_ship>();
    if (!ship) {
        return;
    }

    const bool can_board = ship->can_embark();
    const bool has_troops = ship->has_troops();

    ui["embark"].darkened = can_board ? UiFlags_None : UiFlags_Grayscale;
    ui["disembark"].darkened = has_troops ? UiFlags_None : UiFlags_Grayscale;

    if (has_troops) {
        formation *m = formation_get(ship->transported_formation());
        if (m && m->in_use) {
            const int company_id = company_on_board_text_id(m->figure_type);
            ui["onboard"].text_var("%s %s", ui::str(c.group_id, company_id), ui::str(c.group_id, 34));
        } else {
            ui["onboard"].text_var("%s", "");
        }
        // Boarding in progress: keep Embark blurb; otherwise Disembark prompt.
        if (ship->runtime_data().phase == 1) {
            ui["action_header"].text_var("%s", ui::str(c.group_id, 23));
            ui["action_text"].text_var("%s", ui::str(c.group_id, 24));
        } else {
            ui["action_header"].text_var("%s", ui::str(c.group_id, 25));
            ui["action_text"].text_var("%s", ui::str(c.group_id, 26));
        }
    } else {
        ui["onboard"].text_var("%s", "");
        ui["action_header"].text_var("%s", ui::str(c.group_id, 23));
        ui["action_text"].text_var("%s", ui::str(c.group_id, 24));
    }
}
