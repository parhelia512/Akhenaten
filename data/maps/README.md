# Campaign mission maps

Exported from Pharaoh `mission1.pak` as `FILE_FORMAT_MAP_FILE`
(terrain, image, elevation, moisture, fertility, vegetation, scenario_info, …).

Naming matches `src/scripts/mission/m_NNN_*.js` (scenarios **0–18**).

Re-export: `GamestateIO::export_mission_map(id, path)` / `__test_export_mission_map`
(needs Cleop/Pharaoh install).

Not yet wired into campaign load — assets for future `map_file` in mission JS.
Deployed at build time to `<binary>/Data/maps` (MSVC) or `<binary>/data/maps`.
