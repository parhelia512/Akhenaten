log_info("akhenaten: ui editor started")

[es=event_load_city]
function editor_on_load_city(ev) {
    if (!game.editor_is_active())
        return
    window_file_dialog_load_show(FILE_TYPE_SCENARIO)
}

[es=event_save_city]
function editor_on_save_city(ev) {
    if (!game.editor_is_active())
        return
    window_file_dialog_save_show(FILE_TYPE_SCENARIO)
}
