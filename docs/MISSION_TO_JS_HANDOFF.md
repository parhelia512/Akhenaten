# Handoff: empire-миссии 4–7 → JS (2026-07-25)

Самодостаточная шпаргалка, чтобы продолжить в новой сессии.  
Полный playbook: **[MISSION_TO_JS.md](MISSION_TO_JS.md)**.  
Кампания / B2 favour / грабли: **[REMAKE_NOTES.md](../REMAKE_NOTES.md)**.

---

## Цель трека

Полный redefine empire-карты + точные request/invasion-цепочки из `mission1.pak`
в `src/scripts/mission/m_NNN_*.js`, плюс wiki `docs/wiki/player/missions/`.

Эталон структуры: **`m_004_mennefer.js`** (full hide_pak_* + cities/routes/texts).  
Цепочки запросов/подарков: смотри **m_004 / m_005 / m_006** (не упрощённые ±8/±6).

---

## Статус миссий

| ID | Город | Empire full | Request chains | Invasions | Wiki | Заметки |
|----|-------|-------------|----------------|-----------|------|---------|
| 3 | Nekhen | нет (tutorial) | tutorial | — | `nekhen.html` | housing goal Modest Apt lvl 8 |
| **4** | Men-nefer | **да** | **да** (dump 07-25) | — | `mennefer.html` | beer→+5→gift 20 bricks; papyrus demand±; barley/pottery +9/−6 |
| **5** | Timna | **да** | **да** | Libyan + favour 45 | `timna.html` | gems/weapons→Men-nefer trade unlock; deben→gift chickpeas×16 |
| **6** | Behdet | **да** | **да** | Kushite + favour 45 | `behdet.html` | beer→gift 28 bricks; y15 clear→KR+8 (poll); papyrus demand y1; FAILED_FLOOD пока once (pak recurring) |
| **7** | Abydos / Abedju | **да** | **да** (dump 07-25) | Kushite + favour 40→40 | `abedju.html` | beer→+13→gift 21 bricks; fish +10/−10/−9; perfect flood yearly m8 |

Миссии 0–2: wiki есть (nubt/thinis/perwadjyt), empire почти из pak.  
8–18: скрипты есть, empire patch/частичный. **Следующий кандидат:** Selima (8).

Последний коммит по треку: `3ecc3889d Wire Men-nefer request chains and finish Behdet empire/wiki`.

---

## Незакоммичено (на момент handoff)

Проверь `git status` — в рабочей копии могут быть **два разных слоя**:

### A. Mission 6 follow-up (этот трек) — **включить в следующий commit**

- `src/scripts/mission/m_006_behdet.js` — demand papyrus y1; invasion2 → KR+8
- `src/scripts/city.js` — `create_chain_event().execute()`, `city.num_enemy_formations`
- `src/scenario/request_js.cpp` / `scenario_event_manager.{h,cpp}` — `trigger` у `create_chain_event`, дата = now, `EVENT_TRIGGER_*` в JS via `ANK_CONFIG_ENUM`
- `src/city/city_js.cpp` — `__enemy_army_total_enemy_formations`
- `docs/wiki/player/missions/behdet.html` — обновлённые события

### B. Другая работа (мосты) — **не мешать с A**

В status также видны `building_bridge*`, `grid/bridge*`, `m_129_bridges.js`, `widget_minimap*`, `REMAKE_BRIDGE_PLAN.md` и т.п. — отдельный трек, не коммитить вместе с mission6.

Untracked полезные для трека: `docs/MISSION_TO_JS.md`, `docs/MISSION_TO_JS_HANDOFF.md`, `tests/45_mission11_pak_dump.js`, `REMAKE_NOTES.md`.

---

## Новый API (важно для продолжения)

### One-shot event из JS

```js
city.create_chain_event({
    tag_id: 601,
    type: EVENT_TYPE_DEMAND_INCREASE,   // или REPUTATION_INCREASE / GIFT_…
    resource: RESOURCE_PAPYRUS,         // optional
    amount: 6,
    trigger: EVENT_TRIGGER_ONCE         // default = ONLY_VIA_EVENT (цепочка)
}).execute()
```

- Без `trigger: EVENT_TRIGGER_ONCE` событие **не** сработает через `.execute()` (только via parent chain).
- `create_chain_event` теперь пишет `time` = текущий год/месяц (нужно для ONCE + date match).

### Детект «враги убиты»

```js
city.num_enemy_formations   // → enemy_army_total_enemy_formations()
```

Паттерн Behdet y15 (pak `ok→REPUTATION_INCREASE +8`):

1. Стартовать рейд.
2. Со следующего месяца: если `num_enemy_formations > 0` → `enemies_seen = true`.
3. Когда `enemies_seen && num_enemy_formations == 0` → fire KR+8 once.

Это обход, пока **B2** (настоящие pak `EVENT_TYPE_INVASION` + `on_completed`) не сделан.

### Цепочки запросов (эталон)

Два рабочих паттерна (оба в m_004 / m_006):

```js
// A) Mennefer-style: ok = KR, затем gift на ok_ev
var ok_ev = missionN_fire_request(tag, RES, amt, months, ok, fail, late, okA, failA, lateA)
city.create_chain_event({ tag_id: next, type: EVENT_TYPE_GIFT_FROM_PHARAOH, resource: RESOURCE_BRICKS, amount: 20 })
ok_ev.set_completed_action_tag(next)

// B) Behdet beer-style: gift сразу как completed у request (без промежуточного KR)
var request = city.create_good_request({ tag_id: 2, resource: RESOURCE_BEER, amount: 11, months_initial: 12 })
city.create_chain_event({ tag_id: 201, type: EVENT_TYPE_GIFT_FROM_PHARAOH, resource: RESOURCE_BRICKS, amount: 28 })
request.set_completed_action_tag(201)
```

Смотри `mission4_fire_request` / `mission5_fire_request` / `mission6_fire_request` — late tag обязателен, если в dump `late≠refuse`.

---

## Известные дыры / stubs

| Что | Где | Статус |
|-----|-----|--------|
| `EVENT_TYPE_DEMAND_INCREASE/DECREASE` | engine | **реализовано**: `increase_limit` / `decrease_limit` на открытых маршрутах + demand phrases |
| `EVENT_TYPE_SEA/LAND_TRADE_PROBLEM` | engine | **реализовано**: 48 mo disruption + stormy_seas / sandstorm / landslide phrases |
| `EVENT_TYPE_WAGE_INCREASE/DECREASE` | engine | **реализовано**: `raise/lower_wages_kingdome` + wage phrases |
| `EVENT_TYPE_CONTAMINATED_WATER` | engine | **реализовано**: health −25/−40/−50 (как random_events) + bad_water phrases |
| `EVENT_TYPE_CLAY_PIT_FLOOD` | engine + Behdet y8m6 | **реализовано**: rubble random clay pit + `message_tutorial_flooded_clay_pit` |
| `EVENT_TYPE_FAILED_FLOOD` | engine + Behdet y10 | **handler есть** (`quality_next = 0`); Behdet JS пока **ONCE** (pak — recurring) |
| `EVENT_TYPE_PERFECT_FLOOD` | engine + Abedju y2m8 | **реализовано**: `quality_next = 100`; Abedju JS — recurring yearly m8 |
| `EVENT_TYPE_INVASION` process | engine | **TODO**; JS проксирует `start_foreign_army_invasion` |
| Invasion `on_completed` KR | pak | только Behdet y15 обойдён poll’ом; Timna invasions без ok-chain |
| Favour KR punishment | B2b/c | helper `mission_pharaoh_favour_invasion_tick(mission, size[, chain])`; Abedju: 40→clear→40 |

Pak `attack=` → JS `invasion_attack_target` (`EVENT_ATTACK_TARGET_*`, 0..4).  
`attack=4` = `EVENT_ATTACK_TARGET_RANDOM` → engine `FORMATION_ATTACK_RANDOM` (5). Default в API тоже RANDOM.

---

## Как дампить pak

Данные: `d:/Work/Cleop` (локально). Бинарь: `build/Debug/akhenaten.exe`.

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

Фильтр маркеров: `pak_event:`, `pak_request:`, `pak_invasion_event:`, `pak_map_city:`, `pak_route_limits:`, `pak_map_route:`, `pak_ornament`, `pak_map_obj_count:`, `pak_win:`.

Дамп грузит **raw pak без JS overlay** (`load_mission_pak_raw`).  
Осторожно: не удаляй temp-тест пока процесс ещё стартует — получишь чужой subtitle/мусор.

Постоянный тест: `tests/45_mission11_pak_dump.js`.

---

## Чеклист следующей миссии (рекомендуется **8 Selima**)

1. Дамп scenario **8** → сохранить ключевые `pak_*`.
2. Скопировать каркас empire из `m_004` / `m_006` / `m_007`.
3. `hide_pak_cities/routes/objects` + все города с `pos`/`idx` (включая ours).
4. `empire_routes` + `route_limits` + texts + ornaments/battle если есть.
5. Requests: **не** усреднять KR — читать `ok/refuse/late` indices в dump и цепочки (gift / demand / city_status).
6. Invasions: enemy_id из `pak_enemy_id`; favour size из invasion `by_favour`.
7. Wiki `selima.html` + index + nav.
8. Обновить таблицу статуса в `MISSION_TO_JS.md` §7.

---

## Wiki уже есть

- `docs/wiki/player/missions/mennefer.html` — mission 4  
- `docs/wiki/player/missions/timna.html` — mission 5  
- `docs/wiki/player/missions/behdet.html` — mission 6  
- `docs/wiki/player/missions/abedju.html` — mission 7  
- `docs/wiki/player/missions/nekhen.html` — mission 3  
- Index Early Dynastic: ссылки на 4–7

---

## Файлы быстрого доступа

```
src/scripts/mission/m_004_mennefer.js
src/scripts/mission/m_005_timna.js
src/scripts/mission/m_006_behdet.js
src/scripts/mission/m_007_abydos.js
src/scripts/mission/m_008_selima.js      ← next
src/scripts/missions.js                 ← favour helper, imports
src/scripts/city.js                     ← create_chain_event / num_enemy_formations
src/scenario/scenario_event_manager.*   ← create_chain_event + process stubs
src/scenario/request_js.cpp
src/js/js_test_mission_pak_dump.cpp
docs/MISSION_TO_JS.md
docs/MISSION_TO_JS_HANDOFF.md
docs/wiki/player/missions/
```

---

## Предлагаемый commit message (только слой A)

```
Wire Behdet papyrus demand and y15 invasion KR reward

Fire timed DEMAND_INCREASE and +8 KR after the size-16 Kushite raid clears;
extend create_chain_event with ONCE+execute and expose enemy formation count.
```

---

## Verification

- Rebuild после правок C++ (`create_chain_event` signature / city_js).  
- Hot-reload JS: `--mixed src/scripts`.  
- В игре Behdet: year 1 demand popup; year 15 raid → после зачистки KR +8.  
- Flood handlers в engine уже есть: y8m6 clay-pit flood; y10 failed flood (сейчас one-shot — сверить с pak recurring при полировке).
