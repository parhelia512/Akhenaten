# Akhenaten integral tests

JS-driven tests run by `--integraltests`, plus a small C++ smoke suite (`SDL_strlen`, `vec2i`, `bstring::cat`, `es_hash_str`, …) in `integral_tests.cpp` before the JS files run.

## Running

```bash
# Linux / macOS (adjust build dir if needed)
./build/akhenaten --integraltests --no-logo --no-resource --window --size 800x600

# Windows (Debug MSVC preset)
build\Debug\akhenaten.exe --integraltests --no-logo --no-resource --window --size 800x600
```

`--window --size 800x600` keeps `screen_width`/`screen_height` stable so input-simulation tests can rely on fixed pixel coordinates instead of recomputing them per machine.

### Running with Pharaoh resources (real image packs / data)

The suite normally runs hermetic (`--no-resource`). To exercise tests that need the real
game data (e.g. monument image packs like the sphinx `sphinx1a..6c` `.sg3`), **omit
`--no-resource`** and pass the Pharaoh data directory as the last positional argument. Add
`--nointro` so startup does not enter the intro video (which otherwise plays, and crashes
under `--nosound` in `video_init`):

```bash
# Windows, with resources (data dir last)
build\Debug\akhenaten.exe --integraltests --nointro --no-logo --window --size 800x600 "d:/Work/Cleop"
```

Screenshot-producing tests also want `--nomouse` (suppresses mouse-edge/drag camera
scroll so a programmatically centered view doesn't drift) and optionally
`--screenshot-dir PATH` (created if missing) to collect the PNGs:

```bash
build\Debug\akhenaten.exe --integraltests --integraltest-only 43_sphinx --nointro --nomouse --no-logo --window --size 800x600 --screenshot-dir out\shots "d:/Work/Cleop"
```

Both modes pass equally (use the resource mode to verify pack
loading / art resolution for data-dependent buildings).

Run a single file (substring match, case-insensitive):

```bash
build\Debug\akhenaten.exe --integraltests --integraltest-only 11 --no-logo --no-resource --window --size 800x600
```

The game starts normally (logo skipped, no Pharaoh data required), then iterates over `tests/*.js` in alphabetical order. Each test drives the game and inspects the resulting log file. Process exit code is `0` if every test passed, `1` otherwise.

Results are written to `akhenaten-log.txt` as lines like `[test:tests/11_work_camp_map_placement.js] PASS` or `FAIL: …`, plus a final `[integraltests] N passed, M failed` summary.

C++ smoke checks run first (before JS files): `SDL_strlen`/`strcmp`, `vec2i`, `get_version`, `bstring::cat`, `es_hash_str`, and `archive_helper::coerce`/`set`/`get` for `xstring` (and `vec2i`) property binding.

## Test files (current)

| File | What it checks |
|------|----------------|
| `01_main_menu.js` | Main menu shown marker |
| `02_main_menu_buttons.js` | Synthetic clicks on main-menu buttons |
| `03_mission_won_window.js` | `show_mission_won` console command → mission won window |
| `04_scenario_selection_guard.js` | Scenario selection guard |
| `05_scenario_selection_titles.js` | Scenario selection titles |
| `06_stonemason_guild_info_window.js` | Stonemason guild info window (`test_ensure_city_session` + `__test_building_create`) |
| `07_file_dialog_load_show.js` | Load savegame file dialog |
| `08_granary_info_window.js` | Granary info window |
| `09_ui_button_nonbool_flags.js` | UI button non-boolean flags regression |
| `10_work_camp_info_window.js` | Work camp info window |
| `11_work_camp_map_placement.js` | Work camp placed via `test_building_place` (real `city_planner` path) |
| `12_road_segment_placement.js` | Road via `build_planner` on a valid segment near map center; all routed tiles have `TERRAIN_ROAD` |
| `13_pottery_workshop_placement.js` | Pottery workshop via `test_building_place` |
| `14_hotkey_config_window.js` | Hotkey config window (`window_show_by_id`) |
| `15_victory_dialog_window.js` | `show_victory_dialog` console command → victory dialog |
| `16_papyrus_maker_placement.js` | Papyrus maker via `test_building_place` |
| `17_fishing_wharf_placement.js` | Fishing wharf on synthetic shoreline via `test_building_place` |
| `18_water_lift_placement.js` | Water lift on synthetic shoreline via `test_building_place` |
| `19_transport_wharf_placement.js` | Transport wharf on synthetic shoreline |
| `20_warship_wharf_placement.js` | Warship wharf on synthetic shoreline |
| `21_dock_placement.js` | Dock on synthetic shoreline |
| `22_shipyard_placement.js` | Shipyard on synthetic shoreline |
| `23_ferry_placement.js` | Ferry landing on synthetic shoreline |
| `24_brewery_placement.js` | Brewery via `test_building_place`; also reads `stored_resource(RESOURCE_WATER)` pseudo-resource |
| `33_clay_pit_placement.js` | Clay pit near synthetic water (`needs.nearby_water`) |
| `34_meadow_farm_placement.js` | Grain meadow farm on synthetic meadow; rejected on clear land |
| `35_floodplain_farm_placement.js` | Grain farm on synthetic floodplain via `test_farm_place` |
| `36_house_evolve_text_property.js` | House runtime `{}` property bindings: `evolve_text` xstring roundtrip, `worst_desirability_building_id` int (uint16) roundtrip, and the info-window fill path (`house_determine_*` helpers) without TypeError (H1) |
| `37_farm_preview_images.js` | Farm preview smoke: crops ≠ 0, get_image meadow/floodplain routing, `draw_from_below` |
| `38_color_mask_passing.js` | Full `COLOR_MASK_*` (> INT_MAX) survive MuJS→C++ as `color` (uint32): `__test_color_roundtrip` for GREEN/RED/BLUE + `draw_flat_tile` with a full mask without TypeError (J1) |
| `39_enemy_chariot_registered.js` | Every `FIGURE_ENEMY_*_CHARIOT` resolves to a registered enemy class (`__test_enemy_figure_registered`) instead of asserting; Assyrian/Hyksos chariots (missions 32/33) covered (F2) |
| `53_enemy_warship_registered.js` | Every `FIGURE_ENEMY_*_WAR_SHIP` (+ galera / generic 93) resolves to a registered enemy class (`__test_enemy_figure_registered`); E3a enemy fleet start |
| `40_hippo_spawn.js` | Spawn `FIGURE_HIPPO` on land and water; `update_animation` → `walk` / `swim` (#77) |
| `66_lion_spawn.js` | Cleopatra `FIGURE_LION`: spawn + type + `update_animation` → `walk` (SprMain2 group 5; CF3b) |
| `41_city_smoke_run.js` | Broad crash smoke (TS1): place ~12 building types via the real planner path, open each info window (`[es=(info_window_*, init)]`), advance the sim; driver's whole-log `!!! TypeError:` scan catches on_place/update/init crashes broadly. Per-type `smoke_ok:*` markers isolate the culprit; `smoke_skip:*` logged loudly |
| `46_bridge_placement.js` | Low Bridge on synthetic 3-wide channel: place (`WATER\|ROAD` + `bridge_part`/`bridge_type` + sprite dual-write), floodplain reject, max_length reject, citizen `CITIZEN_0_ROAD` |
| `47_js_hotreload_stack.js` | Hot-reload MuJS stack stability: 20× `js_register_game_handlers` + 20× reload `:console_commands.js` via `js_vm_sync` must not grow the value stack (former `[console_command=…]` getglobal leak → stackoverflow after ~10 mixed saves) |
| `42_enemy_config_valid.js` | Static validator (V1) for all 13 `enemy_*` configs in `enemies.js`: `percentage_type1+2+3 == 100`, a nonzero share has a non-NONE `figure_types[i]` (F1), and every declared figure type resolves to a registered enemy class via `__test_enemy_figure_registered` (F2) |
| `43_sphinx_place.js` | C6 Sphinx: planner-place `BUILDING_SPHINX`, assert 3 linked parts (`next_part_building_id`), open info window without TypeError |
| `44_obelisk_place.js` | C7 Small Obelisk: reject without granite / second unfinished; then staffed SY + 100 granite → place 3×3 (no parts), granite consumed, `__test_monument_add_resource` fills timber; display + full-city screenshots |
| `45_mastaba_place.js` | Small mastaba: planner-place `BUILDING_SMALL_MASTABA`, assert 10 linked parts, `image_stick` resolves (#431), bricks via `__test_monument_add_resource`, phase walk + screenshots |
| `50_invasion_bind_resolve.js` | Invasion bind: spawn + `on_completed_tag` deferred KR + history ring (JS calendar model; native EVENT_TYPE_INVASION cancelled) |
| `56_medium_stepped_first_stone.js` | Medium stepped first brick tier screenshots (companion/regress for large) |
| `57_request_cleared.js` | B13 `event_request_cleared`: ok fulfill, overdue fulfill → late, grace expire → refuse |
| `58_large_stepped_first_tier.js` | C1a first brick tier: place 20×20 large stepped, assert part layout (100 / 3 corners / 32 walls), walk phases →11, screenshot |
| `60_large_stepped_second_tier.js` | C1a second brick tier screenshots for large stepped pyramid |
| `61_large_stepped_third_tier.js` | C1a third brick tier screenshots for large stepped pyramid |
| `62_large_stepped_fourth_tier.js` | C1a fourth brick tier screenshots for large stepped pyramid |
| `63_large_stepped_fifth_tier.js` | C1a fifth brick tier screenshots for large stepped pyramid |
| `60_thinis_protect_nonrebuild.js` | Thinis (23): Osiris temple complex + mansions are map-only (not rebuildable); altar/oracle upgrades stay allowed |
| `59_autosave_slots.js` | Autosave slot filename format + pick missing/oldest (`__test_autosave_*`; AS4) |
| `64_trader_capacity.js` | Trade capacity: OG roll to 800 (`uint16` random), flag 1600, buy gate, `has_traded_max(cap)` (TC*) |
| `65_zoo_place.js` | Cleopatra Zoo: place 6×6, stock straw+gamemeat, spawn `FIGURE_ZOOKEEPER`, animals-present timer, no spawn without stock (Z5) |

Farm **placement** tests (34/35) cover `can_place` / terrain rules; **37** covers preview image helpers.
When adding more preview draw coverage, follow JS draw conventions in
[`src/building/CLAUDE.md`](../src/building/CLAUDE.md) (ghost_preview section) and
[`src/js/CLAUDE.md`](../src/js/CLAUDE.md).

## Canary tests (QA4)

Minimal regressions for a **bug class** — add one before merging a MuJS wave that could
reintroduce that class.

The integral driver (**HR5**):
- clears stale MuJS stack slots before each test (`cleared N stale MuJS stack slot(s)`);
- fails the test if the stack is not idle-empty afterward.

Hot-reload sync logs (**HR7**): `JS: vm_sync done files=N (…) refresh=full elapsed_ms=…`.

| Test | Bug class |
|------|-----------|
| `36_house_evolve_text_property.js` | xstring / house `{}` property binding (H1) |
| `38_color_mask_passing.js` | `color` / `COLOR_MASK_*` > INT_MAX (J1) |
| `47_js_hotreload_stack.js` | MuJS value-stack leak across hot-reload (HR1) |

Shared helpers for city tests and placement live in [`src/scripts/integral_test.js`](../src/scripts/integral_test.js), imported from `modules.js` (always loaded with the game VM, not via `include()` from test files).

## Writing a test

Each `tests/*.js` file (except names starting with `_`) must define exactly two global functions:

```js
function run_test() {
    // Drive the game: trigger UI actions, advance time, etc.
    // MUST eventually call __test_signal_ready(), either here or from
    // an async handler (e.g. [es=event_advance_day]).
    __test_signal_ready();
}

function check_valid() {
    // Search akhenaten-log.txt for a marker (C++ grep; log is not loaded into the VM).
    return __test_find_inlog('[test-marker] something');
}
```

If `__test_signal_ready()` is never called, the driver times out after ~10 seconds (600 frames at 60 fps) and the test is marked FAIL.

After each test finishes pumping frames, the driver scans `akhenaten-log.txt` for `!!! TypeError:` (MuJS runtime errors logged during the test). If that substring appears, the test fails even when `check_valid()` would return true.

**The scan reads the whole log, which is truncated per _process_, not per _test_.** So a single `!!! TypeError:` fails that test **and every test after it** in the same run (cascade). When many tests fail at once, isolate the first with `--integraltest-only N` — usually only it is the real failure. (This also means a broad smoke test like `41` benefits from the earlier tests being clean.)

After each test script loads, the driver calls `js_vm_sync({})` so any top-level `include()` in that file is flushed before `run_test()` runs.

## Naming convention

- `NN_short_name.js` — the `NN_` prefix orders the run sequence.
- Basenames starting with `_` are skipped by the driver (e.g. old shared snippets under `tests/`). Prefer adding helpers to `integral_test.js` instead.
- The name in log lines is the **full path** returned by the VFS iterator, e.g. `tests/11_work_camp_map_placement.js` → `[test:tests/11_work_camp_map_placement.js] PASS`.

## C++ test bindings (`js_test_game.cpp`)

| Function | Returns | Purpose |
|----------|---------|---------|
| `__test_find_inlog(marker)` | boolean | True if `marker` appears in `akhenaten-log.txt` |
| `__test_signal_ready()` | undefined | Stop frame pump; run `check_valid()` |
| `__test_pump_frames(n)` | undefined | Advance the game loop by `n` frames (capped at 240) |
| `__test_mouse_click(x, y)` | undefined | Synthetic left click at screen pixels |
| `__test_mouse_right_click(x, y)` | undefined | Synthetic right click |
| `__test_run_console_command(line)` | undefined | Run a debug-console command |
| `__log_info_native(msg)` | undefined | Log a diagnostic line |
| `__log_marker(tag)` | undefined | In `--integraltests`, logs `[test-marker] tag` (and plain `tag`); use from window `init` handlers |
| `__test_start_city_session(map_path)` | boolean | Load map and start city session |
| `__test_set_treasury(amount)` | undefined | Set treasury deben |
| `__test_process_events()` | undefined | Drain C++ event queue after `emit` (e.g. `event_city_building_mode`) |
| `__test_building_create(type, x, y)` | building id | Fast spawn without terrain checks; reuses first building of that type if present; center tile when `x` or `y` is negative |
| `__test_figure_create(type, x, y)` | figure id | Fast spawn via `figure_create` (no herd/formation); center tile when `x` or `y` is negative |
| `__test_figure_set_action(fid, action)` | undefined | Set figure `action_state` via `advance_action` |
| `__test_figure_update_animation(fid)` | undefined | Call `figure_impl::update_animation()` |
| `__figure_get_anim_key(fid)` | string | Current `animctx.key` (e.g. `walk`, `swim`) |
| `__test_show_tile_info(bid)` | undefined | Open building info window for `bid` |
| `__test_color_roundtrip(color)` | number | Echo a `color` (uint32) back through the binding conversion; asserts full `COLOR_MASK_*` survive MuJS→C++ (J1) |
| `__test_enemy_figure_registered(type)` | boolean | Spawn `type` and report whether it resolved to a registered enemy class (`dcast_enemy` != null); false for a missing `FIGURE_METAINFO` (F2) |
| `__test_monument_set_phase(bid, phase)` | undefined | Force monument (+ linked parts) construction phase |
| `__test_monument_add_resource(bid, resource, amount)` | boolean | Deliver resource units into monument (`deliver_resource`) |
| `__test_monument_resource_pct(bid, resource)` | int | Monument `resources_pct[resource]` (0..100+; −1 if invalid) |
| `__test_storage_yard_add_resource(bid, resource, amount)` | boolean | Force-stock a Storage Yard (bypass accept rules) |
| `__test_yards_stored(resource)` / `__test_yards_stored_staffed(resource)` | int | City granite/etc. in yards (all / staffed only) |
| `__test_building_current_image(bid)` | int | Monument `building_image_get()` |
| `__test_camera_center_building(bid)` | undefined | Center camera on building / monument `center_point` |
| `__test_js_hotreload_handlers_stack_ok(n)` | boolean | Re-run `js_register_game_handlers` `n` times; true if MuJS stack depth unchanged (HR1) |
| `__test_js_hotreload_file_stack_ok(path, n)` | boolean | `js_vm_reload_file` + `js_vm_sync` `n` times; true if stack depth unchanged (HR1) |
| `__test_trader_capacity(fid)` | int | Caravan runtime / ship effective max capacity (TC*) |
| `__test_trader_static_max_capacity(type)` / `__test_trader_static_capacity_random(type)` | int | Static caravan/ship params |
| `__test_trader_set_bought(fid, amount)` / `__test_trader_buy_under_capacity(fid)` | void / 0\|1 | Buy-capacity gate without warehouse |
| `__test_empire_trader_has_traded_max(bought, sold, capacity)` | 0\|1 | `has_traded_max(capacity)` |

## JS helpers (`integral_test.js`)

Loaded via `import integral_test` in `modules.js` (after `city_planner`).

| Function | Returns | Purpose |
|----------|---------|---------|
| `test_ensure_city_session(map_path)` | boolean | If `game.session_active`, no-op; else `__test_start_city_session` |
| `test_reload_city_session(map_path)` | boolean | Always `__test_start_city_session` (fresh map; use after tests that mutate the map) |
| `test_planner_enter_build_mode(type)` | boolean | Allow building, `emit event_city_building_mode`, drain events |
| `test_planner_exit_build_mode()` | undefined | Cancel construction, exit build mode |
| `test_find_buildable_tile(type)` | `{x,y}` or null | Scan map for a valid tile near center |
| `test_building_place(type, x, y)` | building id | Full placement via `city_planner`; auto-tile when `x` or `y` is negative; calls `test_log_building_placed` |
| `test_prepare_shoreline_patch(cx, cy, w, h)` | undefined | Paint water and rebuild shores (land row at `cy - 1`) |
| `test_prepare_bridge_channel(cx, cy, w, len)` | start tile | Paint a straight N–S water channel (width ≥ 3) for Low Bridge tests |
| `test_shoreline_building_place(type, size)` | building id | Reload not included; shoreline patch + `test_building_place` at map center |
| `test_assert_building_placed(bid, type, tag)` | boolean | Type, map tile, and per-bid marker checks |
| `test_staffed_yard_with_resource(resource, amount, x, y)` | building id | Place SY, set workers, force-stock resource |
| `test_log_building_placed(bid)` | undefined | `[test-marker] test_building_placed:…` (work camp uses `work_camp` suffix) |
| `test_figure_create(type, x, y)` | figure id | `__test_figure_create` + marker; auto-tile when `x`/`y` omitted or negative |
| `test_assert_figure_created(fid, type, tag)` | boolean | Type, validity, map occupancy, and marker checks |
| `test_log_figure_created(fid)` | undefined | `[test-marker] test_figure_created:type_<n>:<fid>:x,y` |

`city_planner` is an `ANK_GLOBAL_OBJECT` (`build_type`, `in_progress`, methods in [`city_planner.js`](../src/scripts/city_planner.js)).

### Building placement vs. quick create

- **`test_building_place`** — real placement path (`emit event_city_building_mode`, `city_planner.construction_*`, `validate_last_created`). Needs treasury/terrain; see `11_work_camp_map_placement.js` (`test_reload_city_session` + `__test_set_treasury`).
- **`__test_building_create`** — spawn for UI/info-window tests only (06/08/10); may ignore terrain and reuse an existing building of the same type.

### Other APIs used by tests

`game.session_active`, `__building_type`, `__building_tile`, `city.get_building_at`, `__scenario_map`, `__scenario_building_allow` / `__scenario_building_allowed`, `emit`, `window_*`, etc. — same VM as normal play.

## Window markers

There is no global window-init hook. Each observable window logs `__log_marker("window_show:<name>")` from its `[es=(<window>, init)]` handler in `src/scripts/ui_<window>.js`, for example:

- `ui_player_selection.js`, `ui_records_window.js`, `ui_mods_window.js`
- `ui_stonemason_guild_window.js`, `ui_granary_info.js`, `ui_work_camp_window.js`
- `ui_file_dialog_load.js`, `ui_mission_end_window.js`, `ui_victory_dialog.js`

When adding a test that asserts a window transition, add the matching `__log_marker` in that window's `init` handler so a broken script fails the test if `init` never runs.

## Constraints

- One process, sequential. Tests share global state (session, map, buildings). Later tests may need `test_reload_city_session` if an earlier test left buildings on the map (see 10 → 11).
- The log file is truncated on each process start. Under `--integraltests` it is written to `platform.user_directory()` (e.g. `%APPDATA%\Akhenaten` on Windows); `__test_find_inlog` reads `logs::output_path()`.
- The driver expects `--no-logo --no-resource` for a hermetic run; CI sets `SDL_VIDEODRIVER=dummy` for headless runs.
