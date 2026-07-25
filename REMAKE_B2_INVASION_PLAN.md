# B2 — EVENT_TYPE_INVASION + отложенный on_completed

**Статус:** план готов, код не начат (2026-07-25).  
**Трекер:** `REMAKE_TASKS_P1.md` § B2 · `REMAKE_TODO.md` § B2 · handoff `MISSION_TO_JS_HANDOFF.md`.

План инфраструктуры кампании. Не точечный фикс под Selima: миссии 5–8 уже
закрывают timed/favour poll’ом в JS. Цель — чтобы **pak / editor / JS chain**
вторжения жили в event manager с честным spawn и исходом.

Связанные документы:

- Кратко в трекере: `REMAKE_TASKS_P1.md` § B2 (B2a–d)
- Контекст favour: `REMAKE_NOTES.md` §6
- JS proxy: `src/scripts/missions.js` → `mission_pharaoh_favour_invasion_tick`
- Примеры poll: `m_006_behdet.js` (y15 → KR+8), `m_008_selima.js` (Hyksos → KR / siege / troops)

**Вне скоупа B2:** distant battle (отдельный тип), B3 (сериализация warnings), B4 (phrase_id).  
Selima request/PRICE/debt/routes — **не** часть B2 (уже закрыты отдельно).

---

## 1. Проблема

| Слой | Сейчас |
|------|--------|
| `case EVENT_TYPE_INVASION` | пустой `// TODO` |
| После любого handler | сразу `process_event(on_completed_action, …)` |
| Спавн | работает (`scenario_invasion.cpp`, `start_foreign_army_invasion`) |
| Миссии 5–8 | календарь + poll в JS; favour через helper |

Для invasion исход (wipe / destroy-goal) известен **позже**. Синхронный
`on_completed` ломает pak-цепочки вроде:

```
invasion ok → REPUTATION_INCREASE → CITY_UNDER_SIEGE → REQUEST troops
invasion refuse → REPUTATION_DECREASE → REQUEST troops
```

---

## 2. Цель

1. Event `EVENT_TYPE_INVASION` **спавнит** армию из полей pak/JS.
2. Цепочки **не** стреляют в момент спавна.
3. После разрешения боя движок вызывает `on_completed` / `on_refusal` / `on_defeat`
   (как request, но отложенно).
4. Favour-KR и chain-only работают без JS-proxy (proxy можно снять позже).
5. Консоль `start_invasion` / существующие JS spawn API не ломаются.

---

## 3. Архитектура (ядро)

### 3.1 Два этапа lifecycle

```
[calendar / favour / parent chain]
        │
        ▼
 process_event(INVASION)
        │
        ├─ spawn army (scenario_invasion)
        ├─ связать invasion_id ↔ event_id
        ├─ chain_action_next = NONE   ← не propagate сразу
        └─ (опц.) trigger → BY_FAVOUR_IN_USE / ALREADY_FIRED по правилам pak
        │
        … армия на карте …
        │
        ▼
 invasion_resolve_tick (месяц / день)
        │
        ├─ wipe, destroy-goal не выполнен → EVENT_ACTION_COMPLETED
        ├─ destroy-goal выполнен / «провал» → EVENT_ACTION_REFUSED или DEFEAT
        └─ process_event(on_*_action, via=true, parent=event_id)
```

Эталон логики исхода — текущий JS poll:

- `city.num_enemy_formations` / seen flag (армия успела появиться)
- `city.enemy_army_achieved_destroy_goal(invasion_id)` → refuse path
- иначе wipe → ok path

### 3.2 Связь invasion ↔ event

Нужна таблица (или поля на invasion/warning), переживает save:

| Поле | Смысл |
|------|--------|
| `invasion_id` | id волны (`invasion_opts_t.invasion_id`) |
| `event_id` | индекс `event_ph_t` в scenario events |
| `pending` | ждёт resolve |
| (опц.) `enemies_seen` | как JS seen-flag |

Минимум: при spawn записать `event_id` в структуру армии/warning; при clear —
найти pending event и fire chain.

**Save:** либо расширить существующий invasion chunk (рядом с B3), либо отдельный
маленький chunk + bump `latest_save_version`. Без сериализации mid-fight load
потеряет chain — для `.svx` это блокер приёмки.

### 3.3 Не стрелять on_completed синхронно

В `process_event` после `switch (type)`:

```cpp
case EVENT_TYPE_INVASION:
    process_event_invasion(...); // spawn + register pending
    chain_action_next = EVENT_ACTION_NONE;
    break;
```

`EVENT_ACTION_NONE` уже есть в enum — убедиться, что `switch (chain_action_next)`
его игнорирует (сейчас default отсутствует: только COMPLETED/REFUSED/TOOLATE/DEFEAT).

Помечать event `ALREADY_FIRED` / `BY_FAVOUR_IN_USE` **после spawn**, не после
resolve (иначе календарь/favour могут перезапустить). Chain child клонируется
как у REQUEST (`ONLY_VIA` → `ACTIVATED_8/12`).

---

## 4. Подзадачи

### B2a — Timed enemy / bedouin / egypt spawn

**Файлы:** `scenario_event_manager.cpp/.h`, `scenario_invasion.cpp/.h`

1. Маппинг `e_event_invader` → `e_attack_faction` / `e_enemy_type` / kingdome.
2. Прочитать `amount` (size), `invasion_attack_target` → `formation_attack_from_event_target`,
   `location_fields` / invasion point.
3. Вызвать `scenario_invasion_start` / `scenario_start_invasion_impl` с уникальным
   `invasion_id` (или стабильным из event).
4. Зарегистрировать pending; `chain_action_next = NONE`.
5. Триггеры: `ONCE`, `RECURRING` (дата как у других events).

**Приёмка:** pak timed invasion (без JS) спавнит армию; `on_completed` **не**
вызывается в тот же тик.

### B2-resolve — Отложенный исход (критический кусок)

Вынести из миссий в движок.

1. Хук в `scenario_invasion_process` или месяц/день тик event manager.
2. Условия как в `mission8_resolve_hyksos_invasion` / Behdet y15.
3. Fire `on_completed_action` / `on_refusal_action` / `on_defeat_action` через
   `process_event(..., via_event_trigger=true, caller_event_id)`.
4. Снять pending; не double-fire.

**Семантика исходов (зафиксировать по pak dump m5–8):**

| Исход | Action | Пример Selima Hyksos×22 |
|-------|--------|-------------------------|
| Армия уничтожена, destroy-goal нет | `COMPLETED` | +2 → siege → troops×4 |
| Destroy-goal выполнен, армия ушла | `REFUSED` (или DEFEAT) | −2 → troops×4 |
| (если есть в pak) timeout / special | `TOOLATE` / `DEFEAT` | уточнить dump’ом |

Перед кодом — сверить 1–2 pak event’а с ненулевыми `on_refusal` / `on_defeat`.

### B2b — Favour Kingdom Rating

1. В `process_events` / отдельном тике: для `EVENT_TRIGGER_BY_FAVOUR`, если
   `rating_kingdom <= 0` (или порог оригинала) → `process_event` как via/global
   по правилам pak.
2. После fire: `BY_FAVOUR_IN_USE` / `ALREADY_FIRED` (как editor 0x10 / 0x14).
3. Отключить или заглушить Caesar-legacy `kingdome_relation_t::process_invasion`,
   иначе двойной спавн с JS helper.
4. Размеры из pak: Timna/Behdet 45, Abedju 40, Selima 63 — остаются в event.amount,
   не в хардкоде helper’а.

**Приёмка:** KR→0 без JS helper → одна Pharaoh army нужного size; chain child
(B2c) если есть в pak.

### B2c — Chain-only invasions

1. `ONLY_VIA_EVENT` + type INVASION: `create(..., ACTIVATED_*)` как REQUEST
   (уже есть ветка для REQUEST; расширить или унифицировать).
2. Activated child в свой тик → spawn + pending (B2a path).
3. Родитель может быть request refusal / invasion completed / etc.

**Приёмка:** parent `on_refusal` → child invasion спавнится без календарной даты.

### B2d — Тесты

| Тест | Что проверяет |
|------|----------------|
| Integral: timed spawn | event ONCE → formations > 0 |
| Integral: resolve ok | wipe → child REPUTATION/tag fired |
| Integral: resolve refuse | destroy-goal → refusal chain |
| Integral: favour | KR=0 → Pharaoh army once |
| Console | `start_invasion` по-прежнему работает |
| Dump | `__test_mission_pak_dump` → `semantics=favour_kr_punishment` для 0x10 |

Файлы: `tests/NN_invasion_event_*.js` + при необходимости C++ helpers в
`js_test_*.cpp`.

---

## 5. Маппинг полей event → invasion_opts

| `event_ph_t` | `invasion_opts_t` / spawn |
|--------------|---------------------------|
| `item` (`e_event_invader`) | mode + enemy_type |
| `amount` | size |
| `invasion_attack_target` | `formation_attack_from_event_target` |
| `location_fields` / point | `invasion_point` |
| event index / tag | `invasion_id` (стабильный) |
| (warnings) path / years | B3; spawn может игнорировать warning UI сначала |

Invader mapping (черновик — сверить с существующим JS/`start_foreign_army_invasion`):

| Invader | mode | enemy |
|----------|------|--------|
| `ENEMY` | `ATTACK_TYPE_ENEMIES` | `g_scenario.enemy_id` |
| `EGYPT` | enemies / egypt | Egyptian |
| `PHARAOH` | `ATTACK_TYPE_KINGDOME` | Egyptian |
| `BEDUINS` | natives / barbarian | как в m11 |

---

## 6. Порядок внедрения

```
Phase 0  Спека исходов по pak (1–2 дня dump)
    │
Phase 1  B2a spawn + chain_action_next=NONE + pending registry (без save)
    │
Phase 2  B2-resolve tick + fire on_completed/refusal
    │
Phase 3  Save pending (или совместить с B3)
    │
Phase 4  B2c chain-only clone
    │
Phase 5  B2b favour + убрать dual spawn (legacy/JS)
    │
Phase 6  B2d tests
    │
Phase 7  B2-migrate: снять JS poll/favour helper в m5–8 (по одной миссии)
```

**Не** начинать Phase 7 до стабильного resolve: иначе регрессии Selima/Behdet.

Рекомендуемый первый PR: Phase 1–2 + минимальный integral (spawn + ok chain на
тестовой карте / mission stub). Favour и save — вторым PR.

### B2.5 (опционально, до/параллельно Phase 1)

Общий JS helper `mission_resolve_invasion(mission, opts)` в `missions.js` —
вынести копипасту Selima/Behdet poll (seen / wipe / destroy-goal → fire tags).
Не блокирует B2a; упрощает Phase 7.

---

## 7. Миграция миссий (Phase 7 / B2-migrate)

| Миссия | Сейчас | После B2 |
|--------|--------|----------|
| Timna (5) | JS raids + favour tick | pak events / JS create invasion event |
| Behdet (6) | JS + poll y15 KR+8 | event on_completed → +8 |
| Abedju (7) | JS + favour chain | B2b+c |
| Selima (8) | JS Hyksos poll → siege/troops | event chains; luxury-late troops уже ONLY_VIA |

Пока engine не готов — **не удалять** JS proxy. После B2d — по одной миссии PR,
сверка с dump; favour helper убирать только когда B2b закрыт для всех.

---

## 8. Файлы (ожидаемые касания)

| Файл | Роль |
|------|------|
| `src/scenario/scenario_event_manager.h/.cpp` | handler INVASION; NONE propagate; favour tick |
| `src/scenario/scenario_invasion.h/.cpp` | spawn opts; pending link; resolve tick |
| `src/city/city_kingdome_relations.cpp` | отключить/согласовать legacy invasion |
| `src/scripts/missions.js` | позже: упростить/удалить favour helper |
| `src/scripts/mission/m_005…m_008_*.js` | Phase 7 миграция |
| `src/io/gamestate/boilerplate.cpp` / chunks | save pending (Phase 3) |
| `tests/…` | B2d |
| `REMAKE_TASKS_P1.md` | отметить ✅ по подзадачам |

---

## 9. Риски

| Риск | Митигация |
|------|-----------|
| Двойной спавн favour (legacy + event + JS) | один источник; выключить два других |
| Неверная семантика REFUSED vs DEFEAT | Phase 0 dump |
| Save mid-invasion | Phase 3 обязателен для `.svx` |
| `improve`/ALREADY_FIRED слишком рано | fire chain только из resolve; spawn помечает отдельно |
| Регрессия Selima troops×4 chain | не трогать JS until Phase 7; integral на Hyksos×22 graph |

---

## 10. Критерии готовности (Definition of Done)

- [ ] Pak/editor `EVENT_TYPE_INVASION` спавнит армию без JS.
- [ ] `on_completed` / `on_refusal` стреляют **после** исхода, не в тик спавна.
- [ ] Favour 0x10: одна армия при KR collapse; dump semantics ок.
- [ ] Chain-only child invasion работает.
- [ ] `start_invasion` console ок.
- [ ] Integral tests зелёные.
- [ ] Mid-fight save/load сохраняет pending (или явно documented defer → B3+chunk).
- [ ] `REMAKE_TASKS_P1.md` B2a–d обновлены; миссии 5–8 ещё могут быть на proxy
      до Phase 7.

---

## 11. Оценка (грубо)

| Фаза | Объём |
|------|--------|
| Phase 0 dump/спека | S |
| Phase 1–2 spawn+resolve | M–L |
| Phase 3 save | S–M |
| Phase 4–5 chain+favour | M |
| Phase 6 tests | S–M |
| Phase 7 per mission | S each |

Эпик именно из‑за **отложенного resolve + save + согласование favour**, не из‑за
вызова spawn.
