#pragma once

#include "content/vfs.h"
#include "scenario/editor.h"

// ED4b/ED5: scenario logic for editor/custom maps lives in a JS sidecar, not the .map blob.
// Maps/foo.map ↔ Maps/foo.meta.js (requests[] + invasions[]).

vfs::path editor_map_meta_path(pcstr map_path);

// Snapshot current editor request slots + invasion schedule → sidecar.
// Removes the file when both tables are empty.
bool editor_map_meta_write(pcstr map_path);

// Load sidecar into editor request slots and g_scenario.invasions[].
// Clears invasions first; does not clear event_list (caller may clear_for_editor).
bool editor_map_meta_load(pcstr map_path);

void editor_map_meta_remove(pcstr map_path);

void editor_invasions_clear();

// Preserve / wipe / restore editor request slots around terrain-only map write.
void editor_requests_preserve_begin();
void editor_requests_preserve_end();
