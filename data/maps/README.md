# Campaign mission maps

Exported from Pharaoh `mission1.pak` as `FILE_FORMAT_MAP_FILE`
(terrain, image, elevation, moisture, fertility, vegetation, scenario_info, …).

Naming matches `src/scripts/mission/m_NNN_*.js` (scenarios **0–24** so far).

Each mission sets `map_file : "data/maps/m_NNN_….map"`. `GamestateIO::load_mission`
loads that map first (`e_session_mission`), then falls back to `mission1.pak` if
the file is missing.

Re-export: `GamestateIO::export_mission_map(id, path)` / `__test_export_mission_map`
(needs Cleop/Pharaoh install).

Deployed at build time to `<binary>/Data/maps` (MSVC) or `<binary>/data/maps`.
