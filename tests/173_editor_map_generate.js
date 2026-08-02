// Editor procedural map generation (CaesarIA midpoint-displacement port).
// Markers:
//   [test-marker] editor_map_generate_ok

function run_test() {
    __log_info_native('[test:173] editor map generate')

    if (!game.init_editor()) {
        __log_info_native('[test:173] init_editor failed')
        __test_signal_ready()
        return
    }

    if (!__test_editor_map_generate()) {
        __log_info_native('[test:173] editor_map_generate failed')
        __test_signal_ready()
        return
    }

    __log_marker('editor_map_generate_ok')
    __test_signal_ready()
}

function check_valid() {
    return !!__test_find_inlog('[test-marker] editor_map_generate_ok')
}
