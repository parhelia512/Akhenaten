#include "building/monument_obelisk.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_resource.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

struct info_window_obelisk : building_info_window_t<info_window_obelisk> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.building_get()->dcast_obelisk();
    }
};

info_window_obelisk obelisk_infow;

void info_window_obelisk::init(object_info &c) {
    building_info_window::init(c);

    auto obelisk = c.building_get()->dcast_monument();
    if (!obelisk) {
        return;
    }

    auto &d = obelisk->runtime_data();

    if (obelisk->is_unfinished()) {
        textid reason = {199, 43}; // Work on this obelisk has not yet begun…

        int carpenters = g_city.buildings.count_active(BUILDING_CARPENTERS_GUILD);
        int stonemasons = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);

        if (d.phase <= 1 && !carpenters && obelisk->needs_resource(RESOURCE_TIMBER) > 0) {
            reason = {178, 42};
        } else if (obelisk->need_stonemason() && stonemasons > 0) {
            reason = {178, 43};
        } else if (obelisk->need_stonemason() && !stonemasons) {
            reason = {199, 43};
        } else {
            reason = {178, 42};
        }

        ui["warning_text"] = reason;

        int min_pct = 100;
        bool any_resource = false;
        for (int ri = (int)RESOURCES_MIN; ri <= (int)RESOURCES_MAX; ++ri) {
            auto r = (e_resource)ri;
            if (obelisk->needs_resource(r) <= 0) {
                continue;
            }
            any_resource = true;
            min_pct = std::min(min_pct, (int)d.resources_pct[r]);
        }
        if (!any_resource) {
            min_pct = 100;
        }

        bstring64 progress_str;
        progress_str.printf("%d / %d    %d%%", (int)d.phase, obelisk->phases(), min_pct);
        ui["progress_text"] = progress_str;

        auto fill_resource_slot = [&](e_resource r, pcstr icon_key, pcstr text_key) {
            int needed = obelisk->needs_resource(r);
            if (needed <= 0) {
                ui[text_key] = "";
                ui[icon_key].set_enabled(false);
                return;
            }
            ui[icon_key].set_enabled(true);
            int delivered = std::min(needed * d.resources_pct[r] / 100, needed);
            bstring64 s;
            s.printf("%d / %d", delivered, needed);
            ui[text_key] = s;
        };
        fill_resource_slot(RESOURCE_TIMBER, "timber_icon", "timber_text");
        fill_resource_slot(RESOURCE_GRANITE, "granite_icon", "granite_text");
    } else {
        ui["warning_text"] = textid{199, 46}; // This obelisk is now complete!
        ui["progress_text"] = "";
        ui["timber_text"] = "";
        ui["granite_text"] = "";
        ui["timber_icon"].set_enabled(false);
        ui["granite_icon"].set_enabled(false);
    }
}
