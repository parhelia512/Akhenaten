#include "building/monument_royal_tomb.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_resource.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

#include <algorithm>

struct info_window_royal_tomb : building_info_window_t<info_window_royal_tomb> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!c.building_get()->dcast_royal_tomb();
    }
};

info_window_royal_tomb royal_tomb_infow;

void info_window_royal_tomb::init(object_info &c) {
    building_info_window::init(c);

    auto *st = c.building_get()->dcast_monument();
    if (!st) {
        return;
    }

    auto &d = st->runtime_data();
    auto *rt = c.building_get()->dcast_royal_tomb();

    if (st->is_unfinished()) {
        textid reason = {178, 144}; // artisans/masons progressing
        int stonemasons = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);
        int artisans = g_city.buildings.count_active(BUILDING_ARTISANS_GUILD);

        if (d.phase == 0 || st->needs_resource(RESOURCE_LAMPS) > 0) {
            reason = {178, 143}; // refuse without lamps
        } else if (rt && rt->lamp_stock() <= 0) {
            reason = {178, 143};
        } else if (st->need_stonemason() && stonemasons == 0) {
            reason = {178, 14};
        } else if (st->need_artisan() && artisans == 0) {
            reason = {178, 144};
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
            const int art_phases = std::max(1, st->phases());
            min_pct = std::min(99, (d.phase * 100) / art_phases);
        }

        bstring64 progress_str;
        progress_str.printf("%d%%", min_pct);
        ui["progress_text"] = textid{199, 77}; // "The Small Royal Burial Tomb is"
        ui["progress_pct"] = progress_str;
    } else {
        ui["warning_text"] = textid{178, 145}; // complete — needs burial provisions
        ui["progress_text"] = textid{199, 78};
        ui["progress_pct"] = "100%";
    }
}
