#pragma once

// Fill the current blank editor map with CaesarIA-style procedural terrain.
// Call after create_blank_map / clear_map_data and before prepare_map_for_editing.
void editor_map_generate(float smooth = 2.6f, float terrain_sq = 5.0f);
