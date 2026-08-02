#include "file_dialog_save.h"

#include "window/autoconfig_window.h"
#include "js/js_game.h"
#include "core/profiler.h"

void window_file_dialog_save_show() {
    autoconfig_window::show("file_dialog_save");
}
ANK_FUNCTION(window_file_dialog_save_show)

void window_file_dialog_save_scenario_show() {
    autoconfig_window::show("file_dialog_save_scenario");
}
ANK_FUNCTION(window_file_dialog_save_scenario_show)
