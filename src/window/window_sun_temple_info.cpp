#include "building/monument_sun_temple.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_resource.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

#include <algorithm>

struct info_window_sun_temple : building_info_window_t<info_window_sun_temple> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.building_get()->dcast_sun_temple();
    }
};

info_window_sun_temple sun_temple_infow;

void info_window_sun_temple::init(object_info &c) {
    building_info_window::init(c);

    auto *st = c.building_get()->dcast_monument();
    if (!st) {
        return;
    }

    auto &d = st->runtime_data();

    if (st->is_unfinished()) {
        // Foreman lines gr178:53–57 map roughly to phase bands.
        textid reason = {199, 47};
        int carpenters = g_city.buildings.count_active(BUILDING_CARPENTERS_GUILD);
        int stonemasons = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);

        if (d.phase < 2) {
            reason = {178, 53}; // clearing site / work camps
        } else if (d.phase == 2 && st->needs_resource(RESOURCE_TIMBER) > 0) {
            reason = carpenters ? textid{178, 54} : textid{199, 48};
        } else if (st->need_stonemason() && d.phase == 3) {
            reason = stonemasons ? textid{178, 55} : textid{199, 49};
        } else if (st->need_stonemason() && d.phase == 4) {
            reason = stonemasons ? textid{178, 56} : textid{199, 49};
        } else {
            reason = {178, 57};
        }

        ui["warning_text"] = reason;

        int min_pct = 100;
        bool any_resource = false;
        for (int ri = (int)RESOURCES_MIN; ri <= (int)RESOURCES_MAX; ++ri) {
            auto r = (e_resource)ri;
            if (st->needs_resource(r) <= 0) {
                continue;
            }
            any_resource = true;
            min_pct = std::min(min_pct, (int)d.resources_pct[r]);
        }
        if (!any_resource) {
            min_pct = 100;
        }

        // Map remake phases 0–4 onto gr253 Phase 1–4 (0–1 = Phase 1).
        int display_phase = d.phase;
        if (display_phase <= 1) {
            display_phase = 1;
        }
        bstring64 progress_str;
        progress_str.printf("%d / %d    %d%%", display_phase, 4, min_pct);
        ui["progress_text"] = progress_str;

        auto fill_resource_slot = [&](e_resource r, pcstr icon_key, pcstr text_key) {
            int needed = st->needs_resource(r);
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
        fill_resource_slot(RESOURCE_SANDSTONE, "sandstone_icon", "sandstone_text");
    } else {
        ui["warning_text"] = textid{199, 50};
        ui["progress_text"] = "";
        ui["timber_text"] = "";
        ui["sandstone_text"] = "";
        ui["timber_icon"].set_enabled(false);
        ui["sandstone_icon"].set_enabled(false);
    }
}
