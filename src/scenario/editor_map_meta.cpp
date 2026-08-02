#include "editor_map_meta.h"

#include "content/vfs.h"
#include "core/bstring.h"
#include "game/resource.h"
#include "scenario/scenario.h"
#include "scenario/scenario_event_manager.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <string>

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

namespace {

constexpr pcstr META_HEADER = "// akhenaten-editor-map-meta v1\n";

struct request_slot_snap {
    editor_request req{};
    bool filled = false;
};

request_slot_snap g_preserve[event_manager_t::editor_request_slots];
bool g_preserve_active = false;

bstring64 resource_js_token(e_resource r) {
    bstring64 token("RESOURCE_");
    for (pcstr p = resource_name(r); p && *p; ++p) {
        char c[2] = {(char)std::toupper((unsigned char)*p), 0};
        token.cat(c);
    }
    return token;
}

e_resource parse_resource_token(pcstr tok) {
    if (!tok || !tok[0]) {
        return RESOURCE_NONE;
    }
    if (std::isdigit((unsigned char)tok[0]) || tok[0] == '-') {
        int v = std::atoi(tok);
        if (v < 0 || v >= RESOURCE_COUNT) {
            return RESOURCE_NONE;
        }
        return (e_resource)v;
    }
    pcstr name = tok;
    if (strncasecmp(tok, "RESOURCE_", 9) == 0) {
        name = tok + 9;
    }
    bstring64 lower;
    for (pcstr p = name; *p; ++p) {
        char c[2] = {(char)std::tolower((unsigned char)*p), 0};
        lower.cat(c);
    }
    return resource_type(xstring(lower.c_str()));
}

void snapshot_slots(request_slot_snap *slots) {
    for (int i = 0; i < event_manager_t::editor_request_slots; ++i) {
        slots[i].filled = false;
        memset(&slots[i].req, 0, sizeof(slots[i].req));
        g_scenario.events.editor_request_get(i, &slots[i].req);
        if (slots[i].req.resource != RESOURCE_NONE && slots[i].req.amount > 0) {
            slots[i].filled = true;
        }
    }
}

void restore_slots(const request_slot_snap *slots) {
    g_scenario.events.clear_for_editor();
    for (int i = 0; i < event_manager_t::editor_request_slots; ++i) {
        if (slots[i].filled) {
            g_scenario.events.editor_request_save(i, &slots[i].req);
        }
    }
}

bool read_int_field(pcstr block, pcstr key, int *out) {
    bstring64 needle(key, ":");
    pcstr p = std::strstr(block, needle.c_str());
    if (!p) {
        return false;
    }
    p += needle.len();
    while (*p && std::isspace((unsigned char)*p)) {
        ++p;
    }
    if (!*p) {
        return false;
    }
    *out = std::atoi(p);
    return true;
}

bool read_resource_field(pcstr block, e_resource *out) {
    pcstr p = std::strstr(block, "resource:");
    if (!p) {
        return false;
    }
    p += 9;
    while (*p && std::isspace((unsigned char)*p)) {
        ++p;
    }
    char tok[64] = {0};
    int n = 0;
    while (*p && !std::isspace((unsigned char)*p) && *p != ',' && *p != '}' && n < 63) {
        tok[n++] = *p++;
    }
    tok[n] = 0;
    *out = parse_resource_token(tok);
    return true;
}

} // namespace

vfs::path editor_map_meta_path(pcstr map_path) {
    bstring256 path(map_path ? map_path : "");
    char *dot = std::strrchr(path.data(), '.');
    if (dot && strcasecmp(dot, ".map") == 0) {
        std::snprintf(dot, (size_t)(bstring256::capacity - (dot - path.data())), ".meta.js");
    } else {
        path.cat(".meta.js");
    }
    // Same resolve as GamestateIO map write — relative Maps/… → base-path file.
    vfs::path meta(path.c_str());
    vfs::path resolved = meta.resolve();
    return resolved.empty() ? meta : resolved;
}

void editor_requests_preserve_begin() {
    snapshot_slots(g_preserve);
    g_preserve_active = true;
    g_scenario.events.clear_for_editor();
}

void editor_requests_preserve_end() {
    if (!g_preserve_active) {
        return;
    }
    restore_slots(g_preserve);
    g_preserve_active = false;
}

void editor_map_meta_remove(pcstr map_path) {
    vfs::path meta = editor_map_meta_path(map_path);
    if (vfs::file_exists(meta)) {
        vfs::file_remove(meta.c_str());
    }
}

bool editor_map_meta_write(pcstr map_path) {
    if (!map_path || !map_path[0]) {
        return false;
    }

    request_slot_snap slots[event_manager_t::editor_request_slots];
    snapshot_slots(slots);

    int filled = 0;
    for (int i = 0; i < event_manager_t::editor_request_slots; ++i) {
        filled += slots[i].filled ? 1 : 0;
    }

    if (filled == 0) {
        editor_map_meta_remove(map_path);
        return true;
    }

    vfs::path meta = editor_map_meta_path(map_path);
    FILE *fp = vfs::file_open_os(meta.c_str(), "wb");
    if (!fp) {
        // Ensure Maps/ (or parent) exists — map write may have created it already.
        vfs::create_folders("Maps");
        fp = vfs::file_open_os(meta.c_str(), "wb");
    }
    if (!fp) {
        return false;
    }

    std::fputs(META_HEADER, fp);
    std::fputs("// Editor / custom-map SoT for requests (not stored in .map).\n", fp);
    std::fputs("// Copy requests […] into mission JS, or keep as sidecar next to the map.\n", fp);
    std::fputs("editor_map_meta {\n", fp);
    std::fputs("\trequests [\n", fp);
    for (int i = 0; i < event_manager_t::editor_request_slots; ++i) {
        if (!slots[i].filled) {
            continue;
        }
        const editor_request &r = slots[i].req;
        bstring64 res_tok = resource_js_token(r.resource);
        std::fprintf(fp,
                     "\t\t{ slot: %d, year: %d, resource: %s, amount: %d, deadline_years: %d, kingdom: %d }\n",
                     i,
                     r.year,
                     res_tok.c_str(),
                     r.amount,
                     r.deadline_years,
                     r.kingdom);
    }
    std::fputs("\t]\n}\n", fp);
    std::fclose(fp);
    return true;
}

bool editor_map_meta_load(pcstr map_path) {
    if (!map_path || !map_path[0]) {
        return false;
    }

    vfs::path meta = editor_map_meta_path(map_path);
    if (!vfs::file_exists(meta)) {
        return false;
    }

    FILE *fp = vfs::file_open_os(meta.c_str(), "rb");
    if (!fp) {
        return false;
    }

    std::fseek(fp, 0, SEEK_END);
    long sz = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    if (sz <= 0 || sz > 256 * 1024) {
        std::fclose(fp);
        return false;
    }

    std::string text((size_t)sz, '\0');
    size_t nread = std::fread(text.data(), 1, (size_t)sz, fp);
    std::fclose(fp);
    text.resize(nread);

    bool any = false;
    pcstr p = text.c_str();
    while ((p = std::strstr(p, "{")) != nullptr) {
        pcstr end = std::strstr(p, "}");
        if (!end) {
            break;
        }

        std::string block(p, end - p + 1);
        int slot = -1;
        int year = 0;
        int amount = 0;
        int deadline_years = 1;
        int kingdom = 0;
        e_resource resource = RESOURCE_NONE;

        if (!read_int_field(block.c_str(), "slot", &slot)) {
            p = end + 1;
            continue;
        }
        read_int_field(block.c_str(), "year", &year);
        read_int_field(block.c_str(), "amount", &amount);
        read_int_field(block.c_str(), "deadline_years", &deadline_years);
        read_int_field(block.c_str(), "kingdom", &kingdom);
        read_resource_field(block.c_str(), &resource);

        if (slot >= 0 && slot < event_manager_t::editor_request_slots && resource != RESOURCE_NONE && amount > 0) {
            editor_request r{};
            r.year = year;
            r.resource = resource;
            r.amount = amount;
            r.deadline_years = deadline_years;
            r.kingdom = kingdom;
            g_scenario.events.editor_request_save(slot, &r);
            any = true;
        }
        p = end + 1;
    }

    return any;
}
