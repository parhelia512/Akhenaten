#pragma once

#include "city_overlay.h"

struct city_overlay_hide_cliffs : public city_overlay_t<OVERLAY_HIDE_CLIFFS> {
    city_overlay_hide_cliffs() {
        es_name = "overlay_hide_cliffs";
        title_text = "#overlay_hide_cliffs";
    }

    virtual bool show_figure(const figure *f) const override;
    virtual bool draw_custom_footprint(vec2i pixel, tile2i point, painter &ctx) const override;
    virtual int get_column_height(const building *b) const override;
    virtual bool show_building(const building *b) const override;
};
