# Handoff: empire-миссии → JS (2026-07-25)

Самодостаточная шпаргалка, чтобы продолжить в новой сессии.  
Полный playbook: **[MISSION_TO_JS.md](MISSION_TO_JS.md)**.  
Pak triage (skip/ladder/shared chain): **[MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md)**.  
**План очереди A ∥ B2:** **[REMAKE_EMPIRE_MISSIONS_PLAN.md](REMAKE_EMPIRE_MISSIONS_PLAN.md)**.  
Кампания / favour / грабли: **[REMAKE_NOTES.md](REMAKE_NOTES.md)**.  
B2 (event invasions): **[REMAKE_B2_INVASION_PLAN.md](REMAKE_B2_INVASION_PLAN.md)**.

**HEAD:** Meidum (12) redefine committed as `db5dd9310`.  
**Serabit Khadim (11):** `9ac84fd3f`. **B2 native invasion (Phase 1–5):** `340f7d29d`.  
**Saqqara + map_file:** `3affd5b21`. **Maps assets:** `b63b7c218`. **Abu:** `c10506b5f`.  
**Next:** Buhen / South Dahshur (13/14).

---

## Цель трека

Полный redefine empire-карты + точные request/invasion-цепочки из `missionN.pak`
(через `__test_mission_pak_dump(N)`) в `src/scripts/mission/m_NNN_*.js`, плюс wiki
`docs/wiki/player/missions/`.

Эталон структуры: **`m_012_meidum.js`** / **`m_011_serabit_khadim.js`**.  
Цепочки запросов/подарков: **m_004 … m_012**.

---

## Статус миссий

| ID | Город | Empire full | Request chains | Invasions | Wiki | Заметки |
|----|-------|-------------|----------------|-----------|------|---------|
| 3 | Nekhen | нет (tutorial) | tutorial | — | `nekhen.html` | win: housing_level 8 = Modest Apt |
| **4** | Men-nefer | **да** | **да** | — | `mennefer.html` | map points ✅ |
| **5** | Timna | **да** | **да** | Libyan + favour 45 | `timna.html` | map points ✅ (no river) |
| **6** | Behdet | **да** | **да** | Kushite + favour 45 | `behdet.html` | map points + disembark ✅ |
| **7** | Abedju | **да** | **да** | Kushite + favour 40→40 | `abedju.html` | map points + disembark ✅ |
| **8** | Selima | **да** | **да** | Hyksos + favour 63 | `selima.html` | map points ✅ (no river) |
| **9** | Abu | **да** | **да** | favour 40→20→20 | `abu.html` | **committed** `c10506b5f` |
| **10** | Saqqara | **да** | **да** | favour 69 | `saqqara.html` | **`3affd5b21`**; `map_file`; oil×501 |
| **11** | Serabit | **да** | **да** | Libyan + beduin16 rec + favour 51 | `serabit-khadim.html` | empire id=8; funds 15k; housing 4 |
| **12** | Meidum | **да** | **да** | Hyksos y7m6+ size17 rec + favour 25→60 | `meidum.html` | empire id=1; funds 10k (pak); housing 11; monuments TEMP 58; **`db5dd9310`** |

Миссии 0–2: wiki есть, empire почти из pak.  
13–18: скрипты есть, empire patch/частичный. **Следующий кандидат:** Buhen / S. Dahshur (13/14).

---

## Meidum (12) — сделано (`db5dd9310`)

| Тема | Статус |
|------|--------|
| Full empire redefine (`hide_pak_*`, pos/idx, texts, ornaments) | ✅ |
| Empire id=1; land routes 1 (Men-nefer, display) / 2 (Serabit Khadim) / 5 (Saqqara); sea routes 4 (Abu) / 6 (Nekhen) / 9 (Behdet) | ✅ |
| funds 20000/13300/**10000 (pak)**/6700/5300; debt_interest 4/6/8/10/12 (Normal 8% pak); housing_level 11 | ✅ |
| Once requests: timber×10/6mo y3m4; reeds×8/4mo y5m7; wage decrease 5 y5m3 | ✅ |
| Recurring chains: clay (from pottery i7 y8m1+); timber (i19 y16m7+); grain (i41 y56m11+); papyrus (i53 y78m2+) | ✅ |
| Gift chains: stone→luxury×25 (i49 y69m7); grain→luxury×28 (i60 y85m9+ rec); stone→oil×21 (i63 y95m7+ rec) | ✅ |
| Shared ONLY_VIA KR leaves 1027/1028/1029/1030 reused by most requests | ✅ |
| Hyksos (`ENEMY_5_HYKSOS`, not Libyan) recurring raid size=17 y7m6+; favour Pharaoh 25→chain 60 | ✅ |
| Map: entry [111,141] exit [32,54] river [130,122]/[78,8]; invasion land [114,79] sea [31,7] | ✅ |
| Triage: skip route 47 (orphan, no city); skip map_obj idx=7 (empty stub); skip y≥98 junk (i65/66/67/68/69); skip orphan chain-only leaves i46/i58 | ✅ |
| Monuments goal TEMP=58 (pak goal 39; C1 stepped-pyramid-complex not implemented) | ✅ (tracked C1/F3) |
| Wiki `meidum.html` refresh | ✅ |

## Serabit Khadim (11) — сделано (2026-07-25)

| Тема | Статус |
|------|--------|
| Full empire redefine (`hide_pak_*`, pos/idx, texts, ornaments) | ✅ |
| Empire id=8; land routes 1/2/4/6/7; Saqqara display route 8 stub | ✅ |
| funds 15k/loan 3.5k / debt 20 → int_dcy; housing_level 4 | ✅ |
| Gift chickpeas×32; copper 8 recurring; copper 11; weapons 13/21; luxury 16 recurring | ✅ |
| Shared ONLY_VIA leaves 21/23/24 + 26/28/25 + weapons2 refuse Kerma→Nekhen | ✅ |
| Clay floods i=16–20 y1m0+ recurring ×5 | ✅ (+ `BUILDING_CLAY_PIT`) |
| Raids: Libyan 8/12/20/28/28/32; beduin 16 recurring y2m4+; favour 51 | ✅ |
| Map: entry/exit; omit river/disembark/invasion (pak empty) | ✅ |
| Triage: skip y100 clay floods; skip y1 clay (no pits); skip ok→99; orphan routes 3/5/9/11 | ✅ |
| Wiki `serabit-khadim.html` refresh | ✅ |

## Saqqara (10) — сделано (`3affd5b21`)

| Тема | Статус |
|------|--------|
| Full empire redefine | ✅ |
| funds 12k/loan 5.2k / debt 8; housing_level 10 | ✅ |
| Favour 69 (JS proxy) | ✅ |
| `map_file` + campaign load from `data/maps` | ✅ |

---

## Состояние рабочей копии

**Meidum (12) full redefine** — `m_012_meidum.js` + wiki + handoff/plan docs, **committed** `db5dd9310`.  
**Серабит (11)** committed: `9ac84fd3f`.  
**Следующий full redefine:** Buhen / South Dahshur (**13/14**).  
Параллельно WIP: B2 invasion engine (Phase 1–5 в коде, `340f7d29d`), UI scenario selection, video — **не** мешать в empire PR.

---

## API (важно для продолжения)

### One-shot / chain events

```js
city.create_chain_event({
    tag_id: 601,
    type: EVENT_TYPE_PRICE_DECREASE,
    resource: RESOURCE_LUXURY_GOODS,
    amount: 10,
    trigger: EVENT_TRIGGER_ONCE         // default = ONLY_VIA_EVENT
}).execute()
```

- Default `ONLY_VIA_EVENT`: `.execute()` не сработает. Для one-shot — `EVENT_TRIGGER_ONCE`.
- `create_good_request({ …, trigger: EVENT_TRIGGER_ONLY_VIA_EVENT, subtype })` — chain child.
- `__city_event_fire_chain(tag)` — активировать ONLY_VIA master из JS.

### Meta по сложности (`int_dcy`)

```js
initial_funds [7500, 5000, 3750, 2500, 2000]
rescue_loans  [7500, 5000, 3750, 2500, 2000]
debt_interest [10, 15, 20, 25, 30]   // annual %; 0/empty → pak rate → 10
house_tax_multipliers [300, 200, 150, 100, 75]
```

### Map points из mission JS (`scenario_data_t::load_metadata`)

| Ключ | Нет ключа | Примечание |
|------|-----------|------------|
| `entry_point` / `exit_point` / river / `earthquake_point` | **оставить pak** | optional overlay |
| `disembark_points` | **пусто** | config-only; sparse `[-1,-1]` |
| `invasion_points_land` / `invasion_points_sea` | **пусто** | config-only; hvector; save via `bind_hvector_tiles_xy_u16` |

Dump markers: `pak_entry:`, `pak_exit:`, `pak_river_*`, `pak_earthquake:`,
`pak_disembark:`, `pak_inv_land:` / `pak_inv_sea:`.

### Campaign city map (`map_file`)

```js
map_file : "data/maps/m_011_serabit_khadim.map"   // missions 0–18
```

### Empire routes: 2 точки + deviation

```js
{ route: 8, type: 1, deviation: 40, points: [ [801, 552], [512, 534] ] }
```

Favour: `mission_pharaoh_favour_invasion_tick(mission, size[, chain])`.

---

## Известные дыры / stubs

| Что | Где | Статус |
|-----|-----|--------|
| `EVENT_TYPE_DEMAND/SEA/LAND/WAGE/CONTAMINATED/CLAY` | engine | ✅ |
| `EVENT_TYPE_PRICE_↑/↓` | engine | ✅ |
| `CITY_STATUS` subtype 1 (`FOREIGN_CITY_CONQUERED`) | engine | ✅ (Abu) |
| `debt_interest` / map-point overlays | engine + m4–11 | ✅ |
| `invasion_points_*` from mission JS | engine hvector + m2, m5–18 | ✅ |
| empire route `deviation` | `improve_route` | ✅ |
| `EVENT_TYPE_FAILED_FLOOD` | Behdet y10 | handler есть; JS пока **ONCE** (**FF1**) |
| **`EVENT_TYPE_INVASION` + deferred on_completed** | engine | **TODO — [B2](REMAKE_B2_INVASION_PLAN.md)** |
| Invasion poll / favour | m5–12 JS | proxy до B2 |
| Orphan LOST_TRADE / broken editor links | pak junk | skip |
| B3 invasion warnings / B4 phrase_id | engine | TODO |

---

## Как дампить pak

Данные: `d:/Work/Cleop`. Бинарь: `build/Debug/akhenaten.exe`.

```js
// tests/99_tmp_missionN_dump.js — временный, удаляй после
function run_test() {
    __test_mission_pak_dump(N)
    __test_signal_ready()
}
function check_valid() { return true }
```

```bat
.\build\Debug\akhenaten.exe --integraltests --integraltest-only 99_tmp_missionN --noconfig-window --nosound --nointro --nomouse --no-logo --window --size 800x600 "d:/Work/Cleop"
```

---

## Чеклист следующей миссии (**13/14 Buhen / S. Dahshur**)

**Старт сессии:** handoff freshness (D2) — `git log -1` / `git status` vs этот файл.  
Полный DoD: [`REMAKE_EMPIRE_MISSIONS_PLAN.md`](REMAKE_EMPIRE_MISSIONS_PLAN.md) +
[`MISSION_TO_JS.md`](MISSION_TO_JS.md) §6 + [`MISSION_PAK_TRIAGE.md`](MISSION_PAK_TRIAGE.md).

1. Дамп scenario **13** (Buhen) и/или **14** (S. Dahshur) → ключевые `pak_*` (+ map points).
2. Triage: orphan/broken skip; shared leaf → один `ONLY_VIA`; funds = `int_dcy`.
3. **Triage decision log** (`copy`/`remap`/`skip` + почему) в скрипте и/или wiki.
4. Каркас empire из `m_012` / `m_011`.
5. `hide_pak_*` + города `pos`/`idx` (ours + display `route`).
6. `empire_routes` (2-pt + `deviation` если пусто) + `route_limits` + texts/ornaments.
7. Map points: entry/exit/river (overlay); `disembark_points` /
   `invasion_points_land|sea` из dump в JS (config-only).
8. Requests: `ok/refuse/late` + subtypes; PRICE/DEMAND — мутация state.
9. Invasions: poll до **B2-migrate**; favour из `by_favour` — **не** снимать JS proxy.
10. Wiki `buhen.html` / `south-dahshur.html` refresh + index/nav + эта таблица + `MISSION_TO_JS.md` §7.
11. Stage check (D4): только файлы этой задачи. **Не коммитить** Meidum (12) в тот же PR — отдельный коммит.

**Позже / врезки:** B2→B2-migrate; FF1 FAILED_FLOOD Behdet; AUD1 subtype audit.

---

## Wiki

- Есть: mennefer / timna / behdet / abedju / selima / abu / saqqara / serabit-khadim / **meidum** / nekhen (+ earlier)
- **Следующий refresh:** `buhen.html` / `south-dahshur.html` (mission 13/14 full redefine)

---

## Файлы

```
src/scripts/mission/m_012_meidum.js          ← Meidum done `db5dd9310`
src/scripts/mission/m_013_buhen.js           ← next
src/scripts/mission/m_014_south_dahshur.js   ← next
docs/wiki/player/missions/meidum.html
MISSION_TO_JS.md / MISSION_TO_JS_HANDOFF.md / MISSION_PAK_TRIAGE.md
REMAKE_EMPIRE_MISSIONS_PLAN.md / REMAKE_B2_INVASION_PLAN.md
```

---

## Verification (Meidum)

- Hot-reload JS: `--mixed src/scripts`.
- Empire id=1; trade Abu/Behdet/Nekhen sea (650–1050 closed), Saqqara/Serabit Khadim land (200/350 closed); Men-nefer display-only pharaoh route.
- Funds Normal 10000 (pak) / loan 3000 / debt 8%; win pop 3000 / culture 25 / prosperity 25 / monuments TEMP 58 / KR 40 / housing 11.
- y3m4 timber×10/6mo; y5m3 wage decrease 5; y5m7 reeds×8/4mo; y7m6+ Hyksos raid size 17 recurring; favour 25→chain 60.
- Map: entry [111,141] exit [32,54] river [130,122]/[78,8]; invasion land [114,79] sea [31,7].
- Choice → Buhen (13) / Dahshur (14).

## Verification (Serabit)

- Hot-reload JS: `--mixed src/scripts`.
- Empire id=8; trade Men-nefer/Selima/Abu/Nekhen/Behdet land (closed costs 350–1100).
- Funds Normal 15000 / loan 3500 / debt 20%; win pop 2000 / KR 80 / housing 4.
- y1m0+ clay pit floods ×5; y1m7 chickpeas gift + copper 8 recurring; y2m4+ beduin 16 recurring; favour 51.
- Map: entry [28,84] exit [83,84]; no river/disembark/invasion points.
- Choice → Buhen (13) / Dahshur (14).

## Verification (Saqqara)

- Empire id=1; funds Normal 12000; favour 69; choice → Serabit / Meidum.
