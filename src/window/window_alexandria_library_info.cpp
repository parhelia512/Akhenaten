#include "building/monument_alexandria_library.h"

#include "building/monuments.h"
#include "city/city.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

#include <algorithm>

struct info_window_alexandria_library : building_info_window_t<info_window_alexandria_library> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.building_get()->dcast_alexandria_library();
    }
};

info_window_alexandria_library alexandria_library_infow;

void info_window_alexandria_library::init(object_info &c) {
    building_info_window::init(c);

    auto *st = c.building_get()->dcast_monument();
    if (!st) {
        return;
    }

    auto &d = st->runtime_data();

    if (st->is_unfinished()) {
        textid reason = {178, 114};
        int work_camps = g_city.buildings.count_active(BUILDING_WORK_CAMP);
        int carpenters = g_city.buildings.count_active(BUILDING_CARPENTERS_GUILD);
        int stonemasons = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);

        auto still_needs = [&](e_resource r) {
            return st->needs_resource(r) > 0 && d.resources_pct[r] < 100;
        };

        if (d.phase < 2 && work_camps == 0) {
            reason = {199, 59}; // not begun — need peasant labor (+ marble/copper 60)
        } else if (d.phase < 2) {
            reason = {178, 114}; // clearing the land
        } else if (still_needs(RESOURCE_TIMBER) && carpenters == 0) {
            reason = {178, 16};
        } else if (still_needs(RESOURCE_COPPER)) {
            reason = stonemasons ? textid{178, 119} : textid{178, 14};
        } else if (still_needs(RESOURCE_MARBLE) && !stonemasons) {
            reason = {178, 14};
        } else if (d.phase == 2) {
            reason = stonemasons ? textid{178, 115} : textid{178, 14};
        } else if (d.phase == 3) {
            reason = stonemasons ? textid{178, 116} : textid{178, 14};
        } else if (d.phase == 4) {
            reason = stonemasons ? textid{178, 117} : textid{178, 14};
        } else if (d.phase == 5) {
            reason = stonemasons ? textid{178, 118} : textid{178, 14};
        } else if (d.phase == 6) {
            reason = stonemasons ? textid{178, 119} : textid{178, 14};
        } else if (d.phase == 7) {
            reason = {178, 120};
        } else {
            reason = {178, 110};
        }

        ui["warning_text"] = reason;

        int min_pct = 100;
        bool any_resource = false;
        for (int ri = (int)RESOURCES_MIN; ri < (int)RESOURCES_MAX; ++ri) {
            auto r = (e_resource)ri;
            if (st->needs_resource(r) <= 0) {
                continue;
            }
            any_resource = true;
            min_pct = std::min(min_pct, (int)d.resources_pct[r]);
        }
        if (!any_resource) {
            const int art_phases = std::max(1, st->phases() - 1);
            min_pct = std::min(99, (d.phase * 100) / art_phases);
        }

        bstring64 progress_str;
        progress_str.printf("%d%%", min_pct);
        ui["progress_text"] = textid{199, 61}; // "The Library of Alexandria is"
        ui["progress_pct"] = progress_str;

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
        fill_resource_slot(RESOURCE_MARBLE, "marble_icon", "marble_text");
        fill_resource_slot(RESOURCE_TIMBER, "timber_icon", "timber_text");
        fill_resource_slot(RESOURCE_COPPER, "copper_icon", "copper_text");
    } else {
        ui["warning_text"] = textid{178, 121}; // complete
        ui["progress_text"] = textid{199, 62};
        ui["progress_pct"] = "";
        ui["marble_text"] = "";
        ui["timber_text"] = "";
        ui["copper_text"] = "";
        ui["marble_icon"].set_enabled(false);
        ui["timber_icon"].set_enabled(false);
        ui["copper_icon"].set_enabled(false);
    }
}
