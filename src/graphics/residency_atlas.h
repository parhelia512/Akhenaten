#pragma once

#include "core/vec2i.h"

struct SDL_Texture;

// Add-only "residency atlas" — an observational debug tool.
//
// Every distinct source-atlas sprite region that the game actually draws is
// copied once into a single large GPU texture and never evicted. This lets us
// measure how full one consolidated atlas would get for the real working set
// (the prerequisite for collapsing the depth-sorted draw stream into a handful
// of batched draw calls). It does NOT change how the game renders — it only
// records + mirrors what is drawn.
namespace res_atlas {

// Called from painter::draw_impl for every sprite draw. Main-thread only
// (render-command replay is sequential), so no locking. Cheap no-op when
// disabled. `offset`/`size` describe the source sub-rect inside `src`.
void on_draw(SDL_Texture *src, vec2i offset, vec2i size);

void enable(int size);   // create a size x size atlas and start recording
void disable();          // stop recording + preview (keeps collected data)
void reset();            // drop all collected data and free the texture
void set_preview(bool on);
bool enabled();

// Registered as a frame_pre_present handler: blit newly-seen regions into the
// atlas, then draw the on-screen preview. Runs after all city/UI drawing while
// the render target is still the offscreen render texture.
void frame_pre_present();

void log_stats();
void dump(const char *filename); // save atlas to PNG + log stats

} // namespace res_atlas
