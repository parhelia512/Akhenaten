// UB1/BR1 canary: editor Bridge/Ferry allow flags from pak + Cleop maps.
// Under --no-resource → skip (PASS). Needs Pharaoh/Cleopatra data dir.

function run_test() {
    __test_mission_map_bridge_allow_dump(129, "Maps/Bridges.map")
    __test_mission_map_bridge_allow_dump(128, "Maps/Alexandria.map")
    __test_mission_bridge_allow_dump(0)
    __test_mission_bridge_allow_dump(2)
    __test_mission_bridge_allow_dump(3)
    // load_*_raw calls pre_load() when resources are present — restore a normal
    // city session so later tests are not left on Bridges/Alexandria/Nekhen.
    // Harmless under --no-resource (dumps skip without pre_load).
    test_reload_city_session('data/default.map')
    __test_signal_ready()
}

function check_valid() {
    if (__test_find_inlog("bridge_allow_skipped:no_resource")) {
        return true
    }
    if (__test_find_inlog("bridge_allow_fail:129") || __test_find_inlog("bridge_allow_fail:128")) {
        return false
    }
    // Bridges.map / Alexandria: Bridge ON (slot 28).
    if (!__test_find_inlog("bridge_allow:id=129|src=Maps/Bridges.map|bridge=1|")) {
        return false
    }
    if (!__test_find_inlog("bridge_allow:id=128|src=Maps/Alexandria.map|bridge=1|")) {
        return false
    }
    // Nubt: Bridge OFF.
    if (!__test_find_inlog("bridge_allow:id=0|src=pak|bridge=0|")) {
        return false
    }
    // Perwadjyt: pak Bridge OFF (JS tutorial unlock = REMAP).
    if (!__test_find_inlog("bridge_allow:id=2|src=pak|bridge=0|")) {
        return false
    }
    // Nekhen: Bridge ON.
    if (!__test_find_inlog("bridge_allow:id=3|src=pak|bridge=1|")) {
        return false
    }
    return true
}
