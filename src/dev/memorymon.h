#pragma once

#include "platform/platform.h"

#ifndef GAME_PLATFORM_ANDROID

void game_memorymon_overlay_init();
void game_memorymon_draw();

#else

inline void game_memorymon_overlay_init() {}
inline void game_memorymon_draw() {}

#endif
