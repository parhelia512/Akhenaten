#include "window_building_info.h"

#include "building/building_zoo.h"
#include "city/object_info.h"
#include "graphics/elements/lang_text.h"
#include "graphics/elements/ui.h"
#include "io/gamefiles/lang.h"
#include "window/building/common.h"

struct info_window_zoo : public building_info_window_t<info_window_zoo> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        building *b = c.building_get();
        return b && b->type == BUILDING_ZOO;
    }
};

info_window_zoo g_zoo_infow;

void info_window_zoo::init(object_info &c) {
    building_info_window::init(c);

    building *b = c.building_get();
    auto *zoo = b ? b->dcast_entertainment() : nullptr;
    if (!b || !zoo) {
        return;
    }

    auto &d = zoo->runtime_data();

    // group 308: 1=ok, 2=needs meat, 3=needs straw, 4=no workers, 5=empty cages
    // Animals present (juggler_visited) wins over empty-feed warnings so a just-fed zoo
    // is not shown as "needs meat" while cages are still occupied.
    textid reason{c.group_id, 0};
    if (!b->has_road_access) {
        ui["warning_text"] = lang_text_from_key("#building_no_road_access");
    } else if (b->num_workers <= 0) {
        reason.id = 4;
        ui["warning_text"] = ui::str(reason);
    } else if (d.juggler_visited) {
        reason.id = 1;
        ui["warning_text"] = ui::str(reason);
    } else if (b->stored_amount(RESOURCE_GAMEMEAT) <= 0) {
        reason.id = 2;
        ui["warning_text"] = ui::str(reason);
    } else if (b->stored_amount(RESOURCE_STRAW) <= 0) {
        reason.id = 3;
        ui["warning_text"] = ui::str(reason);
    } else {
        reason.id = 5;
        ui["warning_text"] = ui::str(reason);
    }

    ui["workers_desc"] = "";
    draw_employment_details_ui(ui, c, b, /*text_id*/-1);

    // group 308 ids 6–7: "Game Meat:" / "Straw:"
    ui["advice"] = bstring256().printf("%s %d  %s %d",
        ui::str(308, 6), b->stored_amount(RESOURCE_GAMEMEAT),
        ui::str(308, 7), b->stored_amount(RESOURCE_STRAW));
}
