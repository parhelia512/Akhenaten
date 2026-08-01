#include "building/monument_abu_simbel.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_resource.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

#include <algorithm>

struct info_window_abu_simbel : building_info_window_t<info_window_abu_simbel> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.building_get()->dcast_abu_simbel();
    }
};

info_window_abu_simbel abu_simbel_infow;

void info_window_abu_simbel::init(object_info &c) {
    building_info_window::init(c);

    auto *st = c.building_get()->dcast_monument();
    if (!st) {
        return;
    }

    auto &d = st->runtime_data();

    if (st->is_unfinished()) {
        // Foreman: gr178:141 in progress; guild missing → generic carpenter/mason lines.
        textid reason = {178, 141};
        int carpenters = g_city.buildings.count_active(BUILDING_CARPENTERS_GUILD);
        int stonemasons = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);

        if (d.phase == 0 && carpenters == 0 && stonemasons == 0) {
            reason = {199, 71}; // not begun — need carpenters, stonemasons + wood
        } else if (st->needs_resource(RESOURCE_TIMBER) > 0) {
            reason = carpenters ? textid{178, 141} : textid{178, 16};
        } else if (st->need_stonemason()) {
            reason = stonemasons ? textid{178, 141} : textid{178, 14};
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
            // Mason / no-delivery phases: show phase progress, not a fake 100%.
            const int art_phases = std::max(1, st->phases() - 1);
            min_pct = std::min(99, (d.phase * 100) / art_phases);
        }

        bstring64 progress_str;
        progress_str.printf("%d%%", min_pct);
        ui["progress_text"] = textid{199, 73}; // "Abu Simbel is"
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
        fill_resource_slot(RESOURCE_TIMBER, "timber_icon", "timber_text");
    } else {
        ui["warning_text"] = textid{178, 142}; // complete figures welcome
        ui["progress_text"] = textid{199, 74};
        ui["progress_pct"] = "";
        ui["timber_text"] = "";
        ui["timber_icon"].set_enabled(false);
    }
}
