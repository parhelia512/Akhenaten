# Handoff: empire-миссии → JS (2026-07-25)

Самодостаточная шпаргалка, чтобы продолжить в новой сессии.  
Полный playbook: **[MISSION_TO_JS.md](MISSION_TO_JS.md)**.  
Pak triage (skip/ladder/shared chain): **[MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md)**.  
Кампания / favour / грабли: **[REMAKE_NOTES.md](REMAKE_NOTES.md)**.  
B2 (event invasions): **[REMAKE_B2_INVASION_PLAN.md](REMAKE_B2_INVASION_PLAN.md)**.

---

## Цель трека

Полный redefine empire-карты + точные request/invasion-цепочки из `mission1.pak`
в `src/scripts/mission/m_NNN_*.js`, плюс wiki `docs/wiki/player/missions/`.

Эталон структуры: **`m_004_mennefer.js`** / **`m_007_abydos.js`** / **`m_008_selima.js`**.  
Цепочки запросов/подарков: **m_004 … m_008** (не упрощённые ±8/±6).

---

## Статус миссий

| ID | Город | Empire full | Request chains | Invasions | Wiki | Заметки |
|----|-------|-------------|----------------|-----------|------|---------|
| 3 | Nekhen | нет (tutorial) | tutorial | — | `nekhen.html` | win: housing_level 8 = Modest Apt |
| **4** | Men-nefer | **да** | **да** | — | `mennefer.html` | beer→+5→gift 20 bricks; papyrus demand± |
| **5** | Timna | **да** | **да** | Libyan + favour 45 | `timna.html` | gems/weapons→Men-nefer open; deben→gift |
| **6** | Behdet | **да** | **да** | Kushite + favour 45 | `behdet.html` | y15 clear→KR+8 (poll); FAILED_FLOOD once |
| **7** | Abedju | **да** | **да** | Kushite + favour 40→40 | `abedju.html` | perfect flood yearly m8 |
| **8** | Selima | **да** | **да** | Hyksos + favour 63 (poll) | `selima.html` | см. § Selima ниже |

Миссии 0–2: wiki есть, empire почти из pak.  
9–18: скрипты есть, empire patch/частичный. **Следующий кандидат:** Abu (9).

---

## Selima (8) — сделано в этой волне

| Тема | Статус |
|------|--------|
| Full empire redefine (`hide_pak_*`, pos/idx, texts, ornaments) | ✅ |
| Display cities route ids (Nekhen 6 / Nubt 7 / Kyrene 8 / Byblos 10) | ✅ |
| Routes 7/8/10 — 2-pt stubs + `deviation` (engine `improve_route`) | ✅ |
| Luxury late → KR+2 → Kerma siege → troops×4 (SAME as Hyksos×22 wipe) | ✅ |
| Troops×7/×4 subtypes + MESSAGE/CITY_STATUS tails; NEW_TRADE opens Men-nefer | ✅ |
| Hyksos×22 resolve → +2→siege→troops4 / −2→troops4 (poll; ждёт B2) | ✅ |
| `EVENT_TYPE_PRICE_↑/↓` → `trade_price_change` + price phrases | ✅ engine |
| `debt_interest` meta `int_dcy` [10,15,20,25,30]; Selima Normal=20 pak | ✅ |
| Orphan LOST_TRADE Nekhen/Kyrene | skip (никто не ссылается) |
| price_decrease→siege y0 (битая связь редактора) | skip |
| funds 6000/2500 pak vs ladder 4–7 | оставить лестницу |
| Native `EVENT_TYPE_INVASION` + deferred on_completed | **B2** — план готов |

---

## Состояние рабочей копии

Миссии 4–7 + API — в HEAD (`a89b5d751`).  
**Selima (8) + engine (PRICE, debt_interest/`int_dcy`, route deviation, NEW/LOST open,
troops subtypes, fire_chain)** — в рабочей копии; коммитить отдельно от мостов.

Параллельный трек мостов: `building_bridge*`, `grid/bridge*`, `m_129_bridges.js`, …  
План мостов: `REMAKE_BRIDGE_PLAN.md`.

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
- `create_good_request({ …, trigger: EVENT_TRIGGER_ONLY_VIA_EVENT, subtype })` — chain child
  (Selima troops×4). После активации REQUEST-master → `ALREADY_FIRED`.
- `__city_event_fire_chain(tag)` — активировать ONLY_VIA master из JS (Hyksos, если late
  не сработал).

### Meta по сложности (`int_dcy`)

```js
initial_funds [7500, 5000, 3750, 2500, 2000]
rescue_loans  [7500, 5000, 3750, 2500, 2000]
debt_interest [10, 15, 20, 25, 30]   // annual %; 0/empty → pak rate → 10
house_tax_multipliers [300, 200, 150, 100, 75]
```

Чтение: `g_scenario.debt_interest()` / `.get()` на `int_dcy` (`game/difficulty.h`).

### Empire routes: 2 точки + deviation

```js
{
    route: 8,
    type: 1,
    deviation: 60,   // max perpendicular offset when improve_route() splits
    points: [ [633, 1375], [22, 341] ]
}
```

`fix_trade_routes()` → `improve_route()` после load; UI всё равно densify при draw.

### Детект «враги убиты» (пока нет B2)

```js
city.num_enemy_formations
city.enemy_army_achieved_destroy_goal(invasion_id)
```

Паттерн: start → seen → clear → fire KR / chain.  
Обход до **B2** (`REMAKE_B2_INVASION_PLAN.md`). Favour: `mission_pharaoh_favour_invasion_tick`.

### Цепочки (эталон Selima)

```js
// Luxury late = +2 → Kerma siege → troops×4 (ONLY_VIA tag 4)
// Hyksos×22 wipe = +2 → siege → fire_chain(4); refuse = −2 → fire_chain(4)
// Troops×7: subtype CITY_ASKS; ×4: DISTANT_BATTLE
// NEW_TRADE: set_trade_enabled + is_open=true (Men-nefer, не Kyrene)
```

---

## Известные дыры / stubs

| Что | Где | Статус |
|-----|-----|--------|
| `EVENT_TYPE_DEMAND_↑/↓` | engine | ✅ |
| `EVENT_TYPE_SEA/LAND_TRADE_PROBLEM` | engine | ✅ |
| `EVENT_TYPE_WAGE_↑/↓` | engine | ✅ |
| `EVENT_TYPE_CONTAMINATED_WATER` | engine | ✅ |
| `EVENT_TYPE_CLAY_PIT_FLOOD` | engine + Behdet | ✅ |
| `EVENT_TYPE_PRICE_↑/↓` | engine | ✅ `trade_price_change` + phrases |
| `debt_interest` / `int_dcy` meta | engine + missions | ✅ |
| empire route `deviation` | `improve_route` | ✅ |
| `EVENT_TYPE_FAILED_FLOOD` | Behdet y10 | handler есть; JS пока **ONCE** |
| `EVENT_TYPE_PERFECT_FLOOD` | Abedju | yearly poll m8 |
| **`EVENT_TYPE_INVASION` + deferred on_completed** | engine | **TODO — [план B2](REMAKE_B2_INVASION_PLAN.md)** |
| Invasion poll KR / favour | m5–8 JS | proxy до B2 |
| Orphan LOST_TRADE / broken editor links | pak junk | skip |
| B3 invasion warnings save | engine | TODO |
| B4 phrase_id | engine | TODO |

Pak `attack=` → `invasion_attack_target`. Selima Hyksos: `attack=3` = TROOPS.

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

Фильтр: `pak_event:`, `pak_request:`, `pak_invasion_event:`, `pak_map_city:`,
`pak_route_limits:`, `pak_map_route:`, `pak_ornament`, `pak_funds:`, `pak_win:`.

---

## Чеклист следующей миссии (**9 Abu**)

Следуй DoD в [`MISSION_TO_JS.md`](MISSION_TO_JS.md) §6 и triage
[`MISSION_PAK_TRIAGE.md`](MISSION_PAK_TRIAGE.md). Кратко:

1. Дамп scenario **9** → ключевые `pak_*`.
2. Triage: orphan/broken skip; shared leaf → один `ONLY_VIA`; funds = `int_dcy`.
3. Каркас empire из `m_008`.
4. `hide_pak_*` + города `pos`/`idx` (ours + display `route`).
5. `empire_routes` (2-pt + `deviation` если пусто) + `route_limits` + texts/ornaments.
6. Requests: `ok/refuse/late` + subtypes; PRICE/DEMAND — мутация state.
7. Invasions: poll до B2; favour из `by_favour` — **не** снимать JS proxy.
8. Wiki `abu.html` + index/nav + эта таблица + `MISSION_TO_JS.md` §7.

---

## Wiki

- Есть: mennefer / timna / behdet / abedju / **selima** / nekhen (+ earlier)
- **Нет:** `abu.html` (mission 9)

---

## Файлы

```
src/scripts/mission/m_008_selima.js      ← Selima done
src/scripts/mission/m_009_abu.js        ← next
MISSION_PAK_TRIAGE.md                   ← skip / ladder / shared chain
REMAKE_B2_INVASION_PLAN.md
MISSION_TO_JS.md / MISSION_TO_JS_HANDOFF.md
docs/wiki/player/missions/selima.html
```

---

## Verification (Selima)

- Rebuild C++: PRICE, debt_interest, route deviation, fire_chain, NEW/LOST, troops subtypes.
- Hot-reload JS: `--mixed src/scripts`.
- Empire: display routes 6–8/10; trade routes 1–4/9.
- Luxury late → +2 → siege → troops×4; Hyksos×22 same troops path.
- Debt interest Normal 20%; PRICE luxury↓ / reeds↑ меняют buy/sell.
- Favour KR → Pharaoh 63 (JS proxy until B2).
