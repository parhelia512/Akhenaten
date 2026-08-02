#pragma once

#include "content/vfs.h"
#include "scenario/editor.h"

// ED4b: scenario logic for editor maps lives in a JS sidecar, not the .map blob.
// Maps/foo.map ↔ Maps/foo.meta.js (requests[] matching editor slots).

vfs::path editor_map_meta_path(pcstr map_path);

// Snapshot current editor request slots → sidecar. Empty table removes the file.
bool editor_map_meta_write(pcstr map_path);

// Load sidecar into editor request slots. Returns false if missing/unreadable.
// Caller should clear_for_editor() before load when stripping map-embedded events.
bool editor_map_meta_load(pcstr map_path);

void editor_map_meta_remove(pcstr map_path);

// Preserve / wipe / restore editor request slots around terrain-only map write.
void editor_requests_preserve_begin();
void editor_requests_preserve_end();
