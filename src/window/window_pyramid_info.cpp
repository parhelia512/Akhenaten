#include "building/monument_pyramid.h"

#include "building/monuments.h"
#include "city/city.h"
#include "city/city_resource.h"
#include "game/game.h"
#include "game/resource.h"
#include "window/building/common.h"
#include "window/window_building_info.h"

struct info_window_pyramid : building_info_window_t<info_window_pyramid> {
    virtual void init(object_info &c) override;
    virtual bool check(object_info &c) override {
        return !!smart_cast<building_pyramid>(c.building_get());
    }
};

info_window_pyramid pyramid_infow;

void info_window_pyramid::init(object_info &c) {
    building_info_window::init(c);

    // Workers / resources_pct live on the chain head — clicking a corner/wall
    // part must not read empty local slots.
    auto *clicked = c.building_get()->dcast_monument();
    if (!clicked) {
        return;
    }
    auto *pyramid = clicked->main()->dcast_monument();
    if (!pyramid) {
        return;
    }

    auto &d = pyramid->runtime_data();
    const e_building_type mon_btype = pyramid->config().btype;
    const bool is_true = (mon_btype == BUILDING_SMALL_PYRAMID
        || mon_btype == BUILDING_MEDIUM_PYRAMID
        || mon_btype == BUILDING_LARGE_PYRAMID);
    int polish_begin = 24;
    if (mon_btype == BUILDING_MEDIUM_PYRAMID) {
        polish_begin = building_medium_pyramid::k_polish_phase_begin;
    } else if (mon_btype == BUILDING_LARGE_PYRAMID) {
        polish_begin = building_large_pyramid::k_polish_phase_begin;
    }

    if (pyramid->is_unfinished()) {
        textid reason = {178, 31}; // rising

        int workers_num = 0;
        for (auto &wid : d.workers) {
            workers_num += wid > 0 ? 1 : 0;
        }

        if (d.phase < 2) {
            // Clear / level — work camps
            int work_camps_num = g_city.buildings.count_total(BUILDING_WORK_CAMP);
            int work_camps_active_num = g_city.buildings.count_active(BUILDING_WORK_CAMP);
            if (!work_camps_num) {
                reason = {178, 13};
            } else if (!work_camps_active_num) {
                reason = {178, 17};
            } else if (is_true) {
                reason = {178, 30}; // amass stone + limestone
            } else {
                reason = {178, 36}; // amass plain stone (stepped)
            }
        } else if (is_true && d.phase >= polish_begin) {
            reason = {178, 32};
        } else {
            int carpenters_total = g_city.buildings.count_total(BUILDING_CARPENTERS_GUILD);
            int carpenters_active = g_city.buildings.count_active(BUILDING_CARPENTERS_GUILD);
            int masons_total = g_city.buildings.count_total(BUILDING_STONEMASONS_GUILD);
            int masons_active = g_city.buildings.count_active(BUILDING_STONEMASONS_GUILD);
            bool stone_stockpiled = g_city.resource.is_stockpiled(RESOURCE_STONE);
            bool lime_stockpiled = g_city.resource.is_stockpiled(RESOURCE_LIMESTONE);
            int stone_ready = city_resource_ready_for_using(RESOURCE_STONE);
            int lime_ready = city_resource_ready_for_using(RESOURCE_LIMESTONE);
            int timber_ready = city_resource_ready_for_using(RESOURCE_TIMBER);

            // True pyramid actually spawns stonemasons (need_stonemason override).
            // Stepped/bent do not — don't nag for a guild they never use.
            if (is_true && !masons_total) {
                reason = {178, 14};
            } else if (is_true && !masons_active) {
                reason = {178, 18};
            } else if (d.phase >= 3 && !carpenters_total) {
                reason = {178, 16};
            } else if (d.phase >= 3 && !carpenters_active) {
                reason = {178, 20};
            } else if (stone_stockpiled) {
                reason = {178, 98}; // stone stockpiled
            } else if (is_true && lime_stockpiled) {
                reason = {178, 99}; // limestone stockpiled
            } else if (pyramid->needs_resource(RESOURCE_STONE) > 0 && !stone_ready) {
                reason = {178, 22};
            } else if (is_true && pyramid->needs_resource(RESOURCE_LIMESTONE) > 0 && !lime_ready) {
                reason = {178, 23};
            } else if (pyramid->needs_resource(RESOURCE_TIMBER) > 0 && !timber_ready) {
                reason = {178, 28};
            } else if (is_true) {
                reason = {178, 31};
            } else {
                reason = {178, 37}; // stepped rising
            }
        }

        ui["warning_text"] = reason;

        int min_pct = 100;
        bool any_resource = false;
        for (int ri = (int)RESOURCES_MIN; ri < (int)RESOURCES_MAX; ++ri) {
            auto r = (e_resource)ri;
            if (pyramid->needs_resource(r) <= 0) {
                continue;
            }
            any_resource = true;
            min_pct = std::min(min_pct, (int)d.resources_pct[r]);
        }
        if (!any_resource) {
            min_pct = 100;
        }

        bstring64 progress_str;
        progress_str.printf("%d / %d    %d%%", (int)d.phase, pyramid->phases(), min_pct);
        ui["progress_text"] = progress_str;

        auto fill_resource_slot = [&] (e_resource r, pcstr icon_key, pcstr text_key) {
            int needed = pyramid->needs_resource(r);
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
        fill_resource_slot(RESOURCE_STONE, "stone_icon", "stone_text");
        fill_resource_slot(RESOURCE_LIMESTONE, "limestone_icon", "limestone_text");
        fill_resource_slot(RESOURCE_TIMBER, "timber_icon", "timber_text");

        bstring32 workers_str;
        workers_str.printf("%d / %d", workers_num, (int)d.workers.size());
        ui["workers_text"] = workers_str;
    } else {
        ui["warning_text"] = textid{178, 38}; // At last, the pyramid is complete!
        ui["progress_text"] = "";
        ui["stone_text"] = "";
        ui["limestone_text"] = "";
        ui["timber_text"] = "";
        ui["workers_text"] = "";
        ui["stone_icon"].set_enabled(false);
        ui["limestone_icon"].set_enabled(false);
        ui["timber_icon"].set_enabled(false);
    }
}
