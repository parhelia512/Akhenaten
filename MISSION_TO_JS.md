# Перевод миссий Pharaoh → JS (Akhenaten)

Как переносить данные и логику миссии из `mission1.pak` в скрипт
`src/scripts/mission/m_NNN_*.js`, чтобы карта империи, торговля и события
управлялись из JS, а не только из pak.

Эталон полной empire-карты: **`m_004_mennefer.js`**.  
Общий playbook кампании (запросы, вторжения, choice): **[REMAKE_NOTES.md](../REMAKE_NOTES.md)**.

---

## 1. Зачем

Старт миссии всегда грузит scenario из pak. JS-секция `missionN { ... }`
накладывается поверх: правит города, маршруты, объекты карты, критерии победы,
здания, обработчики событий.

Цель перевода empire-части — миссия могла жить без зависимости от pak-объектов
карты (удобно для правок и будущих custom-миссий). Карта города (тайлы,
здания на старте) по-прежнему из pak/map.

---

## 2. Файлы и регистрация

| Что | Где |
|-----|-----|
| Скрипт миссии | `src/scripts/mission/m_NNN_cityname.js` |
| Импорт | `src/scripts/missions.js` (`import mission/m_…`) |
| Секция конфига | `missionN { … }` (N = номер scenario, 0-based в имени файла часто = N) |
| Wiki игрока | `docs/wiki/player/missions/<city>.html` + строка в `index.html` |
| Pak dump (без JS overlay) | `__test_mission_pak_dump(scenario_id)` — ad-hoc `tests/99_tmp_missionN_dump.js` (см. handoff) |

Hot-reload скриптов: `--mixed PATH` (каталог с `src/scripts`).

---

## 3. Дамп из pak

Нужны данные Pharaoh (`mission1.pak`). Пример теста:

```js
// tests/XX_missionN_pak_dump.js
function run_test() {
    __test_mission_pak_dump(4)  // scenario id
    __test_signal_ready()
}
function check_valid() { return true }
```

Запуск:

```bat
akhenaten.exe --integraltests --integraltest-only XX_missionN --noconfig-window --nosound "D:\path\to\Pharaoh"
```

В логе маркеры `[test-marker] pak_…`:

| Маркер | Содержание |
|--------|------------|
| `pak_empire:id=…` | empire id / expanded / expansion_year |
| `pak_map_city:…` | города на карте (имя, type, trade, sells/buys, route, cost…) |
| `pak_route_limits:…` | годовые лимиты по ресурсам на маршруте |
| `pak_map_route:…` | polylines (`id`, `type` 1=land/2=sea, точки) |
| `pak_map_obj:…` | объекты карты (city/text/battle/land_route/sea_route/…) |
| `pak_event:…` / `pak_request:…` / `pak_invasion_event:…` | timed events |
| `pak_win:…` | критерии победы |

Сверяй числа с JS после перевода.

---

## 4. Каркас `missionN { }`

Минимальный набор (как в кампании 0–18):

```js
mission4 {
    start_message : "message_…"
    selection_title : "Mennefer"
    player_rank : 1
    initial_funds [ … ]
    rescue_loans [ … ]
    house_tax_multipliers [ … ]
    env { has_animals : false, marshland_grow : default_marshland_grow, tree_grow : default_tree_grow }
    buildings [ BUILDING_…, … ]
    sounds { briefing : "Voice/Mission/…", victory : "Voice/Mission/…" }
    win_criteria { population {…} culture {…} … }
    // map points (load_metadata):
    // entry/exit/river/earthquake — optional overlay (omit key → keep pak)
    // entry_point [x, y]   exit_point [x, y]
    // river_entry_point [x, y]   river_exit_point [x, y]
    // earthquake_point [x, y]
    // disembark / invasion — mission config only (omit key → empty; not pak):
    // disembark_points [ [x, y], [-1, -1], [x, y] ]  // sparse slots
    // invasion_points_land [ [x, y], … ]   // sparse → [-1,-1]; dump: pak_inv_land
    // invasion_points_sea  [ [x, y], … ]
    // empire — см. §5
    vars { … }   // флаги прогресса туториала / one-shot событий
}
```

Старт и логика — обработчики:

```js
[es=event_mission_start, mission=mission4]
function mission4_on_start(ev) {
    mission_show_start_message(mission, "message_…")
    empire.set_id(1)
    empire.set_expanded(false)
    // advisors, use_building, set_empire_available…
}

[event=event_advance_month, mission=mission4]
function mission4_…(ev) { … }
```

Прогрессия: `next_mission` **или** `choice[]` (см. REMAKE_NOTES §2).  
Запросы / вторжения / distant battle — паттерны там же §1.

**Empire id / expanded** задавай в `event_mission_start` через API
(`empire.set_id`, `empire.set_expanded`, `empire.expand`), не статическим
блоком `empire { }` в конфиге миссии.

---

## 5. Empire-карта из JS

Загрузка: `empire_t::load_mission_metadata` после pak. Флаги `hide_pak_*`
сначала чистят pak, потом читаются массивы из секции миссии.

### 5.1 Флаги

| Флаг | Действие |
|------|----------|
| `hide_pak_cities` | все `city.in_use = 0`; потом `cities[]`; unused city-objects снимаются |
| `hide_pak_routes` | очистка polylines; потом `empire_routes[]` |
| `hide_pak_objects` | все non-city map objects; потом texts/ornaments/battle/land/sea/armies |

Без флагов массивы **патчат/дополняют** pak (режим частичного перевода).

### 5.2 Фон карты

```js
map_background : {pack:PACK_EMPIRE, id:1}
// или: {path:"…"} / "path/to/image"
```

Если не задан — UI берёт дефолт `empire_window.image`.

### 5.3 Города — два режима

**Полное переопределение** (эталон m_004): есть `pos` и/или `idx` → создаётся/
заменяется map object + city slot.

```js
hide_pak_cities : true
cities [
    {
        name : "Men-nefer"          // lang group 195 / 21
        idx : 0                     // опционально: слот объекта
        pos : [541, 491]
        route : 0
        type : EMPIRE_CITY_OURS     // обязательно наш город при hide_pak_cities
        sells [ RESOURCE_… ]
        buys [ RESOURCE_… ]
    }
    {
        name : "Perwadjyt"
        pos : [489, 350]
        route : 1
        is_open : false
        cost_to_open : 300
        is_sea_trade : false
        type : EMPIRE_CITY_EGYPTIAN_TRADING
        max_traders : 1
        trade_limits : default_trade_limits   // UI-лестница лимитов
        sells [ … ]
        buys [ … ]
        route_limits [                        // годовые лимиты на маршруте
            { resource: RESOURCE_FIGS, limit: 4000 }
            { resource: RESOURCE_CLAY, limit: 2500, traded: 0 }
        ]
    }
    // display-only:
    { name : "Nubt", trade : false, type : EMPIRE_CITY_EGYPTIAN, cost_to_open : 550 }
]
```

**Патч по имени** (многие миссии 5–18): без `pos`/`idx` — ищет уже
существующий pak-город по `name` и правит trade/sells/buys.

Иконки городов **не** из pak `image_id`: `empire_city_images` в `empire.js`
по `type`.

`route_limits` — реальные yearly limits (`trade_route::set_limit`).  
Без массива для отсутствующих sells/buys по-прежнему default 1500.

### 5.4 Polylines маршрутов

```js
hide_pak_routes : true
empire_routes [
    {
        route : 1
        type : 1          // 1=land, 2=sea
        points [ [508, 382], [520, 399], … ]
    }
]
```

Рисование открытого/выделенного маршрута — JS drawer
`[es=(empire_window, draw_map, EMPIRE_OBJECT_TRADE_ROUTE)]`.

После load: `fix_trade_routes()` → `improve_route()` (сегменты >50 режутся;
`deviation : N` — случайный перпендикулярный сдвиг midpoints).

Если у display-city `route=N`, а `pak_map_route` пуст — достаточно **двух точек**
(ours → city) + опц. `deviation`. Поле `route` у города всё равно ставить из pak.

В scenario 4 (Men-nefer) **нет** land/sea *marker objects*
(`pak_map_obj_count … land_route=0|sea_route=0`) — достаточно polylines.

**Triage pak** (orphan / ladder / shared chain / remap): [`MISSION_PAK_TRIAGE.md`](MISSION_PAK_TRIAGE.md).

### 5.5 Прочие map objects

После `hide_pak_objects : true`:

```js
empire_texts [
    { name : "#sinai", pos : [787, 478] }   // #key или строка lang 196
]
empire_ornaments [ { pos : […], image : "…" /* или {pack,id} / tid */ } ]
empire_battle_icons [ { pos : […], path : 1, years : 2, image : … } ]
empire_land_routes [ { pos : […], route : 1, image : … } ]
empire_sea_routes  [ { pos : […], route : 2, image : … } ]
empire_kingdome_armies [ { pos : […], months : N } ]
empire_enemy_armies    [ { pos : […], months : N } ]
```

Общие поля объекта: `idx`, `pos`, `image` / `expanded_pos` / `expanded_image`
(см. `begin_script_map_object` в `empire_object.cpp`).

Battle icons рисуются JS drawer’ом
`[es=(empire_window, draw_map, EMPIRE_OBJECT_BATTLE_ICON)]`
(спрайт `pharaoh_general/empire_bits_00001`).

### 5.6 Что не из статического JS

| Кусок | Источник |
|-------|----------|
| Traders | runtime (`empire_traders`) |
| Invasion warnings | runtime + events |
| Distant battle path/icon | runtime `empire.active_battle` |
| Тайлы карты / стартовые здания | pak / `.map` |
| Timed events из pak | можно дублировать handlers; `enable_scenario_events` |

---

## 6. Чеклист перевода миссии (DoD)

1. **Дамп** scenario N → сохранить/сверить `pak_*` маркеры.
2. **Triage** — [`MISSION_PAK_TRIAGE.md`](MISSION_PAK_TRIAGE.md): skip orphan/broken;
   ladder vs одно число; shared leaf → один `ONLY_VIA` master.
3. **Каркас** `missionN`: `int_dcy` funds/loans/tax/(debt_interest), rank, buildings,
   win_criteria, sounds, vars.
4. **Empire id** в `on_start`: `empire.set_id` / `set_expanded`.
5. **Города**: full redefine = `hide_pak_cities` + `pos`/`idx` у **всех** (ours + display).
6. **Маршруты**: `hide_pak_routes` + `empire_routes`; пустые → 2-pt ± `deviation`.
7. **Лимиты**: `route_limits` из `pak_route_limits` (не путать с `trade_limits`).
8. **Тексты/объекты**: `hide_pak_objects` + нужные массивы.
9. **Фон**: `map_background` при необходимости.
10. **Map points**: `entry_point` / `exit_point` / river из dump (omit invalid = keep pak).
    `disembark_points` / `invasion_points_land|sea` — **только из JS** (нет ключа → пусто);
    sparse → `[-1,-1]`. Entry/exit/river уже в m4–10; invasion points — m2 + m5–18 (где
    pak не пуст, `697a61836`).
10b. **`map_file`**: `"data/maps/m_NNN_….map"` — city grids; load prefers map, falls back to pak
    (`3affd5b21`).
11. **События**: requests (ok/refuse/late + subtypes); PRICE/DEMAND/WAGE — handler
    **мутирует** state (не только фраза); invasions = poll до B2.
12. **Старт**: advisors, `set_empire_available`, tutorial flags.
13. **Wiki** + `missions.js` import + handoff status row.
14. **Проверка**: empire map, open trade (`is_open`), один shared leaf без double-fire.

---

## 7. Статус по миссиям (empire map)

| Уровень | Миссии | Что сделано |
|---------|--------|-------------|
| Full redefine | **4**–**11** (Men-nefer … Serabit) | cities+pos, routes, texts, ornament, map_background, hide_pak_*, **map points**, **`map_file`**; wiki: … / saqqara / serabit-khadim |
| Patch / cities list | 12–18 | `cities[]` без полного hide routes/objects |
| Минимум | 0–3 | туториал, empire map почти из pak |

**Следующий full redefine:** Meidum (**12**).

**Сессионный handoff:** [MISSION_TO_JS_HANDOFF.md](MISSION_TO_JS_HANDOFF.md).  
**Очередь работ A ∥ B2 → B2-migrate + FF1/D1–D4:** [REMAKE_EMPIRE_MISSIONS_PLAN.md](REMAKE_EMPIRE_MISSIONS_PLAN.md).  
**Pak triage:** [MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md).  
**B2 invasions:** [REMAKE_B2_INVASION_PLAN.md](REMAKE_B2_INVASION_PLAN.md).

При полном переносе копируй структуру **m_009** (или m_008), данные — из dump.
Цепочки: `ok`/`refuse`/`late` indices + общие leaf’ы через shared `ONLY_VIA`.
Map points: §4 + `scenario.cpp` `load_metadata`.

---

## 8. Полезные пути в коде

- `src/empire/empire.cpp` — `load_mission_metadata`
- `src/empire/empire_object.cpp` — лоадеры cities/routes/texts/…
- `src/empire/empire_js.cpp` — `set_id` / `set_expanded` / `expand`
- `src/scripts/empire.js` — `empire_city_images`, API
- `src/scripts/ui_empire_window.js` — drawers (routes, traders, battle)
- `src/js/js_test_mission_pak_dump.cpp` — дамп маркеров
- `src/window/window_empire.cpp` — draw + emit JS events

---

## 9. Dev-команды

```
empire_route_debug on|off     — точки polyline на карте
save_empire_routes            — экспорт текущих polylines в empire_routes.js
```
