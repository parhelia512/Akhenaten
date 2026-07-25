# Pak → JS: triage (что копировать, что нет)

Шпаргалка из remake миссий 4–9 (Selima, Abu).  
Playbook: [`MISSION_TO_JS.md`](MISSION_TO_JS.md) · handoff: [`MISSION_TO_JS_HANDOFF.md`](MISSION_TO_JS_HANDOFF.md) ·  
очередь A ∥ B2: [`REMAKE_EMPIRE_MISSIONS_PLAN.md`](REMAKE_EMPIRE_MISSIONS_PLAN.md).

---

## Решение по каждому pak-артефакту

| Сигнал в dump | Действие | Пример |
|---------------|----------|--------|
| Event с `on_*` → реальный child, child достижим из корня | **Копировать** цепочку | luxury late → +2 → siege → troops×4 |
| Два разных parent ведут в **один** leaf (same tag graph) | **Один** shared master (`ONLY_VIA` + `fire_chain` / общий tag) | late luxury ≡ Hyksos×22 wipe → troops×4 |
| Event/LOST/NEW никто не ссылается (`on_*=-1`, нет calendar) | **Skip** + заметка в wiki/handoff | orphan LOST Nekhen/Kyrene |
| Связь редактора бессмысленна (price→siege y0) | **Skip**; чинить playable graph | не wire PRICE к siege |
| Одно число funds/loan в pak, у 4–7 есть лестница | **Лестница** `int_dcy` как у соседей | funds ladder, не 6000/2500 |
| `debt_interest` / аналог в pak | Meta `int_dcy` вокруг Normal=pak | `[10,15,20,25,30]` |
| City `route=N`, polyline пустой | **2 точки** ours→city + опц. `deviation` | Nubt/Kyrene/Byblos |
| City `route=N`, polyline есть | Копировать `pak_map_route` points | route 1–4, 6, 9 |
| NEW_TRADE `city_id` = display-only | Копировать как в pak **или** remap (решение миссии) | Abu → Kyrene; Selima troops → Men-nefer |
| CITY_STATUS subtype 1 | Engine: `FOREIGN_CITY_CONQUERED` message + ok chain | Abu y6 Selima |
| MESSAGE/CITY_STATUS tails у request | Копировать subtype + city; проверить engine handler | troops×7/×4 |
| `pak_entry` / `pak_exit` / river | JS **overlay** (omit → keep pak) | m4–9 |
| `pak_disembark` / `pak_inv_land` / `pak_inv_sea` | JS **config-only** (omit → empty) | disembark m6/7/9; inv m2, m5–18 |
| `pak_earthquake:invalid` / empty disembark/inv | **Omit** ключ (не писать `[]`) | Timna / Selima / Nubt |
| Sparse disembark / invasion slots | `[[x,y], [-1,-1], …]` | Behdet / Abedju / Abu; Iunet / Rostja |
| `EVENT_TYPE_INVASION` + on_completed | Пока **JS poll**; native → [B2](REMAKE_B2_INVASION_PLAN.md) | Hyksos / favour |

---

## Handler checklist (фраза ≠ эффект)

Перед «готово» по типу события проверь мутацию мира:

| Тип | Минимум |
|-----|---------|
| `NEW_TRADE_ROUTE` | `set_trade_enabled` **и** `is_open` / `trade_route_open` |
| `LOST_TRADE_ROUTE` | close only; не wipe sells/buys |
| `PRICE_↑/↓` | `trade_price_change(±amount)` + price phrases |
| `DEMAND_↑/↓` | `route.increase/decrease_limit` |
| `CITY_UNDER_SIEGE` | city_id + months (`amount`) |
| `CITY_STATUS` subtype 1 | foreign city conquered message (+ ok chain) |
| `REQUEST` ok/refuse/late | tags → MESSAGE / KR / gift / trade |
| `INVASION` | spawn + **отложенный** on_* (B2); не sync completed |

---

## Shared chain (шаблон)

```js
// Один master; несколько родителей только ставят completed_action_tag / fire_chain
mission_ensure_troops_request_4(true)  // ONLY_VIA, tag 4, once
late_siege.set_completed_action_tag(4)
// …
mission_fire_troops_request_4()        // если wired → __city_event_fire_chain(4)
```

Флаги: `*_wired` (шаблон создан) ≠ `*_requested` / fired (уже активирован).

---

## Meta по сложности

Всегда `int_dcy` (`game/difficulty.h`), не ручной `array[difficulty()]`:

```js
initial_funds […]
rescue_loans […]
debt_interest […]      // optional; empty → pak → 10%
house_tax_multipliers […]
```

---

## DoD миссии (empire full)

Полный чеклист + B2-migrate / FF1 / D1–D4:
[`REMAKE_EMPIRE_MISSIONS_PLAN.md`](REMAKE_EMPIRE_MISSIONS_PLAN.md).

- [ ] Dump scenario N сохранён/сверен
- [ ] **Triage decision log:** каждый спорный NEW_TRADE / FOREIGN / orphan →
      `copy` | `remap` | `skip` + почему (комментарий скрипта и/или wiki/handoff)
- [ ] `hide_pak_cities/routes/objects` + все города `pos`/`idx` (ours + display)
- [ ] Trade + display `route` ids; пустые polyline → 2-pt ± `deviation`
- [ ] `route_limits` / costs / sells-buys
- [ ] Map points: entry/exit/river (omit → keep pak); disembark +
      `invasion_points_land|sea` в JS (omit → empty; sparse `[-1,-1]`)
- [ ] Requests: ok/refuse/late из dump; shared chains где leaf общий
- [ ] Invasions: poll **до B2-migrate**; favour size из pak
- [ ] PRICE/DEMAND/WAGE/flood по dump; handler мутирует state
- [ ] Wiki + index/nav; handoff/status table
- [ ] Stage только файлы задачи (не bridges/hunting/video)
- [ ] Проверка: empire map, open trade, один shared leaf не двойной fire

---

## Subtype audit (D1)

Неизвестный `CITY_STATUS` / `MESSAGE` subtype → не угадывать эффект.
Занести в таблицу (mission#, subtype, handler yes/no-op) в handoff или сюда;
чинить engine только с подтверждённой семантикой (как Abu subtype 1).