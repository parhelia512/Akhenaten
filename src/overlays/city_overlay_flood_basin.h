#pragma once

#include "city_overlay.h"

struct city_overlay_flood_basin : public city_overlay_t<OVERLAY_FLOOD_BASIN> {
    city_overlay_flood_basin() {
        es_name = "overlay_flood_basin";
        title_text = "#overlay_flood_basin";
    }

    virtual bool show_figure(const figure *f) const override;
    virtual void draw_custom_top(vec2i pixel, tile2i point, painter &ctx) const override;
    virtual int get_column_height(const building *b) const override;
    virtual bool show_building(const building *b) const override;
};
