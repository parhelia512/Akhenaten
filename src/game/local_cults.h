#pragma once

#include <cstdint>

#include "building/building_type.h"
#include "core/bstring.h"
#include "core/tokenum.h"
#include "game/gods.h"

enum e_local_cult : uint8_t {
    LOCAL_CULT_NONE = 0,
    LOCAL_CULT_ANUBIS = 1,
    LOCAL_CULT_THOTH = 2,
    LOCAL_CULT_HATHOR = 3,
    LOCAL_CULT_MAX
};

enum e_festival_theme : uint8_t {
    FESTIVAL_THEME_NONE = 0,
    FESTIVAL_THEME_HARVEST = 1,
    FESTIVAL_THEME_WAR = 2,
    FESTIVAL_THEME_CRAFT = 3,
    FESTIVAL_THEME_MAX
};

struct local_cult_static_t {
    e_local_cult id;
    pcstr name;
    e_god host_major_god;
    e_building_type building_type;
    bool festival_ok;
};

inline constexpr int LOCAL_CULT_REGISTRY_COUNT = LOCAL_CULT_MAX - 1;

const local_cult_static_t &local_cult_static(e_local_cult id);
e_local_cult local_cult_from_name(pcstr name);
pcstr local_cult_name(e_local_cult id);
