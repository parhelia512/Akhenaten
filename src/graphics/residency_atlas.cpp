#include "graphics/residency_atlas.h"

#include "platform/renderer.h"
#include "game/game.h"
#include "content/file_formats.h"
#include "core/log.h"
#include "dev/debug.h"

#include <SDL.h>

#include <algorithm>
#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

// The whole tool is a debug aid; keep it out of the Android build where the
// console system itself is stubbed out.
namespace res_atlas {

namespace {

constexpr int DEFAULT_SIZE = 8192;
constexpr int PADDING = 1;       // guard band to avoid bilinear bleed between neighbours
constexpr int PREVIEW_MAX_DIM = 512;

struct key_t {
    SDL_Texture *tex;
    int ox, oy, w, h;
    bool operator==(const key_t &o) const {
        return tex == o.tex && ox == o.ox && oy == o.oy && w == o.w && h == o.h;
    }
};

struct key_hash {
    size_t operator()(const key_t &k) const {
        size_t h = std::hash<const void *>()(k.tex);
        auto mix = [&](int v) { h ^= (size_t)(uint32_t)v * 0x9e3779b1u + (h << 6) + (h >> 2); };
        mix(k.ox);
        mix(k.oy);
        mix(k.w);
        mix(k.h);
        return h;
    }
};

struct blit_t {
    SDL_Texture *src;
    SDL_Rect srcrect;
    SDL_Rect dstrect;
};

// Skyline (bottom-left) packer node: a horizontal run at height `y`.
struct skyline_node {
    int x, y, width;
};

bool g_enabled = false;
bool g_preview = true;
bool g_handler_registered = false;

SDL_Texture *g_atlas = nullptr;
int g_atlas_w = 0;
int g_atlas_h = 0;

std::unordered_set<key_t, key_hash> g_seen;
std::unordered_set<const void *> g_src_textures;
std::vector<blit_t> g_pending;
std::vector<skyline_node> g_skyline;

int g_distinct = 0;
int64_t g_used_area = 0;
int g_overflow = 0;
int64_t g_overflow_area = 0;

// Can a rect of (w,h) sit on the skyline starting at node `index`? If so, the
// baseline y where it would rest is written to out_y.
bool skyline_fit(int index, int w, int h, int &out_y) {
    const int x = g_skyline[index].x;
    if (x + w > g_atlas_w) {
        return false;
    }
    int width_left = w;
    int i = index;
    int y = 0;
    while (width_left > 0) {
        if (i >= (int)g_skyline.size()) {
            return false;
        }
        y = std::max(y, g_skyline[i].y);
        if (y + h > g_atlas_h) {
            return false;
        }
        width_left -= g_skyline[i].width;
        ++i;
    }
    out_y = y;
    return true;
}

// Insert a placed rect into the skyline and re-merge.
void skyline_add(int index, int x, int y, int w, int h) {
    g_skyline.insert(g_skyline.begin() + index, skyline_node{x, y + h, w});

    // Trim/erase nodes overlapped horizontally by the newly inserted run.
    for (size_t i = index + 1; i < g_skyline.size();) {
        skyline_node &cur = g_skyline[i];
        const skyline_node &prev = g_skyline[i - 1];
        if (cur.x < prev.x + prev.width) {
            const int shrink = prev.x + prev.width - cur.x;
            cur.x += shrink;
            cur.width -= shrink;
            if (cur.width <= 0) {
                g_skyline.erase(g_skyline.begin() + i);
                continue;
            }
        }
        break;
    }

    // Merge adjacent runs that ended up at the same height.
    for (size_t i = 0; i + 1 < g_skyline.size();) {
        if (g_skyline[i].y == g_skyline[i + 1].y) {
            g_skyline[i].width += g_skyline[i + 1].width;
            g_skyline.erase(g_skyline.begin() + i + 1);
        } else {
            ++i;
        }
    }
}

// Add-only allocation of a (w,h) cell. Returns top-left on success.
bool pack(int w, int h, int &rx, int &ry) {
    int best_bottom = INT32_MAX;
    int best_width = INT32_MAX;
    int best_index = -1;
    int best_x = 0, best_y = 0;
    for (int i = 0; i < (int)g_skyline.size(); ++i) {
        int y = 0;
        if (skyline_fit(i, w, h, y)) {
            if (y + h < best_bottom || (y + h == best_bottom && g_skyline[i].width < best_width)) {
                best_bottom = y + h;
                best_width = g_skyline[i].width;
                best_index = i;
                best_x = g_skyline[i].x;
                best_y = y;
            }
        }
    }
    if (best_index < 0) {
        return false;
    }
    skyline_add(best_index, best_x, best_y, w, h);
    rx = best_x;
    ry = best_y;
    return true;
}

int skyline_peak() {
    int peak = 0;
    for (const skyline_node &n : g_skyline) {
        peak = std::max(peak, n.y);
    }
    return peak;
}

void clear_atlas() {
    SDL_Renderer *r = g_render.renderer();
    SDL_Texture *prev = SDL_GetRenderTarget(r);
    SDL_Rect vp;
    SDL_RenderGetViewport(r, &vp);
    SDL_SetRenderTarget(r, g_atlas);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
    SDL_RenderClear(r);
    SDL_SetRenderTarget(r, prev);
    SDL_RenderSetViewport(r, &vp);
}

void flush_pending() {
    if (g_pending.empty()) {
        return;
    }
    SDL_Renderer *r = g_render.renderer();
    SDL_Texture *prev = SDL_GetRenderTarget(r);
    SDL_Rect prev_vp;
    SDL_RenderGetViewport(r, &prev_vp);
    const SDL_bool had_clip = SDL_RenderIsClipEnabled(r);
    SDL_Rect prev_clip{};
    if (had_clip) {
        SDL_RenderGetClipRect(r, &prev_clip);
    }
    SDL_RenderSetClipRect(r, nullptr);
    SDL_SetRenderTarget(r, g_atlas);
    SDL_RenderSetViewport(r, nullptr); // dst rects are in atlas space, not the prior target's viewport

    for (const blit_t &b : g_pending) {
        SDL_BlendMode old_mode = SDL_BLENDMODE_BLEND;
        SDL_GetTextureBlendMode(b.src, &old_mode);
        // Straight copy so source alpha lands verbatim in the atlas.
        SDL_SetTextureBlendMode(b.src, SDL_BLENDMODE_NONE);
        SDL_SetTextureColorMod(b.src, 255, 255, 255);
        SDL_SetTextureAlphaMod(b.src, 255);
        SDL_RenderCopy(r, b.src, &b.srcrect, &b.dstrect);
        SDL_SetTextureBlendMode(b.src, old_mode);
    }
    g_pending.clear();

    SDL_SetRenderTarget(r, prev);
    SDL_RenderSetViewport(r, &prev_vp);
    SDL_RenderSetClipRect(r, had_clip ? &prev_clip : nullptr);
}

void draw_preview() {
    SDL_Renderer *r = g_render.renderer();
    const SDL_bool had_clip = SDL_RenderIsClipEnabled(r);
    SDL_Rect prev_clip{};
    if (had_clip) {
        SDL_RenderGetClipRect(r, &prev_clip);
    }
    SDL_Rect prev_vp;
    SDL_RenderGetViewport(r, &prev_vp);
    SDL_RenderSetClipRect(r, nullptr);
    SDL_RenderSetViewport(r, nullptr); // full render target

    const float scale = (float)PREVIEW_MAX_DIM / (float)std::max(g_atlas_w, g_atlas_h);
    const int margin = 10;
    SDL_Rect dst{margin, margin, (int)(g_atlas_w * scale), (int)(g_atlas_h * scale)};
    SDL_Rect frame{dst.x - 2, dst.y - 2, dst.w + 4, dst.h + 4};

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 210);
    SDL_RenderFillRect(r, &frame);

    SDL_SetTextureBlendMode(g_atlas, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(g_atlas, 255, 255, 255);
    SDL_SetTextureAlphaMod(g_atlas, 255);
    SDL_RenderCopy(r, g_atlas, nullptr, &dst);

    // Skyline peak marker (how tall the packing has grown).
    const int peak_y = dst.y + (int)(skyline_peak() * scale);
    SDL_SetRenderDrawColor(r, 255, 80, 80, 255);
    SDL_RenderDrawLine(r, dst.x, peak_y, dst.x + dst.w, peak_y);

    SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
    SDL_RenderDrawRect(r, &frame);

    SDL_RenderSetViewport(r, &prev_vp);
    SDL_RenderSetClipRect(r, had_clip ? &prev_clip : nullptr);
}

} // namespace

void on_draw(SDL_Texture *src, vec2i offset, vec2i size) {
    if (!g_enabled || src == nullptr || src == g_atlas) {
        return;
    }
    if (offset.x < 0 || offset.y < 0 || size.x <= 0 || size.y <= 0) {
        return;
    }
    const key_t k{src, offset.x, offset.y, size.x, size.y};
    if (!g_seen.insert(k).second) {
        return; // already recorded
    }
    ++g_distinct;
    g_src_textures.insert(src);

    int rx = 0, ry = 0;
    if (pack(size.x + PADDING, size.y + PADDING, rx, ry)) {
        g_used_area += (int64_t)size.x * size.y;
        g_pending.push_back(blit_t{src, {offset.x, offset.y, size.x, size.y}, {rx, ry, size.x, size.y}});
    } else {
        ++g_overflow;
        g_overflow_area += (int64_t)size.x * size.y;
    }
}

void enable(int size) {
    if (size <= 0) {
        size = DEFAULT_SIZE;
    }
    const vec2i maxsz = g_render.get_max_image_size();
    if (maxsz.x > 0) {
        size = std::min(size, maxsz.x);
    }
    if (maxsz.y > 0) {
        size = std::min(size, maxsz.y);
    }

    if (g_atlas && g_atlas_w != size) {
        reset(); // requested a different size -> start over
    }

    if (!g_atlas) {
        SDL_Renderer *r = g_render.renderer();
        g_atlas = SDL_CreateTexture(r, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, size, size);
        if (!g_atlas) {
            logs::error("residency_atlas: failed to create %dx%d target texture: %s", size, size, SDL_GetError());
            return;
        }
        SDL_SetTextureBlendMode(g_atlas, SDL_BLENDMODE_BLEND);
        g_atlas_w = g_atlas_h = size;
        g_skyline.clear();
        g_skyline.push_back(skyline_node{0, 0, size});
        clear_atlas();
    }

    g_enabled = true;
    if (!g_handler_registered) {
        game.add_frame_pre_present_handler([]() { res_atlas::frame_pre_present(); });
        g_handler_registered = true;
    }
    logs::info("residency_atlas: enabled (%dx%d)", g_atlas_w, g_atlas_h);
}

void disable() {
    g_enabled = false;
    logs::info("residency_atlas: recording stopped (data kept)");
}

void reset() {
    if (g_atlas) {
        SDL_DestroyTexture(g_atlas);
        g_atlas = nullptr;
    }
    g_atlas_w = g_atlas_h = 0;
    g_seen.clear();
    g_src_textures.clear();
    g_pending.clear();
    g_skyline.clear();
    g_distinct = 0;
    g_used_area = 0;
    g_overflow = 0;
    g_overflow_area = 0;
}

void set_preview(bool on) {
    g_preview = on;
}

bool enabled() {
    return g_enabled;
}

void frame_pre_present() {
    if (!g_atlas) {
        return;
    }
    flush_pending();
    if (g_enabled && g_preview) {
        draw_preview();
    }
}

void log_stats() {
    if (!g_atlas) {
        logs::info("residency_atlas: not enabled");
        return;
    }
    const int peak = skyline_peak();
    const int64_t total = (int64_t)g_atlas_w * g_atlas_h;
    const int64_t bbox = (int64_t)g_atlas_w * peak;
    const double coverage = 100.0 * (double)g_used_area / (double)total;
    const double peak_pct = 100.0 * (double)peak / (double)g_atlas_h;
    const double pack_eff = bbox > 0 ? 100.0 * (double)g_used_area / (double)bbox : 0.0;
    logs::info("residency_atlas %dx%d: distinct=%d from %d source textures | placed area=%lld px (%.2f%% of atlas) | "
               "skyline peak=%d px (%.1f%% height, %.1f%% packing efficiency) | overflow=%d imgs / %lld px",
               g_atlas_w, g_atlas_h, g_distinct, (int)g_src_textures.size(), (long long)g_used_area, coverage, peak,
               peak_pct, pack_eff, g_overflow, (long long)g_overflow_area);
}

void dump(const char *filename) {
    if (!g_atlas) {
        logs::info("residency_atlas: nothing to dump");
        return;
    }
    const bool ok = g_render.save_texture_to_file(filename, g_atlas, FILE_FORMAT_PNG);
    logs::info("residency_atlas: dumped '%s' -> %s", filename, ok ? "ok" : "FAILED");
    log_stats();
}

} // namespace res_atlas

declare_console_command_p(residency_atlas) {
    std::string sub;
    is >> sub;
    if (sub == "on") {
        int size = 0;
        is >> size;
        res_atlas::enable(size);
        os << "residency_atlas: recording on" << std::endl;
    } else if (sub == "off") {
        res_atlas::disable();
        os << "residency_atlas: recording off" << std::endl;
    } else if (sub == "reset") {
        res_atlas::reset();
        os << "residency_atlas: reset" << std::endl;
    } else if (sub == "preview") {
        std::string v;
        is >> v;
        const bool on = (v != "off");
        res_atlas::set_preview(on);
        os << "residency_atlas: preview " << (on ? "on" : "off") << std::endl;
    } else if (sub == "stats") {
        res_atlas::log_stats();
        os << "residency_atlas: stats written to log" << std::endl;
    } else if (sub == "dump") {
        std::string fn;
        is >> fn;
        if (fn.empty()) {
            fn = "residency_atlas.png";
        }
        res_atlas::dump(fn.c_str());
        os << "residency_atlas: dumped to " << fn << std::endl;
    } else {
        os << "usage: residency_atlas on [size] | off | preview [on|off] | stats | dump [file] | reset" << std::endl;
    }
}
