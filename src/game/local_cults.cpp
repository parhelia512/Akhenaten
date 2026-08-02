#include "game/local_cults.h"

#include <cstring>

namespace {

const local_cult_static_t g_local_cult_registry[LOCAL_CULT_MAX] = {
    {LOCAL_CULT_NONE, "none", GOD_UNKNOWN, BUILDING_NONE, false},
    {LOCAL_CULT_ANUBIS, "anubis", GOD_SETH, BUILDING_TEMPLE_COMPLEX_ALTAR_ANUBIS, true},
    {LOCAL_CULT_THOTH, "thoth", GOD_OSIRIS, BUILDING_TEMPLE_COMPLEX_ORACLE_THOTH, true},
    {LOCAL_CULT_HATHOR, "hathor", GOD_BAST, BUILDING_TEMPLE_COMPLEX_ORACLE_HATHOR, true},
};

} // namespace

const local_cult_static_t &local_cult_static(e_local_cult id) {
    if (id <= LOCAL_CULT_NONE || id >= LOCAL_CULT_MAX) {
        return g_local_cult_registry[LOCAL_CULT_NONE];
    }
    return g_local_cult_registry[id];
}

e_local_cult local_cult_from_name(pcstr name) {
    if (!name || !*name) {
        return LOCAL_CULT_NONE;
    }
    for (int i = LOCAL_CULT_ANUBIS; i < LOCAL_CULT_MAX; ++i) {
        if (strcmp(g_local_cult_registry[i].name, name) == 0) {
            return (e_local_cult)i;
        }
    }
    return LOCAL_CULT_NONE;
}

pcstr local_cult_name(e_local_cult id) {
    return local_cult_static(id).name;
}
