# Handoff: empire-миссии → JS (2026-07-25)

Самодостаточная шпаргалка, чтобы продолжить в новой сессии.  
Полный playbook: **[MISSION_TO_JS.md](MISSION_TO_JS.md)**.  
Pak triage (skip/ladder/shared chain): **[MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md)**.  
**План очереди A ∥ B2:** **[REMAKE_EMPIRE_MISSIONS_PLAN.md](REMAKE_EMPIRE_MISSIONS_PLAN.md)**.  
Кампания / favour / грабли: **[REMAKE_NOTES.md](REMAKE_NOTES.md)**.  
B2 (event invasions): **[REMAKE_B2_INVASION_PLAN.md](REMAKE_B2_INVASION_PLAN.md)**.

**HEAD (до Serabit redefine):** `918d6559e` — video capture try/catch.  
**Saqqara + map_file:** `3affd5b21`. **Maps assets:** `b63b7c218`. **Abu:** `c10506b5f`.

---

## Цель трека

Полный redefine empire-карты + точные request/invasion-цепочки из `missionN.pak`
(через `__test_mission_pak_dump(N)`) в `src/scripts/mission/m_NNN_*.js`, плюс wiki
`docs/wiki/player/missions/`.

Эталон структуры: **`m_011_serabit_khadim.js`** / **`m_010_saqqara.js`**.  
Цепочки запросов/подарков: **m_004 … m_011**.

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

Миссии 0–2: wiki есть, empire почти из pak.  
12–18: скрипты есть, empire patch/частичный. **Следующий кандидат:** Meidum (12).

---

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

**Serabit (11) full redefine** — `m_011_serabit_khadim.js` + wiki + handoff/plan docs (эта сессия).  
**Следующий full redefine:** Meidum (**12**).  
Параллельно WIP: B2 invasion engine, UI scenario selection, video — **не** мешать в empire PR.

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
| Invasion poll / favour | m5–11 JS | proxy до B2 |
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

## Чеклист следующей миссии (**12 Meidum**)

**Старт сессии:** handoff freshness (D2) — `git log -1` / `git status` vs этот файл.  
Полный DoD: [`REMAKE_EMPIRE_MISSIONS_PLAN.md`](REMAKE_EMPIRE_MISSIONS_PLAN.md) +
[`MISSION_TO_JS.md`](MISSION_TO_JS.md) §6 + [`MISSION_PAK_TRIAGE.md`](MISSION_PAK_TRIAGE.md).

1. Дамп scenario **12** → ключевые `pak_*` (+ map points).
2. Triage: orphan/broken skip; shared leaf → один `ONLY_VIA`; funds = `int_dcy`.
3. **Triage decision log** (`copy`/`remap`/`skip` + почему) в скрипте и/или wiki.
4. Каркас empire из `m_011` / `m_010`.
5. `hide_pak_*` + города `pos`/`idx` (ours + display `route`).
6. `empire_routes` (2-pt + `deviation` если пусто) + `route_limits` + texts/ornaments.
7. Map points: entry/exit/river (overlay); `disembark_points` /
   `invasion_points_land|sea` из dump в JS (config-only).
8. Requests: `ok/refuse/late` + subtypes; PRICE/DEMAND — мутация state.
9. Invasions: poll до **B2-migrate**; favour из `by_favour` — **не** снимать JS proxy.
10. Wiki `meidum.html` refresh + index/nav + эта таблица + `MISSION_TO_JS.md` §7.
11. Stage check (D4): только файлы этой задачи.

**Позже / врезки:** B2→B2-migrate; FF1 FAILED_FLOOD Behdet; AUD1 subtype audit.

---

## Wiki

- Есть: mennefer / timna / behdet / abedju / selima / abu / saqqara / **serabit-khadim** / nekhen (+ earlier)
- **Следующий refresh:** `meidum.html` (mission 12 full redefine)

---

## Файлы

```
src/scripts/mission/m_011_serabit_khadim.js  ← Serabit done (this session)
src/scripts/mission/m_012_meidum.js          ← next
docs/wiki/player/missions/serabit-khadim.html
MISSION_TO_JS.md / MISSION_TO_JS_HANDOFF.md / MISSION_PAK_TRIAGE.md
REMAKE_EMPIRE_MISSIONS_PLAN.md / REMAKE_B2_INVASION_PLAN.md
```

---

## Verification (Serabit)

- Hot-reload JS: `--mixed src/scripts`.
- Empire id=8; trade Men-nefer/Selima/Abu/Nekhen/Behdet land (closed costs 350–1100).
- Funds Normal 15000 / loan 3500 / debt 20%; win pop 2000 / KR 80 / housing 4.
- y1m0+ clay pit floods ×5; y1m7 chickpeas gift + copper 8 recurring; y2m4+ beduin 16 recurring; favour 51.
- Map: entry [28,84] exit [83,84]; no river/disembark/invasion points.
- Choice → Buhen (13) / Dahshur (14).

## Verification (Saqqara)

- Empire id=1; funds Normal 12000; favour 69; choice → Serabit / Meidum.
