#pragma once

#include "core/bstring.h"

class Arguments;

/** Show configuration window to override parameters of the startup.
 */
void show_options_window(Arguments& args);

/** Load akhenaten.conf from data/user/cwd into game_features (before main JS VM).
 *  Returns true if a settings file was loaded.
 */
bool try_load_game_features(pcstr data_directory);

/** Save current game_features to akhenaten.conf under data/user/cwd.
 */
void try_save_game_features(pcstr data_directory);
