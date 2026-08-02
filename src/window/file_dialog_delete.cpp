#include "file_dialog_delete.h"

#include "window/autoconfig_window.h"
#include "js/js_game.h"
#include "core/profiler.h"

void window_file_dialog_delete_show() {
    autoconfig_window::show("file_dialog_delete");
}
ANK_FUNCTION(window_file_dialog_delete_show)
