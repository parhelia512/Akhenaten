#pragma once

#include "input/mouse.h"
#include "input/hotkey.h"

bool figure_follow_enabled();
int figure_follow_figure_id();
int figure_follow_texture_id();

bool figure_follow_start(int figure_id);
void figure_follow_stop(bool toast_lost = false);

void figure_follow_capture_if_due();
void figure_follow_draw_panel();
bool figure_follow_handle_mouse(const mouse *m);
bool figure_follow_handle_escape(const hotkeys *h);
