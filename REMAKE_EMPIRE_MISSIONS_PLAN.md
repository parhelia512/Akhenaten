# План: empire-миссии → JS + B2 (2026-07-25)

Рабочий план трека после **Saqqara (10)** (`3affd5b21`; Abu `c10506b5f`).  
Не смешивать с bridges / hunting / OpenH264 / video — отдельные PR (**PC4**).

| Документ | Роль |
|----------|------|
| **Этот файл** | очередь работ A ∥ B ∥ D, DoD, hygiene |
| [MISSION_TO_JS.md](MISSION_TO_JS.md) | playbook переноса |
| [MISSION_TO_JS_HANDOFF.md](MISSION_TO_JS_HANDOFF.md) | сессионный статус / API |
| [MISSION_PAK_TRIAGE.md](MISSION_PAK_TRIAGE.md) | copy / remap / skip |
| [REMAKE_B2_INVASION_PLAN.md](REMAKE_B2_INVASION_PLAN.md) | native invasions |
| [REMAKE_NOTES.md](REMAKE_NOTES.md) | грабли кампании |
| [REMAKE_TODO.md](REMAKE_TODO.md) | QA1 / PC4 / B2–B4 в общем backlog |

---

## Цель

1. Full redefine empire + events для миссий **11 → 18** (затем Cleopatra при необходимости).
2. Параллельно (или врезкой) закрыть **B2**, затем **B2-migrate** (снять JS poll).
3. Каждая миссия закрывается только с wiki + handoff-строкой + triage decision log.

---

## Поток A — очередь миссий (последовательный)

Одна миссия = один коммит (или PR) **только** mission/wiki/handoff (+ C++ если нужен новый handler).

| # | Миссия | Эталон каркаса | Примечание |
|---|--------|----------------|------------|
| **10** | Saqqara | `m_009_abu.js` | **done** `3affd5b21` + `map_file`/`data/maps` |
| **11** | Serabit Khadim | m_010 / m_009 | **done** (2026-07-25 full redefine) |
| **12** | Meidum | m_010 / m_011 | **next** |
| 13 / 14 | Buhen / S. Dahshur | m_009 | ветка; `next_mission:15` явно |
| 15 | N. Dahshur | m_009 | choice host |
| 16 / 17 | Iunet / On | m_009 | |
| 18 | Rostja | m_009 | |

**Следующий шаг прямо сейчас:** dump scenario **12** → triage → redefine `m_012_meidum.js` + wiki.

### DoD одной миссии (чекбокс)

- [ ] `__test_mission_pak_dump(N)` → события, города, routes, funds, **map points**
- [ ] Triage: orphan/broken skip; shared leaf → один `ONLY_VIA`; funds = `int_dcy`
- [ ] **Triage decision log** в комментарии скрипта и/или handoff/wiki: каждый
      display NEW_TRADE / пустой FOREIGN / orphan — `copy` | `remap` | `skip` + почему
- [ ] `hide_pak_*` + cities `pos`/`idx` + routes (± `deviation`) + texts/ornaments
- [ ] Map points: entry/exit/river (omit invalid → keep pak); disembark +
      `invasion_points_land|sea` в JS (omit → empty; sparse → `[-1,-1]`)
- [ ] Requests/DEMAND/PRICE/… с мутацией state; favour/invasions = JS poll до B2-migrate
- [ ] Wiki + index/nav; обновить handoff § статус
- [ ] Коммит **без** посторонних треков (bridges / hunting / video / OpenH264)

### Правила triage (кратко)

| Ситуация | Действие |
|----------|----------|
| Display NEW_TRADE / пустой FOREIGN | 1:1 как pak **или** remap — записать решение |
| `ok→` на уже fired ONCE | wire граф; ожидать no-op |
| Favour `by_favour` | `mission_pharaoh_favour_invasion_tick`; не timed enemy |
| CITY_STATUS subtype 1 | engine OK (FOREIGN_CITY_CONQUERED) |
| Неизвестный CITY_STATUS / MESSAGE subtype | не «угадывать»; занести в **AUD1** |

---

## Поток B — engine (параллельно A)

| ID | Задача | Блокирует | Статус |
|----|--------|-----------|--------|
| **B2** | Native `EVENT_TYPE_INVASION` + deferred on_completed/refuse | poll в m5–9+; editor | план готов |
| **B2-migrate** | После B2 Phase 7: убрать/сузить JS favour+invasion poll в **m5–9** (+10+ по мере redefine) | fidelity campaigns | **явный шаг, не «когда-нибудь»** |
| B3 | Invasion warnings save | long campaigns | TODO; после B2 |
| B4 | phrase_id | event messages | TODO; после B2 |
| **FF1** | FAILED_FLOOD recurring (Behdet) | fidelity m6 | small; см. DoD ниже |

**B2 gate:** код B2 → **сразу** B2-migrate m5–9 → только потом B3/B4, если нет spare bandwidth.

### FF1 — FAILED_FLOOD recurring (DoD)

- [ ] Dump Behdet: подтвердить recurring vs ONCE
- [ ] Engine/JS: recurring как в pak (не один ONCE)
- [ ] Wiki Behdet + handoff note
- [ ] Отдельный маленький коммит (не смешивать с Serabit / empire PR)

---

## Поток D — hygiene / fidelity (из волны Abu)

| ID | Задача | Когда |
|----|--------|-------|
| **AUD1** | **CITY_STATUS / MESSAGE subtype audit** (= «D1» в старых заметках; не миссия 11) — dump m4–18: какие subtypes silent no-op; чинить handlers или явно skip в triage | врезка 1 сессия или по мере миссий 10+ |
| **D2** | **Handoff freshness** — в начале сессии: `git log -1` / `git status` vs handoff «последний коммит / untracked»; править до работы | каждая сессия |
| **D3** | **QA1 canary slice** — не полный golden dump: после redefine миссии N маркеры map points + ключевые event tags (ad-hoc `99_tmp_*`) | опционально после каждой миссии; см. REMAKE_TODO QA1 |
| **D4** | **PC4 commit hygiene** — stage только файлы задачи; reject hunting/bridges/video в empire PR | каждый коммит |

### AUD1 — subtype audit (DoD)

- [ ] Таблица: `type` + `subtype` + где встречается (mission#) + handler? (yes/message/no-op)
- [ ] Минимум: все `CITY_STATUS` и `MESSAGE` из dump m4–9, затем 10–18 по мере redefine
- [ ] Новые no-op → либо engine fix (как subtype 1), либо triage `skip` + wiki
- [ ] Результат: секция в handoff или `MISSION_PAK_TRIAGE.md`

### D2 — handoff freshness (чеклист сессии)

```
git log -1 --oneline
git status --short -- MISSION_TO_JS_HANDOFF.md src/scripts/mission/
# сверить «последний коммит / незакоммичено» в handoff; обновить если врало
```

### D3 — QA1 canary (шаблон маркеров)

После redefine N (ad-hoc dump или hot-load):

```
pak_entry / pak_exit / pak_river_*  == JS overlay (omit → pak)
pak_disembark / pak_inv_land / pak_inv_sea  == JS config (omit → empty)
ключевые tag_id REQUEST/DEMAND/NEW_TRADE/CITY_STATUS из triage log
```

Полный baseline 0–18 — отдельная задача **QA1** в `REMAKE_TODO.md`, не блокер Saqqara.

---

## Поток C — не в этом треке (явный запрос)

- Bridges (BR*), hunting/ostrich, OpenH264/video
- Генерация тайлов / стартовых зданий из JS
- Слепое копирование всего dump без triage
- Full redefine 10–18 одним PR
- Cleopatra empire full — после 10–18
- B3/B4 параллельно с Saqqara — только при spare bandwidth

---

## Каденция

1. **Session start:** D2 (handoff freshness).
2. **Session default:** одна миссия из очереди A (сейчас Meidum).
3. **Когда B2 готов к коду:** врезка → B2 phases → **B2-migrate** → снова A.
4. **После каждой миссии:** triage decision log, handoff «следующий = N+1», wiki, опц. D3 canary, D4 stage check.
5. **Врезки:** FF1 или AUD1 — отдельный маленький коммит, не внутри Serabit / empire PR.

### Verification (шаблон)

```
--mixed src/scripts
empire map: routes + display
trade open / NEW_TRADE
requests KR tails
favour (if any) still JS until B2-migrate
map points match dump
triage decisions documented
git status: no foreign tracks in stage
```

---

## Снимок прогресса empire full

| Done | Next |
|------|------|
| **4–10** Men-nefer … Saqqara (`3affd5b21`) | **11** Serabit Khadim |
| `map_file` → `data/maps` (0–18); pak fallback | — |
| Map-point API + values m4–10; **invasion_points** JS m2, m5–18 (`697a61836`) | entry/exit/river/disembark для 11+ где есть |
| CITY_STATUS subtype 1 | **AUD1** audit остальных subtypes |
| PRICE / debt_interest / NEW_TRADE is_open | — |
| — | **B2** → **B2-migrate**; **FF1** Behdet |

Handoff-детали Saqqara / API: [MISSION_TO_JS_HANDOFF.md](MISSION_TO_JS_HANDOFF.md).
