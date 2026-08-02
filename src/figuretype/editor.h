#pragma once

#include "figure/figure.h"
#include "figure/figure_impl.h"

enum {
    MAP_FLAG_EARTHQUAKE = 1,
    MAP_FLAG_ENTRY = 2,
    MAP_FLAG_EXIT = 3,
    MAP_FLAG_RIVER_ENTRY = 12,
    MAP_FLAG_RIVER_EXIT = 13,
    MAP_FLAG_INVASION_MIN = 4,
    MAP_FLAG_INVASION_MAX = 12,
    MAP_FLAG_FISHING_MIN = 14,
    MAP_FLAG_FISHING_MAX = 22,
    MAP_FLAG_HERD_MIN = 22,
    MAP_FLAG_HERD_MAX = 26,

    MAP_FLAG_MIN = 1,
    MAP_FLAG_MAX = 26,
};

class figure_map_flag : public figure_impl {
public:
    FIGURE_METAINFO(FIGURE_MAP_FLAG, figure_map_flag)
    figure_map_flag(figure *f) : figure_impl(f) {}

    virtual void figure_action() override;
};

void figure_create_editor_flags(void);
