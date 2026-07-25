# Заметки по скриптованию миссий и находки (на будущее)

Рабочие заметки, накопленные при скриптовании миссий 11–18 (задачи D1–D5).
Здесь — то, что **не является отдельной задачей**, но важно знать перед продолжением:
переиспользуемые «грабли», кросс-задачные находки и сводка технического долга.
Действия по конкретным дефектам живут в **[REMAKE_TASKS_P1.md](REMAKE_TASKS_P1.md)**
(задачи B5, F1, F2, F3, C1/C3/~~C4~~/~~C6~~/C7, D1b) — здесь только указатели на них.

Последнее обновление: 2026-07-25 (Selima empire/events; PRICE/debt_interest/`int_dcy`;
route `deviation`; план **B2** — [`REMAKE_B2_INVASION_PLAN.md`](REMAKE_B2_INVASION_PLAN.md);
§9–10 HR/terrain — без изменений). Заскриптованы миссии 0–18; empire full **4–8**.

---

## 1. Playbook: как скриптуется миссия кампании (эталон)

Эталоны: `m_010_saqqara.js` (монумент+choice), `m_008_selima.js` (empire full + Hyksos/
Kerma/troops; invasions пока poll), `m_016_iunet.js` (полный набор).
Регистрация — `import` в `src/scripts/missions.js`.
Полный перевод **empire-карты**: эталон `m_004`…`m_008`, гайд —
**[MISSION_TO_JS.md](MISSION_TO_JS.md)** / handoff **[MISSION_TO_JS_HANDOFF.md](MISSION_TO_JS_HANDOFF.md)** /
triage **[MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md)**.

Обязательные части (правило D0 в задачнике):
1. Блок `missionN { ... }`: `start_message`, `player_rank`, **`int_dcy`**
   funds/loans/tax/(debt_interest), `env`, `sounds`, `buildings[]`, `win_criteria`,
   `cities[]`, `vars`.
2. `enable_scenario_events : true` + обработчики `[es=event_advance_month, mission=missionN]`.
3. Прогрессия: `next_mission` **или** `choice[]` (см. §2).
4. Wiki-страница в `docs/wiki/player/missions/` + строка в `index.html`
   (**без wiki задача не закрыта** — «урок D1», см. §4).

**Синхронный vs отложенный `on_*`:** request / MESSAGE / PRICE / DEMAND — chain в том же
`process_event`. `EVENT_TYPE_INVASION` должен ждать исход боя → **B2**; до него JS poll.
**Не снимать** `mission_pharaoh_favour_invasion_tick` / invasion resolve в миссиях до
Phase 7 плана B2.

Запросы Фараона — паттерн с реп-цепочкой (или shared `ONLY_VIA` leaf, см. triage):
```js
var request = city.create_good_request({ tag_id: N, resource: RESOURCE_X, amount: A, months_initial: M })
city.create_chain_event({ tag_id: N01, type: EVENT_TYPE_REPUTATION_INCREASE, amount: 8 })
city.create_chain_event({ tag_id: N02, type: EVENT_TYPE_REPUTATION_DECREASE, amount: 6 })
request.set_completed_action_tag(N01); request.set_refusal_action_tag(N02); request.execute()
```
Вторжения (пока B2): `city.start_foreign_army_invasion({…})` + poll formations / destroy-goal.
Distant battle — `city.create_distant_battle({ tag_id, city })` (+ `set_param`, `set_reasons`, `execute`).

---

## 2. Грабли прогрессии (подтверждены, важно не повторить)

- **`next_mission` fall-through.** Без явного `next_mission` движок берёт `completed_id+1`
  (`missions.js:104`). Для миссии-схода пары это баг: Buhen(13) без `next_mission:15` ушёл бы
  в South Dahshur(14) — соседнюю ветку. **Всегда ставить `next_mission` явно** на обеих
  ветках, сходящихся в одну миссию.
- **`choice[]` ставится на миссии ПЕРЕД развилкой**, не на самих ветках. Цепочка:
  10 Saqqara→choice(11/12); 11/12→choice(13/14); 13/14→`next_mission`15; 15→choice(16/17);
  16/17→`next_mission`18; 18→choice(19/20)…
- **`choice[]` НЕ валидирует цель** (в отличие от `next_mission`, который проходит
  `__game_mission_is_valid`). Выбор незаскриптованной миссии = тупик/неопределённое поведение.
  → **задача B5** ✅ закрыта: `mission_is_playable(id)` (is_valid И наличие JS-конфига)
  фильтрует неиграбельные пункты и завершает кампанию штатно. Тупик 18→19/20 закрыт.
- **Choice-хост через `next_mission` пропускался.** Если у миссии есть `choice[]` и в неё
  заходят по линейному `next_mission`, движок показывал её выбор вместо загрузки самой
  миссии (поле `after` нигде не проставлено), пропуская Saqqara (8/9→10), North Dahshur
  (13/14→15) и др. → **задача B6** ✅ закрыта (вариант B): развилка активна только при
  `host == completed`, иначе хост грузится и играется.

---

## 3. Кросс-задачные находки (детали и план — в REMAKE_TASKS_P1.md)

| Находка | Суть | Задача |
|---------|------|--------|
| Формула рейтинга монументов | ✅ (частично) Заменена на аддитивную `2.25·Σ+4.5` (`city/monuments.js`): 1 мастаба=9, 3 мастабы=18, средняя ступенчатая=40 (≥ Saqqara-19). Форма формулы исправлена; **точная калибровка весов по типам ещё нужна** (веса-плейсхолдеры). | **F3** |
| Баг данных `enemies.js` | ✅ `percentage_type3>0` при `figure_types[2]=FIGURE_NONE` у 5 наций — доля армии не спавнилась (миссии 11/13/15/16). Исправлено: доля сложена в type2, суммы=100. Assyrian/Hyksos chariot — под F2. | **F1** |
| Вражеские колесницы | ✅ `figure_enemy_chariot` + METAINFO на все 12 `FIGURE_ENEMY_*_CHARIOT` (наследуют fast_sword). Ассирийцы/гиксосы спавнят колесницы; миссии 32/33 разблокированы. Тест 39. | **F2** |
| Валидация `choice[]` | ✅ см. §2 — `mission_is_playable` фильтрует незаскриптованные цели. | **B5** |
| Миссия 11 vs оригинал | ~~мёртвые торговые записи (Men-nefer/Kebet), Bedouin через `ENEMY_0_BARBARIAN`~~ → **D1b ✅** (pak dump; Libyan; Nekhen). | **D1b** |

Пробелы движка, всплывшие при скриптовании (пока без отдельных задач — завести при необходимости):
- **Нет вражеского флота (E3)** → «морские» вторжения (кушиты в Iunet, нубийский флот)
  скриптуются наземным прокси. `TODO(E3)` в `m_016`.
- **Нет ресурса `RESOURCE_IVORY`** — слоновая кость из брифингов Iunet/On заменена
  на `RESOURCE_LUXURY_GOODS`.
- **Нет JS-API «открыть торговый маршрут по ходу миссии»** — в оригинале маршруты
  (Serabit Khadim в Meidum, возможно Byblos) открываются mid-mission; сейчас открыты со старта.
- **`message_mission_*` для 16–18 идут под классическими именами** (Dendera/Heliopolis/Giza),
  не под именами кампании. При скриптовании 19+ проверять фактический ключ в `game_messages_en.js`.

---

## 4. Процессные уроки

- **Wiki — часть задачи.** Коммит `fa91fa687` закрыл D1 без wiki-страницы; недостача
  всплыла только при ревизии. При приёмке любой D-задачи проверять wiki наравне со скриптом.
  То же для зданий блока C: страница в `docs/wiki/player/buildings/` + ссылки с overview /
  каталога / затронутых миссий — в том же PR, что и код (урок C6/C7).
- **Данные из walkthrough ≠ данные из `.pak`.** Всё, что взято из Pharaoh Heaven, помечать
  комментарием в скрипте и в spoiler «Known deviations» на wiki. `.pak` за сессию не сверялся ни разу.
- **Рантайм не запускался** (нет `mission1.pak`) — вся проверка структурная (grep enum'ов,
  сверка с закоммиченными миссиями). Перед мержем нужен прогон `--mixed`.
- **Amounts монументов — в JS, не в C++.** Pre-stock (`placement_resources`), timber/loads
  (`timber_loads` / фазы через `archive_load`) правятся конфигом здания. C++ читает
  `current_params()`; калибровка vs `.pak` = правка JS (F3), без пересборки логики.
- **Staffed Storage Yard.** Если оригинал требует «гранит/камень уже на складе» — считать
  и списывать только из SY с `num_workers > 0` (`yards_stored_staffed` /
  `staffed_only` на remove). Пустой двор не считается.
- **Миссия + новый монумент:** целевой тип в `buildings[]`, снять stand-in/`TODO(C*)`,
  на старте `__image_request_pak` для delayed-паков здания.
- **Integral tests + скриншоты (урок C6/C7):**
  - `test_find_buildable_tile` тянет к **центру** — вспомогательные здания (SY) ставить
    с краю, тестируемый монумент в центр (`cx - footprint/2`), иначе full-city скрин
    «съезжает»; под `--no-resource` угловые тайлы могут отказать → fallback `-1,-1`.
  - Finished для скрина: `set_phase(phases())` → `MONUMENT_FINISHED`; `set_phase(art_stages)`
    ещё не finished (леса/лестница остаются).
  - **`--no-resource`:** `graphics_save_screenshot` делает early-return (`Screenshot skipped`) —
    билдферма без Pharaoh data не должна писать PNG. Маркеры `*_screenshot_done` в JS
    можно оставлять после вызова.
  - Доставка в монумент без телеги: `__test_monument_add_resource` → `deliver_resource`.
  - Прогон со скринами: без `--no-resource`, `--screenshot-dir PATH`, data dir в конце
    (см. `tests/README.md`, пример 43/44).

---

## 5. Долг временных целей монументов (вернуть при реализации C-задач)

Из-за нереализованных монументов (C1/C3/C7; **C4/C6 код готов**, цели миссий ещё
не возвращены где нужен C3) и формулы (F3) часть миссий используют временные
цели/замены. При закрытии C-задачи — вернуть оригинал (шаги в C1/C3e/C4/C6/C7 и F3).

Значения «Сейчас (врем.)» пересчитаны под новую **аддитивную** формулу рейтинга
`2.25·Σ+4.5` (см. F3 ниже). Веса типов монументов пока прежние (не откалиброваны по
оригиналу) → стендин-цели ушли вверх; это ожидаемо до калибровки весов.

| Миссия | Оригинал | Сейчас (врем.) | Замена монумента | Вернуть в |
|--------|----------|----------------|------------------|-----------|
| m_012 Meidum | mon 39 | mon 58 | complex → малая+средняя ступенчатые | **C1** |
| m_013 Buhen | mon 9 | mon 9 | small obelisk (C7 каркас) | — |
| ~~m_014 South Dahshur~~ | mon 21 | **mon 21 ✅** | (bent pyramid реализован — C4) | — (закрыто) |
| m_015 North Dahshur | mon 32 | mon 58 | true → малая+средняя ступенчатые | **C3** (C3e) |
| m_017 On | mon 18 | **mon 18 ✅** | (мастабы те же; формула исправлена — F3) | — (закрыто) |
| m_018 Rostja | mon 53 | mon 67 | sphinx+complex+pyramid → ступенчатые+мастабы | **C3+C6** (C6 код ✅; цель 53 — с C3) |

Маркер в коде: искать `TODO(C1)`, `TODO(C3)`, `TODO(C6)`, `TODO(E3)`
по `src/scripts/mission/`. (`TODO(C4)` / `TODO(C7)` у миссий сняты.)

---

## 6. Рекомендация по порядку

Долг из §5 растёт с каждой новой D-миссией. Хотфиксы волны 3 закрыты:
1. ~~**F3**~~ — форма формулы исправлена (аддитивная `2.25·Σ+4.5`); миссия 17 On
   вернулась к оригиналу 18. Осталась **калибровка весов по типам** (нужны данные
   оригинала) — держать под тем же ID F3;
2. ~~**B5**~~ ✅ — тупики выбора убраны (`mission_is_playable`);
3. ~~**F1**~~ ✅ — армии вторжений 11/13/15/16 спавнятся полностью.

Осталось из инфраструктуры кампании: **B2** (event-invasions), **B3** (invasion
warnings), **B4** (phrase_id). Дальше — блок C (монументы) и миссии D6+ / Abu(9).

**B2 — план:** [`REMAKE_B2_INVASION_PLAN.md`](REMAKE_B2_INVASION_PLAN.md)
(spawn + **отложенный** `on_completed`/`on_refusal`, favour, chain-only, tests).
Кратко в `REMAKE_TASKS_P1.md` § B2. Пока не сделан — миссии 5–8 на JS poll /
`mission_pharaoh_favour_invasion_tick`.

**B2 — расшифровка pak (2026-07-24, m5–7):** крупные amount=40/45 — это
`EVENT_TRIGGER_BY_FAVOUR` (0x10) + `EVENT_INVADER_PHARAOH` (не distant battle).
Dump: `semantics=favour_kr_punishment`. Selima favour size **63**.

**JS proxy:** `mission_pharaoh_favour_invasion_tick(mission, size[, chain])` в
`missions.js`. Параллельно может жить Caesar-legacy
`kingdome_relation_t::process_invasion` — при B2b согласовать, иначе двойной спавн.

**Закрыто рядом с Selima (2026-07-25):** `EVENT_TYPE_PRICE_↑/↓` → `trade_price_change`;
meta `debt_interest` / `initial_funds` / … как `int_dcy`; empire route `deviation`
в `improve_route`; NEW_TRADE выставляет `is_open`.

---

## 7. Справочник ID сообщений (миссии 10–18)

`game_messages_en.js`: history-панель `message_history_<city>` (id 210–218),
брифинг-попап `message_mission_<name>` (id 410–418). Для 16–18 `<name>` — классическое имя:

| # | Город | history id | mission-попап (ключ / id) |
|---|-------|-----------|----------------------------|
| 11 | Serabit Khadim | 211 | serabit_khadim / 411 |
| 12 | Meidum | 212 | meidum / 412 |
| 13 | Buhen | 213 | buhen / 413 |
| 14 | South Dahshur | 214 | south_dahshur / 414 |
| 15 | North Dahshur | 215 | north_dahshur / 415 |
| 16 | Iunet | 216 | **dendera** / 416 |
| 17 | On | 217 | **heliopolis** / 417 |
| 18 | Rostja | 218 | **giza** / 418 |

---

## 8. Storage orders / Empty All (склад и амбар)

Выявлено при фиксе [#608](https://github.com/dalerank/Akhenaten/issues/608)
(`building_storage_toggle_empty_all`). Код: `src/building/building_storage.cpp`,
UI: `ui_storage_yard_orders_window.js` / `ui_granary_orders_window.js`.

**Текущее поведение (после #608):**
- START emptying → флаг `empty_all = 1`, всем товарам `STORAGE_STATE_EMPTY`;
- STOP emptying → флаг `0`, всем товарам `STORAGE_STATE_ACCEPT`
  (кастомные Get/Refuse **теряются** — осознанный упрощённый фикс).

**Два разных механизма (не путать):**
1. флаг `storage_t::empty_all` — кнопка START/STOP; воркеры смотрят `is_empty_all()`;
2. per-resource `STORAGE_STATE_EMPTY` — заказ «Empty» у одного товара; воркеры
   смотрят `is_emptying(r)`. При Empty All оба включены сразу.

Julius только крутит флаг и **не** трогает `resource_state`. У Akhenaten своя
модель с четырьмя стейтами (Accept / Refuse / Get / Empty).

**Грабли:**
- `building_storage_clear_all` / `create` должны memset’ить весь `city_storage_t`,
  не `sizeof(storage_t)` (иначе хвост слота не очищается).
- Пока `empty_all` включён, ручной cycle одного товара или Accept None разъезжаются
  с кнопкой STOP — см. задачи **S1 / S2** в `REMAKE_TODO.md`.
- Info-окно склада/амбара заказы не показывает; Empty в orders — `__loc(99, 21)`.

---

## 9. Terrain-feature grids (уроки Low Bridge)

Эталон: мосты — [REMAKE_BRIDGE_PLAN.md](REMAKE_BRIDGE_PLAN.md), код `grid/bridge_grid.*`,
конфиг `src/scripts/building/bridge.js`. Задачи: **UB1**, **BR1–BR5**, **TG1**, **PC4**
в `REMAKE_TODO.md`.

Когда фича на карте **не** building instance, а данные на тайле (мост, похожие
оригинальные «орнаменты»):

1. **Не делать building ради удобства** — ломает save compat с Pharaoh.
2. **Свои grids** для runtime-истины; если оригинал читает общий chunk
   (`sprite_grid` и т.п.) — **dual-write** туда при place/remove/rotate.
3. **Migrate-on-load** для старых сейвов / оригинала: один проход после unserialize,
   если новый grid пуст, а legacy-данные на месте.
4. **Новый grid в `.svx`:** bump `latest_save_version` + `push_chunk` размера
   `228² × sizeof` (`u8` → 51984, `u16` → 103968); clear в pre_load, backup/restore в undo.
5. **Новый `.cpp`:** GLOB без `CONFIGURE_DEPENDS` → нужен `cmake --preset …` перед build.
6. **Конфиг здания/стилей** — отдельный `building/<name>.js` + `import` в `buildings.js`,
   не раздувать `config.js`. Стили: явный `index` + текстовый `type` для lookup.
7. **Integral test** как контракт place / reject / routing (+ чтение нового grid),
   иначе регресс «снова только sprite» не ловится. Минимум на новый grid: place →
   save → load → assert (задача **TG1** / **BR4** в `REMAKE_TODO.md`).
8. **PR:** не смешивать terrain-feature / grids с mission-empire скриптами (**PC4**).
9. **Build:** новый `.cpp` → reconfigure перед build (**PC4**, пункт 5 выше).

---

## 10. MuJS value stack и hot-reload (уроки 2026-07-25)

Задачи в `REMAKE_TODO.md`: ~~**HR1**~~ ✅ ~~**HR5**~~ ✅ ~~**HR6**~~ ✅ ~~**HR7**~~ ✅
~~**QA4**~~ ✅; открыты **HR2**, **HR3**, **HR4**, **PC5**.
Регрессия: `tests/47_js_hotreload_stack.js`.

**Симптом:** при `--mixed` после ~10 сохранений любого скрипта — abort
`Fatal program exit` / `stackoverflow` в `js_throw`, стек C++ в
`config::refresh` → UI `archive::r_int` / `getproperty`. Кажется «баг в UI JS»,
на деле стек MuJS уже почти полный.

**Triage: «сделай X много раз».** Если repro — N повторов (сохранений, кликов, reload),
сразу смотреть накопители (value stack, registry refs, event handler lists), а не только
UI-дерево в точке падения dump’а.

**Факты движка:**
- `JS_STACKSIZE` = 256 (`src/mujs/jsi.h`) — общий value stack на VM.
- `js_gettop(J)` = `TOP − BOT`, не абсолютный `TOP`. Мерять утечку удобно внутри
  одного C-вызова (BOT фиксирован) или на `frame_end` при `BOT == 0`.
- `js_vm_sync` на каждый reload: exec файла → `js_register_game_handlers` →
  `js_register_entity_systems` → `config::refresh` (все autoconfig windows).
- Лог sync (**HR7**): `JS: vm_sync done files=N (…) refresh=full elapsed_ms=…`.

**Что текло (HR1):**
1. `js_register_console_command_from_function` — `js_getglobal` без `pop`
   (~30 `[console_command=…]` × reload ≈ порог за ~8–10 сохранений).
2. `js_call_function` / `js_call_function_bool` — не снимали результат `pcall` на success.
3. `js_vm_exec_function_args` — лишний `js_pop(2)` после `trypcall` (ломал baseline).

**Правила (stack hygiene):**
- Каждый `getglobal` / `getregistry` / `getproperty` / `pcall` имеет парный `pop`
  (или restore к сохранённому baseline).
- Safety-net в `js_vm_sync` чистит только прирост относительно entry `js_gettop`,
  не «pop to zero» — иначе nested JS→C сносит args текущего фрейма.
- Регрессия: N≫порога (20) итераций handlers-only **и** полного reload файла с
  modifiers; assert `after == baseline`. Safety-net не должен маскировать handlers-only.
- Integral suite (**HR5**): перед тестом сброс stale (`cleared N stale…`); после теста
  leftover → FAIL. Stale со старта VM (часто 2 слота) не должен валить тест.

**Canary (QA4 ✅):** таблица в `tests/README.md` — 36 (xstring), 38 (color), 47 (stack).
Новый класс краша → новый `tests/NN_*.js` до мержа MuJS-волны.

**Приоритет остатка:** **HR2** (аудит pcall) ≫ **HR3** (диагностика) ≫ **HR4** (dirty
refresh). Infra/dev отдельно от mission parity — **PC5**.
