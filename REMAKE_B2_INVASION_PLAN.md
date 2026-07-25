# B2 — EVENT_TYPE_INVASION + отложенный on_completed

**Статус:** Phase 1–2 в коде (2026-07-25); favour/save/migrate ещё нет.  
**Трекер:** `REMAKE_TASKS_P1.md` § B2 · `REMAKE_TODO.md` § B2 · handoff `MISSION_TO_JS_HANDOFF.md`.  
**Очередь с empire:** [`REMAKE_EMPIRE_MISSIONS_PLAN.md`](REMAKE_EMPIRE_MISSIONS_PLAN.md)
(B2 → **B2-migrate** m5–9 → снова A; B3/B4 после).

План инфраструктуры кампании. Не точечный фикс под Selima: миссии 5–9 уже
закрывают timed/favour poll’ом в JS. Цель — чтобы **pak / editor / JS chain**
вторжения жили в event manager с честным spawn и исходом.

Связанные документы:

- Кратко в трекере: `REMAKE_TASKS_P1.md` § B2 (B2a–d)
- Контекст favour: `REMAKE_NOTES.md` §6
- JS proxy: `src/scripts/missions.js` → `mission_pharaoh_favour_invasion_tick`
- Примеры poll: `m_006_behdet.js` (y15 wipe → KR+8, ok-only),
  `m_008_selima.js` (Hyksos seen/wipe/destroy-goal → KR / siege / troops)
- Post-B2 cleanup: **B2-migrate** в `REMAKE_EMPIRE_MISSIONS_PLAN.md` (явный шаг)

**Вне скоупа B2:** distant battle (отдельный тип), B3 (сериализация warnings), B4 (phrase_id).  
Selima request/PRICE/debt/routes — **не** часть B2 (уже закрыты отдельно).  
**Сразу после B2 Phase 7:** B2-migrate — снять/сузить JS poll в m5–9 (+10+), не откладывать.

---

## 1. Проблема

| Слой | Сейчас |
|------|--------|
| `case EVENT_TYPE_INVASION` | пустой `// TODO` |
| После любого handler | `chain_action_next` default = `COMPLETED` → сразу `process_event(on_completed_action, …)` |
| Спавн | работает (`scenario_invasion.cpp`, `start_foreign_army_invasion`) |
| Миссии 5–9 (+10 favour) | календарь + poll в JS; favour через helper |

Для invasion исход (wipe / destroy-goal) известен **позже**. Синхронный
`on_completed` ломает pak-цепочки вроде:

```
invasion ok → REPUTATION_INCREASE → CITY_UNDER_SIEGE → REQUEST troops
invasion refuse → REPUTATION_DECREASE → REQUEST troops
```

Тот же default `COMPLETED` стоит и у `EVENT_TYPE_REQUEST` при activate — смежный
риск (см. §9); в B2 чинить обязательно для INVASION, REQUEST — по возможности рядом.

---

## 2. Цель

1. Event `EVENT_TYPE_INVASION` **спавнит** армию из полей pak/JS.
2. Цепочки **не** стреляют в момент спавна.
3. После разрешения боя движок вызывает `on_completed` / `on_refusal` / `on_defeat`
   (как request, но отложенно).
4. Favour-KR и chain-only работают без JS-proxy (proxy снимается в Phase 7).
5. Консоль `start_invasion` / существующие JS spawn API не ломаются.
6. До Phase 7 **нет dual-spawn**: engine не спавнит ту же волну, что ещё ведёт JS proxy.

---

## 3. Архитектура (ядро)

### 3.1 Два этапа lifecycle

```
[calendar / favour / parent chain]
        │
        ▼
 process_event(INVASION)   ← только если миссия не на JS invasion-proxy
        │
        ├─ выделить invasion_id (слот < MAX_ENEMY_ARMIES=120), НЕ = event_id
        ├─ spawn army (scenario_invasion)
        ├─ pending: invasion_id ↔ event_id (+ enemies_seen)
        ├─ chain_action_next = NONE   ← не propagate сразу
        └─ trigger: ONCE→ALREADY_FIRED; BY_FAVOUR→BY_FAVOUR_IN_USE (явно!);
           RECURRING: держать pending/is_active, не второй spawn до resolve
        │
        … армия на карте …
        │
        ▼
 invasion_resolve_tick (месяц/день **event manager**, не scenario_invasion_process)
        │
        ├─ wipe, destroy-goal не выполнен → EVENT_ACTION_COMPLETED
        ├─ destroy-goal выполнен / «провал» → EVENT_ACTION_REFUSED или DEFEAT
        └─ process_event(on_*_action, via=true, parent=event_id)
```

**Эталон полного resolve** — Selima (`mission8_resolve_hyksos_invasion`):

- `city.num_enemy_formations` / seen flag (армия успела появиться)
- `city.enemy_army_achieved_destroy_goal(invasion_id)` → refuse path
- иначе wipe → ok path

**Behdet y15** — урезанный ok-only (wipe → KR+8, без refuse/destroy-goal check).
Не смешивать с эталоном Selima×22.

### 3.2 Связь invasion ↔ event (pending registry)

Отдельная таблица (не «event_id как invasion_id»):

| Поле | Смысл |
|------|--------|
| `invasion_id` | слот `enemy_army` / `invasion_opts_t.invasion_id` (**0…119**, uint8) |
| `event_id` | индекс `event_ph_t` в scenario events |
| `pending` | ждёт resolve |
| `enemies_seen` | как JS seen-flag |
| (опц.) `want_destroy` | снимок цели на момент spawn |

`enemy_army` / formation хранят `invasion_id` как `uint8_t`, массив
`MAX_ENEMY_ARMIES = 120`. Индекс события в pak часто ≥15 и легко ≥120 —
**нельзя** писать `opts.invasion_id = event.event_id`. Аллоцировать свободный
слот (или стабильный малый id из pending allocator); связь только через registry.

**Save:** либо расширить существующий invasion chunk (рядом с B3), либо отдельный
маленький chunk + bump `latest_save_version`. Без сериализации mid-fight load
потеряет chain — для `.svx` это блокер приёмки.

### 3.3 Не стрелять on_completed синхронно

```cpp
case EVENT_TYPE_INVASION:
    process_event_invasion(...); // spawn + register pending
    chain_action_next = EVENT_ACTION_NONE;
    break;
```

`EVENT_ACTION_NONE` уже в enum — `switch (chain_action_next)` его игнорирует
(нет `default`; только COMPLETED/REFUSED/TOOLATE/DEFEAT).

Помечать event **после spawn**, не после resolve:

| Trigger до | После spawn |
|------------|-------------|
| `ONCE` / `ACTIVATED_*` | `ALREADY_FIRED` (уже делает хвост `process_event`) |
| `BY_FAVOUR` (0x10) | **явно** `BY_FAVOUR_IN_USE` (0x14) — хвост ONCE-логики на favour **не** срабатывает |
| `RECURRING` | не ALREADY_FIRED; блок повторного входа пока `pending` |

### 3.4 Dual-spawn gate (обязателен с Phase 1)

Пока миссия на JS proxy (`start_foreign_army_invasion` + month poll), engine
**не** должен спавнить те же волны.

Варианты (выбрать один в Phase 1):

1. **Mission flag / config:** `use_native_invasion_events: false` в mission JS до migrate.
2. **Stub missions only:** Phase 1–2 гонять на тестовой карте / mission stub без proxy;
   live m5–9 не подключать handler’ом до Phase 7 per-mission.
3. **Per-event:** JS не создаёт timed/favour invasion events, только poll — тогда
   engine может читать pak; сейчас миссии **сами** спавнят, поэтому (1) или (2).

Favour: кроме JS helper — ещё Caesar-legacy `kingdome_relation_t::process_invasion`
(§ B2b). Три источника → оставить один.

### 3.5 Chain-only clone (уже почти есть)

`ONLY_VIA` + не-REQUEST уже идёт в `create(..., EVENT_TRIGGER_ACTIVATED_12)`
(`scenario_event_manager.cpp`). Для INVASION **clone path есть**; дыра —
пустой handler + sync `COMPLETED`. B2c = убедиться, что ACTIVATED child
проходит B2a spawn+pending, а не «добавить clone с нуля».

---

## 4. Подзадачи

### B2a — Timed enemy / bedouin / egypt spawn

**Файлы:** `scenario_event_manager.cpp/.h`, `scenario_invasion.cpp/.h`

1. Маппинг `e_event_invader` → mode / enemy (см. §5); dual-spawn gate.
2. Прочитать `amount` (size), `invasion_attack_target` →
   `formation_attack_from_event_target`, `location_fields` / invasion point,
   **`want_destroy`** (см. §5 / Phase 0).
3. Аллоцировать `invasion_id` < 120; `scenario_invasion_start` /
   `scenario_start_invasion_impl`.
4. Pending registry; `chain_action_next = NONE`.
5. Триггеры: `ONCE`, `RECURRING` (+ pending guard).

**Приёмка:** на stub/тесте без JS proxy — spawn; `on_completed` **не** в тот же тик;
live m5–9 без dual army.

### B2-resolve — Отложенный исход (критический кусок)

1. Хук: **месяц/день тик event manager** (рядом с `process_active_request`).
   **Не** `scenario_invasion_process` — тот обслуживает classic
   `g_scenario.invasions[]` warnings, не event-manager волны.
2. Условия как Selima resolve (seen / wipe / destroy-goal). Behdet-style ok-only —
   когда в pak `on_refusal` / `on_defeat` пустые.
3. Fire `on_*_action` через `process_event(..., via_event_trigger=true, caller_event_id)`.
4. Снять pending; не double-fire.

**Семантика исходов (зафиксировать Phase 0 по pak dump m5–9):**

| Исход | Action | Пример |
|-------|--------|--------|
| Wipe, destroy-goal нет | `COMPLETED` | Selima Hyksos×22 → +2 → siege → troops×4 |
| Destroy-goal выполнен, армия ушла | `REFUSED` (или DEFEAT) | Selima×22 → −2 → troops×4 |
| Wipe, refuse/defeat в pak = −1 | `COMPLETED` only | Behdet y15 → +8 |
| (если есть в pak) timeout / special | `TOOLATE` / `DEFEAT` | уточнить dump’ом |

Перед кодом — сверить 1–2 pak event’а с ненулевыми `on_refusal` / `on_defeat`.

### B2b — Favour Kingdom Rating

1. Отдельный тик: `EVENT_TRIGGER_BY_FAVOUR`, если `rating_kingdom <= 0`
   (как JS helper) → `process_event` по правилам pak.
2. После spawn: **явно** `BY_FAVOUR_IN_USE` (0x14); не полагаться на ONCE-хвост.
3. Отключить Caesar-legacy `kingdome_relation_t::process_invasion` + JS helper
   на миссиях с native favour (иначе 2–3 спавна).
4. Размеры из `event.amount` (не хардкод): Timna/Behdet 45, Abedju 40, Selima 63,
   Abu 40 (+ chain 20, 20), Saqqara 69.

**Приёмка:** KR→0 → одна Pharaoh army нужного size; chain child (B2c) если в pak.

### B2c — Chain-only invasions

1. Подтвердить: `ONLY_VIA` INVASION → `ACTIVATED_12` (уже else-ветка; REQUEST → `ACTIVATED_8`).
2. Activated child → B2a spawn + pending (не sync COMPLETED).
3. Родитель: request refusal / invasion completed / favour ok / etc.

**Приёмка:** parent `on_refusal` → child invasion без календарной даты (Abu favour chains).

### B2d — Тесты

| Тест | Что проверяет |
|------|----------------|
| Integral: timed spawn | event ONCE → formations > 0; нет sync chain |
| Integral: resolve ok | wipe → child REPUTATION/tag fired |
| Integral: resolve refuse | destroy-goal → refusal chain |
| Integral: favour | KR=0 → Pharaoh army once |
| Integral: no dual-spawn | proxy mission + engine off / stub only |
| Console | `start_invasion` ок |
| Dump | `__test_mission_pak_dump` → `semantics=favour_kr_punishment` для 0x10 |
| Bounds | `invasion_id` всегда < 120 |

Файлы: `tests/NN_invasion_event_*.js` + при необходимости C++ helpers в
`js_test_*.cpp`.

---

## 5. Маппинг полей event → invasion_opts

| `event_ph_t` | `invasion_opts_t` / spawn |
|--------------|---------------------------|
| `item` (`e_event_invader`) | mode + enemy_type (§5.1) |
| `amount` | size |
| `invasion_attack_target` | `formation_attack_from_event_target` |
| `location_fields` / point | `invasion_point` |
| **pending allocator** | `invasion_id` (слот < 120) — **не** event_id/tag |
| `want_destroy` | см. Phase 0 (§5.2) |
| `months_initial` | скорее warning duration → **B3**; spawn UI может игнорировать сначала |

### 5.1 Invader mapping

Сверить с JS / dump; черновик:

| Invader | mode | enemy |
|----------|------|--------|
| `ENEMY` | `ATTACK_TYPE_ENEMIES` | `g_scenario.enemy_id` (миссия: Hyksos/Kushite/…) |
| `EGYPT` | уточнить Phase 0 (не путать с Pharaoh) | обычно Egyptian / scenario |
| `PHARAOH` | `ATTACK_TYPE_KINGDOME` | `ENEMY_3_EGYPTIAN` (как favour helper) |
| `BEDUINS` | `ATTACK_TYPE_ENEMIES` (не native blindly) | часто **`g_scenario.enemy_id`**: Timna pak=BEDUINS, sprites=`ENEMY_7_LIBIAN` (`m_005_timna.js`) |

### 5.2 want_destroy (Phase 0 — эвристика)

В `event_ph_t` **нет** явного поля. Engine (B2a) пишет:
- `ENEMY` / `BEDUINS` / `EGYPT` → `want_destroy = amount` (как JS missions)
- `PHARAOH` → `0`

`scenario_start_invasion_impl` применяет `opts.want_destroy` → `enemy_army.buildings_to_destroy`
(раньше поле игнорировалось — destroy-goal в JS был мёртвым).

`months_initial` у favour часто `9` — warning / B3, не want_destroy.

---

## 6. Порядок внедрения

```
Phase 0  Спека: исходы ok/refuse/defeat; want_destroy; EGYPT vs PHARAOH; dump m5–9
    │
Phase 1  B2a: spawn + NONE + pending allocator + dual-spawn gate (stub / flag)
    │
Phase 2  B2-resolve: event-manager tick + fire on_completed/refusal
    │
Phase 3  Save pending (или совместить с B3)
    │
Phase 4  B2c: ACTIVATED child → B2a path (clone уже есть)
    │
Phase 5  B2b: favour tick + BY_FAVOUR_IN_USE + выключить legacy/JS dual
    │
Phase 6  B2d tests
    │
Phase 7  B2-migrate: снять JS poll/favour в m5–9 (+10 по готовности), по одной миссии
```

**Не** начинать Phase 7 до стабильного resolve и dual-spawn gate.

Рекомендуемый первый PR: Phase 0 заметки + Phase 1–2 на **stub** + минимальный
integral (spawn + ok chain). Favour/save — вторым PR. Live m5–9 — только с Phase 7.

### B2.5 (опционально, до/параллельно Phase 1)

Общий JS helper `mission_resolve_invasion(mission, opts)` в `missions.js` —
вынести копипасту Selima poll (seen / wipe / destroy-goal → fire tags).
Behdet ok-only может звать тот же helper с `refuse_tag=0`.
Не блокирует B2a; упрощает Phase 7.

---

## 7. Миграция миссий (Phase 7 / B2-migrate)

| Миссия | Сейчас | После B2 |
|--------|--------|----------|
| Timna (5) | JS raids + favour 45 | pak/native events; BEDUINS→Libian via enemy_id |
| Behdet (6) | JS + wipe poll y15 → +8 | event on_completed → +8 (ok-only) |
| Abedju (7) | JS + favour 40 + chain 40 | B2b+c |
| Selima (8) | JS Hyksos poll → siege/troops | event chains; luxury-late troops уже ONLY_VIA |
| Abu (9) | favour 40 → chain 20 → chain2 20 | B2b+c; снять custom chain2 poll |
| Saqqara (10+) | favour 69 (по мере redefine) | B2b; не блокирует m5–9 |

Пока engine не готов / gate off — **не удалять** JS proxy. После B2d — по одной
миссии PR, сверка с dump; favour helper убирать только когда B2b закрыт для
мигрируемых миссий. На мигрированной миссии выключить proxy **в том же PR**,
что включает native (иначе dual-spawn).

---

## 8. Файлы (ожидаемые касания)

| Файл | Роль |
|------|------|
| `src/scenario/scenario_event_manager.h/.cpp` | handler INVASION; NONE; favour tick; resolve tick |
| `src/scenario/scenario_invasion.h/.cpp` | spawn opts; pending registry/allocator |
| `src/city/city_kingdome_relations.cpp` | отключить/согласовать legacy invasion |
| `src/figure/enemy_army.*` | границы invasion_id / слоты |
| `src/scripts/missions.js` | Phase 7: убрать favour helper; опц. B2.5 resolve helper |
| `src/scripts/mission/m_005…m_009_*.js` (+10) | Phase 7 миграция + gate flag |
| `src/io/gamestate/boilerplate.cpp` / chunks | save pending (Phase 3) |
| `tests/…` | B2d |
| `REMAKE_TASKS_P1.md` | отметить ✅ по подзадачам |

---

## 9. Риски

| Риск | Митигация |
|------|-----------|
| **Dual-spawn** engine + JS (+ legacy favour) | gate с Phase 1; Phase 7 снимает proxy в том же PR; выключить legacy в B2b |
| `invasion_id = event_id` → OOB (≥120) | pending allocator, слоты < `MAX_ENEMY_ARMIES` |
| Нет поля want_destroy в event | Phase 0: найти или зафиксировать эвристику |
| Неверная семантика REFUSED vs DEFEAT | Phase 0 dump; Selima×22 vs Behdet ok-only |
| Save mid-invasion | Phase 3 обязателен для `.svx` |
| `BY_FAVOUR` не становится ALREADY_FIRED сам | явный `BY_FAVOUR_IN_USE` после spawn |
| RECURRING + pending | не второй spawn до resolve |
| REQUEST activate тоже default COMPLETED | смежно; по возможности `NONE` как у invasion |
| Регрессия Selima troops×4 | JS until Phase 7; integral на Hyksos×22 graph |
| `scenario_invasion_process` как resolve hook | не использовать; только event-manager tick |

---

## 10. Критерии готовности (Definition of Done)

- [ ] Pak/editor `EVENT_TYPE_INVASION` спавнит армию без JS (на stub / после migrate).
- [ ] `on_completed` / `on_refusal` стреляют **после** исхода, не в тик спавна.
- [ ] `invasion_id` всегда < 120; pending registry переживает mid-fight (или documented defer).
- [ ] want_destroy: поле найдено **или** эвристика задокументирована + тест refuse.
- [ ] Favour 0x10: одна армия при KR collapse; `BY_FAVOUR_IN_USE`; dump semantics ок.
- [ ] Chain-only child invasion работает (Abu-style).
- [ ] Dual-spawn gate: live proxy-миссии не получают вторую армию от engine.
- [ ] `start_invasion` console ок.
- [ ] Integral tests зелёные.
- [ ] `REMAKE_TASKS_P1.md` B2a–d обновлены; m5–9 на proxy до Phase 7 per mission.

---

## 11. Оценка (грубо)

| Фаза | Объём |
|------|--------|
| Phase 0 dump/спека (исходы + want_destroy + invader) | S–M |
| Phase 1–2 spawn+resolve + gate | M–L |
| Phase 3 save | S–M |
| Phase 4–5 chain+favour | M |
| Phase 6 tests | S–M |
| Phase 7 per mission (m5–9) | S each |

Эпик именно из‑за **отложенного resolve + save + favour + dual-spawn gate**, не из‑за
вызова spawn.
