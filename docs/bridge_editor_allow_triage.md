# Editor Bridge / Ferry allow flags (UB1 / BR1)

Dump helpers: `__test_mission_bridge_allow_dump(id)`,
`__test_mission_map_bridge_allow_dump(id, path)`.

| Slot | Lang 67 | Types |
|------|---------|--------|
| **28** | Bridge | `{BUILDING_LOW_BRIDGE}` only |
| **29** | Ferry Landing | `{BUILDING_FERRY}` |

Merge: **UB-A** (JS `buildings[]` / `use_building` = menu; pak = audit).
Canary: `tests/114_bridge_editor_allow_dump.js` (skips under `--no-resource`).
Menu smoke: `tests/115_bridge_menu_smoke.js` (Perwadjyt REMAP + Bridges/Nekhen day-0).

Dump date: **2026-07-31** (`mission1.pak` + `Cleop/Maps/{Alexandria,Bridges}.map`).
Wiki Dev Ref: Low Bridge listed where JS unlocks it (2026-07-31 BR2).

| ID | Src | Bridge | Ferry | JS triage |
|----|-----|--------|-------|-----------|
| 0 Nubt | pak | 0 | 0 | OK (no unlock) |
| 1 Thinis tut | pak | 0 | 0 | OK |
| 2 Perwadjyt | pak | 0 | 0 | **REMAP** — tutorial `use_building` Low Bridge + Ferry |
| 3 Nekhen | pak | 1 | 1 | OK |
| 4 Men-nefer | pak | 1 | 0 | Bridge OK |
| 5 Timna | pak | 1 | 0 | Bridge OK |
| 6 Behdet | pak | 1 | 1 | OK |
| 7 Abedju | pak | 1 | 1 | OK |
| 8 Selima | pak | 1 | 0 | Bridge OK |
| 9 Abu | pak | 1 | 0 | Bridge OK; JS Ferry = REMAP (pak OFF) |
| 10 Saqqara | pak | 1 | 1 | OK |
| 11 Serabit | pak | 1 | 1 | OK (Ferry synced; map has no river) |
| 12 Meidum | pak | 1 | 0 | Bridge OK; JS Ferry = REMAP |
| 13 Buhen | pak | 1 | 1 | OK |
| 14 S. Dahshur | pak | 1 | 1 | OK |
| 15 N. Dahshur | pak | 1 | 1 | OK |
| 16 Iunet | pak | 1 | 1 | OK |
| 17 On | pak | 1 | 0 | Bridge OK; JS Ferry = REMAP |
| 18 Rostja | pak | 1 | 1 | OK |
| 19 Bahariya | pak | 1 | 0 | Bridge OK |
| 20 Djedu | pak | 1 | 1 | OK |
| 21 Dunqul | pak | 1 | 1 | OK (Ferry synced) |
| 22 Dakhla | pak | 1 | 1 | OK (Ferry synced) |
| 23–31 | pak | 1 | 1 | OK |
| 38–40 Valley | pak | 1 | 0 | Bridge OK; JS Ferry = REMAP |
| 41 Sumur | pak | 1 | 1 | OK |
| 42 Qadesh | pak | 1 | 1 | OK (Ferry synced) |
| 45 Pi-Yer | pak | 1 | 1 | OK |
| 128 Alexandria | Cleop map | 1 | 1 | OK |
| 129 Bridges | Cleop map | 1 | 1 | OK |

## Open / out of scope

- Cleo **32–37 / 43–44** — no mission JS yet
- Custom **130–135** — minimal sandbox `buildings[]` (SKIP)
- Re-export `data/maps` (zero reserved until batch re-export)
- Ferry REMAP cleanup (Abu / Meidum / On / Valley keep JS Ferry despite pak OFF)
- Hermetic CI: canary always skips without Cleop data
