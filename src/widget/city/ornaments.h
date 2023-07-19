#pragma once

#include "graphics/color.h"
#include "graphics/view/view.h"

int get_farm_image(int grid_offset);
void draw_farm_crops(int type, int progress, int grid_offset, int x, int y, color_t color_mask);

void draw_ornaments_and_animations(pixel_coordinate pixel, map_point point);