# Akhenaten — что осталось до полного ремейка Pharaoh + Cleopatra

Снимок состояния на 2026-07-03 (версия 0.2.7). Собрано автоматическим обходом
`src/`, `docs/wiki/`, `tests/` и истории коммитов. Пути и номера строк указаны на момент анализа.
Обновлено 2026-07-16: P1 разбит на 5 частей, отмечены выполненные B1 (ветвление),
D1 (миссия 11), D2 (миссия 12 Meidum), D3 (миссии 13 Buhen / 14 South Dahshur),
D4 (миссия 15), D5 (миссии 16 Iunet / 17 On / 18 Rostja); статус остальных пунктов P1
сверен с кодом. Добавлены B5 (валидация choice) и блок F — вражеские колесницы, баг
данных `enemies.js`, формула рейтинга монументов (F3). Заскриптованы миссии 0–18.
Обновлено 2026-07-18: закрыты хотфиксы волны 3 — F3 (форма формулы, калибровка весов
осталась), F1 (`enemies.js`), B5 (валидация choice). Открыты в блоке B: B2, B3, B4.
Дополнено 2026-07-18 (вечер): в P3 добавлены H1 / J1 / FP1 / J2 / T1 / T2 —
баги, техдолг и процессные задачи после farm preview и house `evolve_text` в JS.
Позже 2026-07-18: закрыт #608 (STOP Empty All больше не оставляет всем Empty);
в P3 добавлены S1 / S2 (техдолг storage orders), заметки — `REMAKE_NOTES.md` §8.
Обновлено 2026-07-20: разведка редактора сценариев — добавлен блок «P2 — Редактор
сценариев (восстановление)» (ED1–ED6): код компилируется, но точка входа не подключена
и persist сценариев — заглушки; пересекается с B2 и `write_mission` (P3-IO).
Обновлено 2026-07-24: сверка статуса с деревом. F2 (колесницы, `3e8db22c8`) и C4 (bent
pyramid, `412b7bc9b`) закоммичены — сняты пометки «(в работе)» в таблице «Выполнено».
B2/B3/B4 по-прежнему открыты (TODO-заглушки на месте, номера строк не сдвинулись).
Вне плана добавлена венгерская локализация (#612, `81d7e4f2d`) — новый полный язык (hu).
Позже 2026-07-24: soft-warn для Pharaoh-only (без Cleopatra packs) + `--nodatacheck`;
в P3 добавлен блок **DX1–DX6** (старт / data paths / docs / degrade).
Ещё позже 2026-07-24: в P3 добавлены **QA1–QA3**, **DX7–DX10**, **AS1–AS3**,
**PC1–PC3** (локали-заглушки, mission dumps, first-run UX, community sticky, техдолг bootstrap).

Приоритеты:
- **P1** — блокирует прохождение оригинальной кампании;
- **P2** — нужно для полноты ремейка (контент/механики оригинала);
- **P3** — полировка, инфраструктура, необязательное.

---

## Что уже сделано (кратко)

- **Ядро симуляции города — практически полное**: население (возраст, рождаемость, миграция),
  здоровье/чума, финансы (налоги, зарплаты, долг, дань), рейтинги culture/prosperity/kingdom,
  религия (5 богов, благословения/проклятия, фестивали, храмовые комплексы),
  цикл разлива Нила и floodplain-фермерство, каналы/water lift.
- **Здания**: жильё (все 20 уровней), фермы, добыча (с истощением ресурсов), мастерские,
  распределение (базары, амбары, склады, доки), религия, развлечения, здоровье/образование,
  администрация, военные постройки, гильдии строителей — реализованы.
- **Движок монументов** (фазы, доставка ресурсов, work camp + гильдии) готов; работают
  малые/средние мастабы и малые/средние ступенчатые пирамиды; medium mastaba/stepped
  используют корректные конфиги и типы; stepped pyramids учитываются в monument rating;
  enum mausoleum без дубликатов (`e630bd1fc`). Сфинкс (C6) и обелиски (C7 каркас) —
  в коде; процессные уроки (JS amounts, staffed SY, integral screenshots) —
  `REMAKE_NOTES.md` §4 / паттерн блока C в `REMAKE_TASKS_P1.md`.
- **Наземная военка**: форты (все 3 типа), батальоны, formation AI, мораль, боёвка,
  пехота/лучники/колесницы игрока, цепочка recruiter → military academy, башни с часовыми и
  баллистами, пешие юниты всех 13 наций врагов, вторжения, distant battles, армия Фараона
  при низком kingdom rating.
- **Флот игрока (базовый)**: warship — бой, приказы, repair; transport ship — embark/sail/disembark
  (`99f8dcd23`). UI погрузки батальона и вражеский флот — ещё нет (E3).
- **Торговля и империя**: карта империи, маршруты, караваны и торговые корабли, requests, подарки.
- **UI**: все 12 советников (+2 новых), инфо-панели зданий и фигур, карта империи, сообщения,
  брифинги/победа, файловые диалоги, опции, главное меню, выбор сценариев. Видео (Bink+Smacker),
  музыка/речь/эмбиент — работают.
- **Ветвление кампании (JS)** — независимые выборы через `choice[]` (Timna→6/7, Behdet/Abydos→8/9,
  Saqqara→11/12); линейные `next_mission` между сходами (8/9→10).
- **Миссии 0–18** (Nubt → Rostja) полностью заскриптованы; загрузка оригинальных
  `.sav`/`.pak`/`.map`; брифинги с видео, перенос личных сбережений между миссиями,
  логика победы/поражения (в JS: `src/scripts/city/victory.js`), настройки сложности
  с реальными эффектами, эволюция/деградация жилья всех 20 уровней.
- **Инфраструктура**: MuJS-скриптование (~90k строк JS, идёт активная миграция логики),
  JS-отладчик (DAP), Tracy, 23 интеграционных теста, сборки Win/Linux/mac/Android/Web.

---

> P1 разбит на **5 частей**. Атомарные задачи с описаниями для исполнителей — в
> **[REMAKE_TASKS_P1.md](REMAKE_TASKS_P1.md)** (блоки B/C/D/E); в скобках их ID и коммиты.
> Легенда: `[x]` — сделано, `[ ]` — осталось.
>
> Рабочие заметки по скриптованию миссий (грабли прогрессии, долг временных целей
> монументов, чего не хватает в движке) — в **[REMAKE_NOTES.md](REMAKE_NOTES.md)**.

## P1 — разбивка на части

### Часть 1 — Инфраструктура кампании (блок B)

- [x] **B1. Ветвление кампании и ранги (JS)** — независимые выборы через `choice[]`
      (Timna→6/7, Behdet/Abydos→8/9, Saqqara→11/12), сохранение `campaign_mission_rank`
      (`f5a17b591`, `bd1298219`).
- [ ] **B2. Event-driven вторжения — no-op**: `case EVENT_TYPE_INVASION: // TODO break;`
      в `src/scenario/scenario_event_manager.cpp:544-546`. Таймерные/редакторские вторжения
      не срабатывают (прямой спавн через `scenario_invasion.cpp` работает). Редактор
      сценариев (`src/window/editor/`) позволяет создавать invasions/events, которые
      рантайм молча игнорирует. **Разблокирует редакторские вторжения — см. ED5.**
- [ ] **B3. Сериализация invasion warnings** — тело `iob_invasion_warnings` закомментировано
      (`src/scenario/scenario_invasion.cpp:456-477`, `// TODO`). После load предупреждения теряются.
- [ ] **B4. Подстановка фраз в событийных сообщениях**: `int phrase_id = -1; // TODO`
      в `scenario_event_manager.cpp:607`; там же TODO на строках 545, 788.
- [x] **B5. Валидация целей `choice[]`** — путь `choice[]` → `__game_load_mission()` шёл
      без проверки JS-конфига (`__game_mission_is_valid` истинен для всех слотов кампании
      из `campaign.txt`, включая незаскриптованные). Введён `mission_is_playable(id) =
      is_valid && get_mission_config(id)`; `mission_get_visible_choices` скрывает тупиковые
      пункты, `compute_next` и guard хоста используют его. Тупик 18→19/20 закрыт; кампания
      после последней заскриптованной миссии завершается штатно.
- [x] **B6. Choice-хост через `next_mission` пропускался** — если у миссии есть `choice[]`
      и в неё заходят по линейному `next_mission`, `game_show_mission_choice` показывал выбор
      вместо загрузки хоста (поле `after` нигде не проставлено), пропуская Saqqara (8/9→10),
      North Dahshur (13/14→15) и др. Исправлено (вариант B): развилка активна только при
      `host == completed`; иначе хост грузится и играется. Пред­существующий баг, не из B5.

### Часть 2 — Монументы (блок C, ключевая фича Pharaoh)

Движок готов (`src/building/monuments.cpp`); реализованы только **малые/средние**
ступенчатые пирамиды и мастабы (`BUILDING_METAINFO`). Остальные типы существуют лишь как
enum в `src/building/building_type.h` — класса и конфига нет. Каждый новый тип **обязан**
получить вес в `MONUMENT_WEIGHTS` (`src/scripts/city/monuments.js`), иначе не влияет на рейтинг.

- [ ] **C1. Большая ступенчатая пирамида** — `BUILDING_LARGE_STEPPED_PYRAMID` (enum `:217`)
- [ ] **C2. Большая мастаба** — `BUILDING_LARGE_MASTABA` (enum `:227`), параметры закомм. в `monument_mastaba.cpp:139`
- [ ] **C3. Истинные (гладкие) пирамиды** — `BUILDING_SMALL/MEDIUM/LARGE_PYRAMID` (`:220-222`) + комплексы
- [x] **C4. Ломаные (bent) пирамиды** — `BUILDING_SMALL/MEDIUM_BENT_PYRAMID` (`:241-242`);
      классы мирроринг stepped (наследуют `building_stepped_pyramid`), реальный арт из
      `PACK_BENT_PYRAMID` (пак структурно идентичен stepped), веса в `MONUMENT_WEIGHTS`,
      миссия 14 возвращена на bent (цель 21). Сборка + 41/41 интегральных тестов с ресурсами.
- [ ] **C5. Кирпичные (mudbrick) пирамиды** — все варианты + комплексы (`:210-214`)
- [x] **C6. Сфинкс** — `BUILDING_SPHINX` (=210): 3 части 6×6 (`monument_sphinx.*`),
      паки stage 1–6 (`53000..53036`, compaction SYSTEM при `system:false`), info-окно,
      `tests/43_sphinx_place.js` PASS. Ресурсы/кол-ва — stub `TODO(orig-data)`;
      rock placement — `TODO(sphinx-rock)`; миссия 18 — после C3. План:
      `REMAKE_SPHINX_PLAN.md`.
- [ ] **C7. Обелиски** — `BUILDING_SMALL/LARGE_OBELISK` (**262 / 263**); гранитная цепочка.
      План: `REMAKE_OBELISK_PLAN.md` (одно здание, этапы 3×3 a–d / 5×5 a–f; без частей).
      **Каркас ✅:** staffed SY pre-stock, Buhen, wiki, `timber_loads` в JS, `44_obelisk_place`
      happy-path + reject (C7-T1 ✅). **Хвост (план §10):** ручной прогон (C7-T2);
      визуал Q5 + ladder offset (C7-V1/V2); опц. хелпер центра для тестов (C7-I1).
      Не блокер C8: F3-калибровка / миссии large (D-блок).
- [ ] **C8. Sun Temple** — `BUILDING_SUN_TEMPLE` (`:230`)
- [ ] **C9. Mausoleum** — `BUILDING_MAUSOLEUM`, `MAUSOLEUM_0/1/2` (`:192,231-233`), 3 размера
- [ ] **C10. Царские гробницы** — small/medium/large/grand royal tomb (`:199,204-206`); burial-варианты — P2
- [ ] **Монументы Cleopatra** (для кампании 38–52, тем же паттерном после блока C):
      Abu Simbel, Alexandria Library (не путать с обычной `building_library.*`), Caesareum,
      Pharos Lighthouse, Colossi, Temple of Luxor

### Часть 3 — Флот и вражеские юниты (блоки E, F)

- [x] **E1. Боевой корабль игрока** — бой, приказы, repair (`99f8dcd23`).
- [x] **E2. Транспортный корабль** — embark/sail/disembark (`99f8dcd23`).
- [ ] **E3. Вражеский флот и морские вторжения** — данные наций есть в `enemies.js` и
      enum'ы в `figure_type.h`, но `FIGURE_METAINFO`-класса нет → морские вторжения
      нефункциональны. Нужен к миссии 36 (Rowarty).
- [ ] **UI погрузки батальона на transport ship** — логика embark/sail/disembark есть,
      но нет окна/клика как в оригинале (пока только JS: `__transport_ship_embark`).
- [x] **F1. Баг данных `enemies.js`** — у canaanite/kushite/nubian/phoenician/seapeople
      `percentage_type3 > 0` (10–20%) при `figure_types[2] = FIGURE_NONE` — доля армии
      не спавнилась. Затрагивало миссии **11** (Canaanite), **13/15** (Nubian), **16**
      (Kushite). Исправлено: доля type3 сложена в type2, `percentage_type3=0` (как у
      Libyan/Roman); суммы = 100 у всех наций. Assyrian/Hyksos (chariot 10%) оставлены под F2.
      `TODO(F2)` у каждой исправленной нации.
- [x] **F2. Вражеские колесницы** — класс `figure_enemy_chariot` (наследует
      `figure_enemy_fast_sword`, мелейная механика марша/боя) + `FIGURE_METAINFO` на все 12
      `FIGURE_ENEMY_*_CHARIOT` (`src/figuretype/figure_enemy_chariot.{h,cpp}`); статические
      параметры (анимации/бой) из конфигов `figure_<nation>_chariot` в `enemies.js`. Раньше
      незарегистрированный тип → `assert(false)` в debug (`figure_impl.cpp:35`). Ассирийцы и
      гиксосы спавнят колесницы (type3=10%) — **миссии 32/33 разблокированы**. Регресс-тест
      `tests/39_enemy_chariot_registered.js` (+ биндинг `__test_enemy_figure_registered`).
      egyptian/hittite/persian оставлены с колесницей в `figure_types[2]` при `type3=0`
      (класс теперь есть; включение доли — data-решение по оригиналу, как и у 4 наций из F1).

### Часть 4 — Скриптование миссий 11–37 (блок D)

Заскриптованы 0–11; шаблон новой миссии — D0 в задачнике. Осталось **26 миссий** (Старое
царство — продолжение, Среднее и Новое царство). Скрипты: `src/scripts/mission/`, реестр
`src/scripts/missions.js`, статусная таблица `docs/wiki/player/missions/index.html`.
Зависят от частей 1–3.

- [x] **D1. Миссия 11 — Serabit Khadim** (`fa91fa687`; wiki-страница дописана 2026-07-16).
- [x] **D1b. Доводка миссии 11 — сверка с оригиналом** (2026-07-24): dump
      `45_mission11_pak_dump` / `load_mission_pak_raw`; торговля Men-nefer/Abu/Behdet/
      Nekhen/Selima из pak (Kebet убран); враг `ENEMY_7_LIBIAN`; запросы и 7 timed-raid
      в JS; wiki обновлена. Цепочки `EVENT_TYPE_INVASION` из бинарника → B2.
- [x] **D2. Миссия 12 — Meidum** — заскриптована с **временным monument goal**
      (31 при скриптовании, **сейчас 58 после F3**; малая + средняя ступенчатые):
      оригинал требует комплекс ступенчатой пирамиды
      (goal 39), который не реализован (C1). `TODO(C1)` в скрипте; расхождения
      задокументированы в задаче D2 и wiki.
- [x] **D3. Миссии 13 Buhen / 14 South Dahshur** — заскриптованы (пара, обе → 15).
      Buhen: `BUILDING_SMALL_OBELISK` в buildings (C7 каркас), цель monuments 9.
      South Dahshur: bent pyramid реализован (C4 закрыт) — цель возвращена на 21.
- [x] **D4. Миссия 15 North Dahshur** — заскриптована (сход веток 13/14; `choice[]` → 16/17).
      Истинная пирамида (C3) заменена двумя ступенчатыми (врем. цель 31 вместо 32). `TODO(C3)`.
- [x] **D5. Миссии 16 Iunet / 17 On / 18 Rostja** — заскриптованы (16/17 пара → 18;
      18 сход + `choice[]` → 19/20). Iunet: мастаба реализована (цель 9 как в оригинале).
      On: 3 мастабы теперь дают 18 (оригинал) после исправления формулы рейтинга (F3). Rostja:
      Сфинкс (C6) + комплекс/пирамида (C3) заменены доступными монументами (врем. цель 33
      вместо 53). `TODO(C3+C6)`.
- [ ] **D6–D9. Миссии 19–37** — по эпохам; зависимости от блоков B/C/E указаны в задачнике.
- [ ] **Кампания Cleopatra (38–52)** — enum'ы и wiki есть (`src/game/mission.h`), скриптов
      нет ни одной миссии. 4 линейные кампании без `choice` (задача B1b):
      Valley of the Kings (38–40), Ramses II (41–44), Ancient Conquerors (45–47),
      Cleopatra's Capital (48–52).
      **Приёмка:** полный прогон требует Cleopatra packs (`Data/Expansion.sg3` /
      `SprMain2.sg3`); Pharaoh-only — только smoke ранних миссий Pharaoh (см. **DX2**).

### Часть 5 — Здания из меню без реализации (нет BUILDING_METAINFO)

- [ ] **Zoo** — `BUILDING_ZOO` в меню развлечений (`building_menu.js:50`); связан с
      отсутствующим FIGURE_ZOOKEEPER.
- [ ] **Artisans Guild** — `BUILDING_ARTISANS_GUILD` в меню гильдий (`building_menu.js:106`);
      остальные три гильдии реализованы.
- [ ] **Brick Wall** — `BUILDING_BRICK_WALL` в меню обороны (`building_menu.js:181`);
      planner обрабатывает только `BUILDING_MUD_WALL` (`build_planner.cpp:1087`).

## P2 — Фигуры и юниты без реализации (enum есть, класса нет)

- **Перенесены в P1 (часть 3, блок F):** вражеские колесницы (F2 — блокируют миссии 32/33)
  и баг данных `enemies.js` (F1 — деградирует миссию 11). Детали — в REMAKE_TASKS_P1.md.
- [~] **F3. Формула рейтинга монументов** — 🟡 форма исправлена: `6.32·√Σ+0.5` →
  аддитивная `2.25·Σ+4.5` (`city/monuments.js`); 1 мастаба=9, 3 мастабы=18, средняя
  ступенчатая=40 (≥ Saqqara-19 из `.pak`). Цели миссий 12/14/15/18 пересчитаны, 17 On
  вернулась к оригиналу 18. **Осталась калибровка per-type весов по данным оригинала**
  (веса-плейсхолдеры; уедут в per-building конфиги). Обнаружено при D5.
- [ ] **Верблюды/слоны/конные лучники египтян** — только enum'ы (`FIGURE_ENEMY_EGYPTIAN_CAMEL/
      ELEPHANT/MOUNTED_ARCHER`), упомянуты в звуках/картинках (`figure_enemy.cpp:91-92`),
      ни конфигов в `enemies.js`, ни классов.
- [ ] **Существа проклятий/чумы**: locust (саранча Осириса), frog (лягушки Птаха), asp, scorpion, lion.
      Связано: заглушки проклятий — `perform_malaria_plague()` в `src/city/city_religion_bast.cpp:23`
      и `perform_frogs()` в `src/city/city_religion_ptah.cpp:14`.
- [ ] **Погребальный/сюжетный контент**: FIGURE_PHARAOH, mummy, tomb artisan,
      funeral walker (нужны для гробниц/монументов).
- [ ] Прочие: missionary (частично упомянут в `figure/service.cpp:43`, класса нет),
      showman, plagued citizen, zookeeper (`src/figure/figure_type.h`).
- [ ] Мелкие TODO зверей: атака страуса/антилопы/птиц на людей
      (`animal_ostrich.cpp:110`, `figure_antelope.cpp:106`, `figure_birds.cpp:77`);
      «убегание» delivery boy / caravan donkey от угроз.

## P2 — Симуляция и карта

- [ ] **Дамбы (dikes, Cleopatra)** — не найдены нигде в `src/`; нужны для поздних миссий.
- [ ] Обработчик конца разлива (`cycle_end+1`) не реализован —
      `src/city/city_floods.cpp:368` («figures/boats?»).
- [ ] Каналы: стыковка с дорогами и выход в Нил — TODO в `src/grid/canals.cpp:231,259`.
- [ ] `map_tiles_set_water` помечен «todo: broken» (`src/grid/tiles.cpp:462`);
      углы травы «doesn't work yet» (`:768`).
- [ ] Prosperity: бонус за «food_types_eaten» закомментирован (`src/city/rating_prosperity.cpp:26`);
      `city_finance.cpp:195` — «fix this calculation».

## P2 — UI и оверлеи

Каркас оверлеев готов, но часть закомментирована в `src/scripts/ui_overlay_menu.js`:

- [ ] **Товарные оверлеи** (весь набор): grain, chickpeas, pomegranates, figs, meat, game,
      pottery, jewelry, linen (активен только brewery).
- [ ] Здоровье: disease, infected housing, malaria (есть только malaria *risk*).
- [ ] Администрация: administration problems, water crossings, empty housing, magistrate.
- [ ] Группа «Other»: irrigation, city defenses, hide cliffs.
- [ ] Плейсхолдеры в сообщениях: `[amount_granted]`, `[time_until_attack]`, `[travel_time]`,
      `[god]` — `src/window/message_dialog_new.cpp:204-209`.
- [ ] Инфо-окно фермы не показывает данные разлива/ирригации —
      `src/scripts/ui_farm_info_window.js:57-59`.
- [ ] Усталость экипажа warship не симулируется (всегда «Rested») —
      `src/window/window_figure_warship_info.cpp:71`.
- [ ] История событий (records) использует placeholder-данные источника —
      `src/game/game_events_history.h:34`.

## P2 — Локализация

Полные языки: en, de, ru, ru_f, **hu** (венгерский добавлен #612, `81d7e4f2d`:
`eventmsg_hu.js` + `game_messages_hu.js`). У остальных 8 (fr, it, sp, po, pr, kr, ci, ch)
есть только верхнеуровневая таблица (~150–200 строк), а `localization_base_XX`, `eventmsg_XX`,
`game_messages_XX` отсутствуют на диске:

- [ ] **L1. Локали-заглушки fr/it/sp/po/pr/kr/ci/ch.** Либо добить `localization_base_*` +
      `eventmsg_*` + `game_messages_*`, либо убрать из списка `src/scripts/localization.js`,
      пока не готовы (меньше «пустых» языков у игроков).
- [ ] **L2. hu smoke.** Короткий чеклист после #612: `--language hu` до main menu / брифинга
      миссии 0–1 (с Pharaoh data); ловить missing strings / fallback на en. Связано с DX
      (репорт Pharaoh-only часто с `--language hu`).
- [ ] Перевести base UI + event messages + game messages для fr / it / sp / po / pr / kr /
      chinese (simpl./trad.) — детализация **L1** «добить», не «убрать».

## P2 — Редактор сценариев (восстановление)

Разведка кода 2026-07-20. **Итог:** редактор в исходниках почти целиком присутствует и
**полностью компилируется** (все `.cpp` собираются через `file(GLOB)`, ничего не `#ifdef`-нуто),
но **недостижим из UI** (точка входа не подключена) и **не умеет грузить/сохранять сценарии**
(обе функции persist — заглушки). Движок рисования карты (кисти рельефа, дороги, точки
входа/выхода/вторжений, флаг-фигуры) — рабочий, ему нужна интеграционная проверка, а не переписывание.

> **Precondition (до кода):** согласовать с мейнтейнером через Issue/Discord — редактор как
> dev-инструмент или фича для игроков. `CONTRIBUTING.md` отклоняет крупный UI без обсуждения;
> редактор именно такой. Не начинать без этого решения.

Дерево кода: `src/editor/` (bootstrap + tool engine), `src/scenario/editor*.cpp` (модель атрибутов
сценария), `src/window/editor/` (UI-панели), `src/widget/{map_editor,top_menu_editor}.cpp` и
`src/widget/sidebar/editor.cpp` (HUD), `src/figuretype/editor.cpp` (флаг-фигуры),
`src/game/file_editor.cpp` (сессия), `src/game/game.cpp:422-442` (`game_init_editor`/`game_exit_editor`).

Порядок восстановления (по возрастанию объёма):

- [ ] **ED1. Подключить точку входа (мелкая).** `game_init_editor()` (`game.cpp:422-435`) полностью
      рабочая, но **у неё ноль вызовов** — мёртвый код. Главное меню (JS `ui_main_menu.js:11-16`)
      не содержит пункта редактора; CLI-флага тоже нет. `window_editor_map_show` уже проброшен в JS
      (`ui_common.js:108`). **Сделать:** биндинг `__game_init_editor` → `game_init_editor()`, пункт
      в `ui_main_menu.js` (по желанию — гейт через `editor_is_present()`, проверяет наличие 9 C3-ассетов
      типа `c3map.sg2`; строки `#TR_NO_EDITOR_MESSAGE` уже есть). Флаг режима `editor_is_active`
      (`src/editor/editor.cpp:17`) работает — симуляция его уважает (`game.cpp:116-120` ранний выход).
- [ ] **ED2. Включить верхнее меню редактора (мелкая).** `src/widget/top_menu_editor.cpp:66-72` —
      массив `menu[]` пустой (реальные пункты File/Options/Help/Resets/Empire закомментированы),
      `menu_bar_*`-вызовы закомм. (`:96,117,126-146`) → панель рисуется декоративной полосой, ввод
      игнорируется. Обработчики New/Load/Save/Exit и пр. **реализованы** (`:158-248`), но недостижимы.
      **Сделать:** раскомментировать `menu[]` и `menu_bar_draw/handle_mouse`. Заодно вернуть
      load/save-хоткеи в `src/window/editor/window_editor.cpp:59-65` (тело закомм.).
- [ ] **ED3. Persist сценариев — грузить/сохранять (средняя–крупная, ключевой блокер).**
      - `GamestateIO::write_mission()` — заглушка `return false;` (`src/io/gamestate/boilerplate.cpp:612-615`;
        та же запись в P3-IO ниже).
      - `game_file_editor_write_scenario()` (`src/game/file_editor.cpp:141-154`): подготовка есть,
        реальный `game_file_io_write_scenario()` закомм. (`:152`) → `return 0` (провал).
      - `game_file_editor_load_scenario()` (`:132-139`): чистит карту, но `game_file_io_read_scenario()`
        закомм. (`:134`) → возвращает «успех», загрузив пустую карту.
      - JS-диалоги уже подключены к этим заглушкам (`ui_file_dialog_save.js:103` → `__game_editor_write_scenario`,
        `ui_file_dialog_load.js:130` → load). Каркас сериализации рабочий: `write_savegame`
        (`boilerplate.cpp:617`) и `write_map` (`:637`) реализованы — переиспользовать их chunk/FILEIO-схему.
      - **Открытое решение (формат):** (а) свой chunk-формат — **рекомендуется**, поверх готовых
        `write_map`/`write_savegame`, низкий риск; оригинальный `.pak` остаётся load-only. (б) запись
        оригинального `.pak` — реверс junk-чанков формата (см. P3-IO), крупная и рискованная, вынести
        в отдельную дальнюю задачу. Выбрать (а) для первого захода.
- [ ] **ED4. Восстановить модель requests в редакторе (средняя).** В `src/scenario/editor.cpp`
      весь request-слой закомментирован: `scenario_editor_request_get/sort_requests/_request_delete/
      _request_save` (`:103-143`) и init-цикл в `scenario_editor_create` (`:86-89`) — массив
      `g_scenario.requests[]` убран/перенесён. UI (`window/editor/requests.cpp`) рисует 20 пустых
      слотов, правки не сохраняются. Рантайм-путь запросов (`EVENT_TYPE_REQUEST` →
      `scenario_request_activate`) **живой** — сломана именно редакторская половина. **Сделать:**
      восстановить хранилище или переложить на новый event-manager.
- [ ] **ED5. Рантайм-потребители редакторских данных (средняя).** Редактор умеет авторить то,
      что рантайм молча игнорирует:
      - `EVENT_TYPE_INVASION` — no-op (`scenario_event_manager.cpp:544-546`); это **задача B2** выше.
        Редактор полноценно создаёт вторжения (`editor.cpp:145-185`, `window/editor/invasions.cpp`,
        `edit_invasion.cpp`, точки в `editor_map.cpp`) — на рантайме они мертвы.
      - price/demand changes: авторинг есть (`editor.cpp:187-271`), рантайм-кейсы
        (`scenario_event_manager.cpp:562-569`) только шлют сообщение — проверить, применяют ли эффект.
      - флаг `gameplay_fix_editor_events` (`src/scripts/game.js:99`, `city/victory.js:104-107`) намекает
        на известные проблемы с редакторскими win/survival-условиями — верифицировать этот путь.
- [ ] **ED6. Интеграционная проверка движка карты (мелкая–средняя).** Кисти рельефа
      (`src/editor/tool.cpp`, 651 строк: grass/trees/water/…/ore, raise/lower, road, ramp, точки),
      ограничения (`tool_restriction.cpp`), viewport-ввод (`widget/map_editor.cpp`), флаг-фигуры
      (`figuretype/editor.cpp`) — реализованы. Нужна проверка после ED1–ED3, не переписывание.
      Мелкий мусор: баннер `// TODO !!!!!!` (`src/scenario/editor_map.cpp:6`), закомм. `memset`
      в `scenario_editor_create` (`editor.cpp:21`), старые clip/flag-блоки в `map_editor.cpp:49-79`.
      **Тест (после ED3):** интеграционный round-trip — войти в editor mode → поставить тайлы/точки →
      save → load → сверить карту (защита от регрессий). **Перед стартом спайка** проверить наличие
      9 C3-ассетов для `editor_is_present()` — иначе вход в редактор не откроется.

**Порядок — вертикальные срезы (каждый проверяем самостоятельно):**
- **Срез A (спайк): ED1 + ED2.** Цель не «сделать», а подтвердить, что движок карты жив
  (войти, поводить кистями, точки) — до вложений в persist. Всплывёт поломка — переоценить объём.
- **Срез B: ED3** — load/save (после того, как спайк показал, что редактор рисует).
- **Срез C: ED4 + ED5** — requests и рантайм-потребители поверх рабочего save/load; пересекается с B2.

**Мин. играбельный редактор:** ED1 + ED2 + ED3 (войти → нарисовать карту → сохранить/загрузить).

## P3 — Сохранения и IO

- [ ] Запись сейва только в собственный `.svx`; оригинальный `.sav` — load-only
      (`write_savegame()` в `src/io/gamestate/boilerplate.cpp:614`). Решить, нужна ли запись
      в оригинальный формат (полная «savegame compatibility» в обе стороны).
- [ ] `write_mission()` — заглушка (`boilerplate.cpp:609`): нельзя сохранять mission pak.
      **= ядро ED3** (persist сценариев редактора) — одна и та же работа, не дублировать.
- [ ] ~15 уникальных chunk'ов формата читаются как «junk»/padding (31 `push_chunk("junk…")`
      по схемам в `src/io/gamestate/boilerplate.cpp`, буферы в `chunks.cpp`) — постепенно
      расшифровывать семантику.
- [ ] Back-compat записи empire objects для версий <160 — `src/empire/empire_object.cpp:386`.

## P3 — Тесты и инфраструктура

- [ ] Интеграционные тесты (`tests/`) покрывают UI, размещение зданий, точечную регистрацию
      типов и валидацию enemy-конфигов (V1, тест 42). Добавить: round-trip save/load, долгий
      прогон симуляции (детерминизм), экономика, прохождение миссий 0–10 автопилотом,
      боёвка/вторжения.
- [x] **TS1. Smoke-тест «прогон без TypeError».** `tests/41_city_smoke_run.js`: ставит ~12
      типов зданий реальным путём планировщика (`on_place → update_animation/graphic`),
      открывает инфо-окно каждого (`[es=(info_window_*, init)]`), прокручивает сим; скан
      драйвера на `!!! TypeError:` (по всему логу) ловит краши широко — тот же **класс**, что
      баг дока (`e2756c9eb`), но для зданий без отдельных тестов (их ~15). Per-type маркеры
      `smoke_ok:*` изолируют виновника, `smoke_skip:*` логируется громко (анти-деградация —
      сработала на ревью: первый прогон упал на `missing smoke_ok:bandstand`).
      **Границы (честно):** это сетка на КРАШИ, не на корректность; НЕ воспроизводит
      entertainment-null-дереф (нужен невалидный bid — закрыто guard'ом из **N1**); не берёт
      ветки, требующие населённой экономики. Здания, которые харнесс не может легально
      поставить (entertainment — нужна дорога), — fast-spawn через `__test_building_create`
      (прогоняет только init-окна, не `on_place`). Связано с **N1**.
- [x] **V1. Валидатор конфигов врагов** — интеграционный тест `tests/42_enemy_config_valid.js`.
      Для всех 13 `enemy_*` в `enemies.js`: `percentage_type1+2+3 == 100`, ненулевая доля
      имеет реальный (не `FIGURE_NONE`) `figure_types[i]` (класс **F1**), и каждый объявленный
      тип резолвится в зарегистрированный enemy-класс через `__test_enemy_figure_registered`
      (класс **F2**) — читает конфиги напрямую из JS-глобалов, ловит на CI, а не падением
      в конкретной миссии. Маркеры `enemy_config_control_ok` / `enemy_config_all_valid`;
      прогон `--integraltest-only 42_enemy_config` → PASS (1/0). Строка в `tests/README.md`.
      Игровую логику не трогает (CONTRIBUTING-safe). Связано с тестом 39 (F2).
- [ ] Автосейв — только ежемесячный `autosave_month.svx` (`src/game/game.cpp:184-186`),
      нет настраиваемого интервала и ротации слотов.
- [ ] Wiki: детальные страницы есть только у 3 миссий (nubt, thinis, perwadjyt) —
      дописать nekhen…saqqara (`docs/wiki/player/missions/`), обновлять по мере скриптования 11+.
- [ ] Порты Vita/Switch: исходники в `src/platform/{vita,switch}/` есть, но сборка не подключена
      в CMake — восстановить или официально убрать.
- [ ] Продолжающаяся миграция логики C++ → MuJS (здания/оверлеи/окна) — текущий рабочий поток,
      см. историю коммитов. Связанные баги/техдолг: **H1**, **J1**, **J2**, **FP1**, **T1**, **T2**
      (ниже).
- [ ] Мультиплеера нет (`src/net/` — только HTTP к GitHub) — вне скоупа ремейка, зафиксировано
      для полноты.

## P3 — Старт / Pharaoh data (DX)

Контекст (2026-07-24): master раньше hard-fail'ил без Cleopatra packs; пользователи с
Pharaoh-only (и Linux case-symlink'ами) получали «Pharaoh data required», хотя `ra2606`
стартовал. Сейчас: soft-warn Continue/Quit + CLI `--nodatacheck`. Код:
`src/platform/akhenaten.cpp`, `innoextract_util.{h,cpp}`, `options_window.cpp`,
`arguments.{h,cpp}`; state — `xvalue<innoextract::settings_t>`.

- [ ] **DX1. Документация install check.** README всё ещё пишет, что Cleopatra обязательна
      и demo «rejected at startup». Обновить под soft-warn + `--nodatacheck` (+ кратко в
      `CLAUDE.md` / wiki install, если есть). Не обещать полноту без Cleopatra.
      **Файлы:** `README.md`, `CLAUDE.md`.

- [ ] **DX2. Graceful degrade без Cleopatra packs.** После Continue Pharaoh-only пройти
      критичные пути (image paks / шрифты / UI / миссии без Cleo-ассетов): либо явный
      fallback, либо disable фич с логом — не поздний crash. Связано с приёмкой кампании
      38–52 (нужны packs) и B1b.
      **Файлы:** `src/graphics/image*.cpp`, `font.cpp`, load paks; смотреть логи
      `missing Cleopatra packs`.

- [ ] **DX3. Тест install checks.** Unit/integral на `has_pharaoh_data` /
      `has_required_game_files` / `--nodatacheck` без полного Pharaoh: temp dirs с/без
      `campaign.txt` и `Data/SprMain2.sg3` (или stub-файл). Маркеры + строка в
      `tests/README.md`.
      **Файлы:** `src/platform/innoextract_util.cpp`, `integral_tests.cpp` или `tests/*.js`.

- [ ] **DX4. Linux case-fold для data paths.** На case-sensitive FS пользователи вручную
      делают `AUDIO→Audio`, `data→Data`. Проверять/резолвить типичные варианты
      (`Data`/`data`, `Expansion.sg3`/`expansion.sg3`, `SprMain2.SG3`) в
      `has_required_game_files` / VFS base, без требования symlink'ов.
      **Файлы:** `src/platform/innoextract_util.cpp`, при необходимости VFS.

- [ ] **DX5. Правило: не автоподменять настроенный Pharaoh-only.** Уже частично в
      `akhenaten.cpp` (Steam / PharaohData bootstrap только если нет `campaign.txt` в
      текущем `data_directory`). Зафиксировать в `CLAUDE.md` / комментарии у bootstrap;
      не регрессировать при правках installer flow.

- [ ] **DX6. Warning Cleopatra: кнопка «Find data…».** Сейчас Continue/Quit. Добавить
      действие открыть options / выбрать `data_directory` (полный Pharaoh+Cleopatra),
      не только выход.
      **Файлы:** `src/platform/akhenaten.cpp` (`confirm_continue_without_cleopatra`),
      `options_window.cpp`.

- [ ] **DX7. First-run UX.** Если нет `data_directory` и нет `Installer/*.exe` — одна
      понятная страница/диалог: куда указать Pharaoh, куда положить GOG/Inno Setup.exe;
      не цепочка «Pharaoh data required» → options → снова ошибка.
      **Файлы:** `akhenaten.cpp`, `options_window.cpp`.

- [ ] **DX8. AppImage / Flatpak / Steam Deck data discovery.** Типичный Linux path pain
      рядом с DX4: поиск data рядом с AppImage, `XDG_*`, Steam compatdata. Smoke:
      «доходит до main menu» с валидным `data_directory`.
      **Файлы:** `platform_unix.cpp`, `arguments.cpp`, docs.

- [ ] **DX9. innoextract progress UX.** Отмена extract job; понятный fail «это demo Setup»
      vs full Cleopatra; статус в options не только строкой.
      **Файлы:** `innoextract_util.cpp`, `options_window.cpp`.

- [ ] **DX10. Community sticky: Linux Pharaoh data.** Issue/Discord/wiki: Cleopatra packs,
      case-sensitive symlink’ы, soft-warn, `--nodatacheck`, куда писать `data_directory`.
      Цель — меньше повторных тикетов как у sirkalmi (2026-07-24).

## P3 — QA / регрессии миссий

Рядом с D-блоком: ловить рассинхрон скриптов и оригинала раньше, чем руками на миссии 19+.

- [ ] **QA1. Mission golden dumps.** Расширить `mission_pak_dump` / `js_test_mission_pak_dump`
      → baseline vs оригинал для миссий 0–18 (win criteria, empire, requests, starting funds).
      Маркеры + опционально `--integraltest-only` на одну миссию.
      **Файлы:** `src/js/js_test_mission_pak_dump.cpp`, `tests/45_*.js` (или рядом), wiki
      Developer Reference.

- [ ] **QA2. Deterministic tick harness.** N месяцев на фикс. сейве + hash ключевых гридов /
      finance/population (ловля рассинхрона economy/flood). Не блокер геймплея; полезно
      после правок floodplain/canals.
      **Файлы:** `tests/`, биндинги snapshot в `js_test_*.cpp`.

- [ ] **QA3. Crash triage без Cleopatra packs.** Каталог известных падений после Continue
      (Pharaoh-only) с минимальным repro → корм для **DX2**. Живёт в `REMAKE_NOTES.md`
      или wiki troubleshooting.

## P3 — Архитектура старта (AS)

- [ ] **AS1. Вынести install bootstrap из `akhenaten.cpp`.** Сейчас setup раздут (extract /
      Steam / PharaohData / pending installer / pre_init). Модуль `pharaoh_data_bootstrap`
      (или рядом с `innoextract_util`) — проще тестировать DX3/DX5.
      **Файлы:** `akhenaten.cpp` → новый `src/platform/*bootstrap*`.

- [ ] **AS2. Content readiness report при старте.** Один лог (и опционально строка в options):
      campaign ✓/✗, Cleopatra packs ✓/✗, fonts fallback, audio probing skipped. Не UI-дашборд.
      **Файлы:** `innoextract_util` / `akhenaten.cpp` / `options_window.cpp`.

- [ ] **AS3. Правило агентов: не Unicode mass-rewrite.** Не делать массовый rename/replace
      через PowerShell `Set-Content` (сносит BOM и `—`/`²`/`→`). Предпочтительно
      StrReplace / `rg`+patch / clang-format. Зафиксировать в `CLAUDE.md` или
      `.cursor/rules` при наличии.

## P3 — Продукт / процесс (обсудить, не кодить вслепую)

- [ ] **PC1. Редактор: Issue «dev-only vs player feature».** Precondition для ED1–ED6 уже
      в блоке P2; завести явное GitHub Issue / Discord и не начинать крупный UI без ответа
      (`CONTRIBUTING.md`).

- [ ] **PC2. Backlog «Enhanced later».** Явный список «не сейчас» (autosave slots, overlays
      polish, QoL), чтобы PRы не уезжали в polish вопреки CONTRIBUTING. Можно секция в
      этом файле или wiki.

- [ ] **PC3. Wiki sync из mission scripts.** Детальные страницы только у ~3 миссий при
      0–18 в скриптах — шаблон/генератор Developer Reference (paths, line numbers, message
      IDs) из `m_NNN_*.js`. Связано с правилом wiki в `CLAUDE.md`.

## P3 — MuJS биндинги и house/farm (после farm preview / evolve_text)

Выявлено 2026-07-18 при переносе farm ghost_preview в JS и фиксе `House.evolve_text`
(`archive_helper::coerce` для `xstring`). Уже закрыто в той же волне: coerce `xstring`/`vec2i`/`tile2i`,
farm placement tests 34/35, `evolve_text` roundtrip test 36 (без UI), `sound_channel` ферм в params,
preview crops/image в `farm.js`.

Порядок: ~~**H1**~~ ✅ → ~~**J1**~~ ✅ → **T1** / **J2** / **T2** по возможности;
**FP1** — когда снова трогаете фермы, не блокер. Осталось из P3-волны: **FP1**, **S1**, **S2**, **N1**.

- [ ] **N1. Null-guard для `city.get_X(...)` в JS-хендлерах.** Валидирующие геттеры
      (`get_dock`/`get_farm`/`get_monument`/`get_entertainment_building`/`get_roadblock`)
      возвращают `null`, если bid не того типа (напр. во время `on_place`, когда здание ещё
      не `BUILDING_STATE_VALID`). Дереф без проверки → `TypeError: cannot convert null to
      object`. Уже пойман и закрыт в `dock.js` (fix `e2756c9eb`); `farm.js` защищён.
      **Найдено и исправлено при F2-ревизии:** `ui_bandstand_window.js`, `ui_booth_window.js`,
      `ui_pavilion_window.js` дереференсили `city.get_entertainment_building(window.bid)
      .meta_text_id` **без guard** — добавлен `if (!b) return` в init-хендлеры всех трёх
      (прогон 39/39). **Осталось (общий принцип):** при переносе новой логики зданий в JS —
      всегда проверять результат валидирующего геттера; автоматизацию даёт **TS1**
      (smoke-прогон без TypeError). Связано с блоком **G** (контракт `es(__func__)`).

- [x] **H1. House `worst_desirability_building_id` — TypeError из JS.** ✅ уже устранено
      волной «Standardize building runtime property access for MuJS» (`ce756fd88`) +
      xstring coerce (`56c94e775`). Причина была НЕ в отдельном поле: `building_id`(uint16)
      идёт через тот же generic `{}`-аксессор, что и `evolve_text`. После стандартизации:
      House имеет свой `__property_getter` (читает `runtime_data`→`base` через `archive_helper`),
      сеттер наследуется от `Building.prototype.__property_setter` (компилятор MuJS синтезирует
      ИЗ пустого `{}` И getter, И guard-сеттер — `jscompile.cpp: try_proto_property_define`),
      а запись доходит до `building_house::set_property` (`BUILDING_RUNTIME_DATA_IMPL(building_house)`).
      **Проверено (репро не воспроизводится):** write/read int-свойства (42→0) и полный путь
      `house_determine_worst_desirability_building` + `house_determine_evolve_text` работают без
      TypeError — и в embedded-скриптах, и с `--mixed`.
      **Регрессия добавлена:** `tests/36_house_evolve_text_property.js` — маркеры
      `house_worst_desirability_int_roundtrip_ok` (int roundtrip) и
      `house_info_window_determine_ok` (UI-ветка: оба determine-хелпера без throw).
      **Прогон:** `--integraltests --integraltest-only 36_house … --no-resource` → 5/5 маркеров, PASS.
      **Осталось (не H1):** ручная проверка реального info-окна на населённом доме при мерже.

- [x] **J1. Безопасная передача `color` / `COLOR_MASK_*` из MuJS в C++.** ✅
      Полные маски (`COLOR_MASK_GREEN` = `0xff18ff18` и т.п.) > `INT_MAX`; `js_tointeger`
      портил значение → спрайты невидимы/неверны. `_30`-маски (< INT_MAX) проходили.
      **Сделано:** (1) `js_to_value<unsigned int>` теперь через `js_touint32` (канонический
      ECMAScript ToUint32, сохраняет весь 32-битный диапазон) — это спец-я для `color`, т.к.
      `color` == `uint32_t` == `unsigned int` (`src/js/js_game.h`). (2) color-параметры
      биндингов `__city_planner_draw_flat_tile` / `__city_planner_draw_overlay_tile` сменены
      с `int color_mask` на `color` (`src/js/city_planner_js.cpp`); каст `(color)` убран.
      (3) `COLOR_MASK_RED` (> INT_MAX) добавлен в JS-константы (`js_constants.cpp`) — раньше
      был только `COLOR_MASK_RED_30`. Аудит: только эти два биндинга принимали цвет из JS;
      прочие (`draw_from_below`/`draw_ghost`/`draw_isometric_ghost`) хардкодят маску в C++.
      **Регрессия:** `tests/38_color_mask_passing.js` + test-биндинг `__test_color_roundtrip`
      (`js_test_game.cpp`): GREEN/RED/BLUE roundtrip (проверка `> INT_MAX` и точного равенства)
      + реальный `draw_flat_tile(pixel, COLOR_MASK_GREEN)` без TypeError.
      **Прогон:** `--integraltests --integraltest-only 38_color --no-resource` → 4/4 маркеров, PASS;
      тесты 1–20 остались зелёными (общий заголовок не сломан).
      **Docs:** `src/js/CLAUDE.md`, `src/building/CLAUDE.md` (раздел ghost_preview), `tests/README.md`.
      **Прим.:** `farm.js` пока оставлен на `_30`-масках (это полупрозрачный вид превью, а не
      только обход) — менять визуал не в скоупе J1; при желании можно перейти на полные маски.

- [x] **J2. Конвенция draw-API для JS preview.**
      Зафиксировано в `src/building/CLAUDE.md` (раздел ghost_preview), `src/js/CLAUDE.md`,
      ссылка из `tests/README.md` у farm placement tests. Правила: маска в C++ до J1;
      без тонких `__foo_draw_*`; порядок ghost → from_below/overlay; логика в JS.

- [x] **T1. Smoke для farm ghost preview (image/crops).**
      `tests/37_farm_preview_images.js`: `first_img("crops")` ≠ 0; routing
      `building_farm_get_image` (meadow → farm_house, floodplain → farmland+fert);
      `draw_from_below` без TypeError. (При `--no-resource` id farm_house/farmland
      могут быть 0 — проверяется равенство маршруту, не «> 0».)

- [x] **T2. Правило: `ANK_CONFIG_PROPERTY` / `ANK_CONFIG_STRUCT` только global scope.**
      Макрос открывает `namespace archive_helper`; внутри anonymous namespace MSVC даёт C2988
      (уже ловили в `integral_tests.cpp`).
      Сделано: комментарий у макросов в `src/core/archive.h`, заметка в `src/CLAUDE.md` /
      `src/building/CLAUDE.md`, комментарий у unit-теста в `integral_tests.cpp`.

- [ ] **FP1. Свести farm image/crops к одному пути (техдолг).**
      Preview: `building_farm_get_image` / `building_farm_draw_crops` в `farm.js`.
      Placed farms / tiles: всё ещё `building_farm::get_farm_image` / `draw_crops` в C++
      (`map_building_tiles_add_farm`, ornaments). Дублирование логики offsets/fertility.
      **Сделать (по желанию, когда снова трогаете фермы):** ornaments/tiles через JS
      или оставить C++ единственным источником и звать его из preview без копии алгоритма.
      Не блокер геймплея. Делать после H1/J1.
      **Файлы:** `src/scripts/building/farm.js`, `src/building/building_farm.cpp`.

- [ ] **S1. Empty All не затирает кастомные заказы.**
      Сейчас STOP ставит всем `STORAGE_STATE_ACCEPT` (#608 hotfix). Get/Refuse до START
      теряются. Модель и грабли — `REMAKE_NOTES.md` §8.
      **Сделать:** при STOP восстанавливать заказы до Empty All (снимок) **или** снимать
      только Empty у товаров, выставленных Empty All, не трогая ручные Accept/Get/Refuse.
      **Приёмка:** Accept+Get → START → STOP возвращает те же Accept/Get.
      **Файлы:** `src/building/building_storage.cpp` (+ при необходимости `.h`).

- [ ] **S2. Синхрон `empty_all` с ручным cycle / Accept None.**
      Пока Empty All включён, клик по товару или Accept None меняет стейты, а кнопка
      всё ещё «STOP emptying» — флаг и список расходятся.
      **Сделать:** при cycle/accept_none во время empty-all сбрасывать флаг **или**
      блокировать правки до STOP; UI должен совпадать со стейтом.
      **Приёмка:** нельзя получить «STOP» на кнопке при не-Empty заказах в списке
      (или флаг сбрасывается сразу при правке).
      **Файлы:** `building_storage.cpp`, `ui_*_orders_window.js`.

## P3 — Техдолг системы анимации зданий (`update_animation`)

Выявлено при рефакторинге `can_play_animation()` → `bool building::play_animation` +
`virtual void update_animation()` (флаг считается раз в день, `es(__func__)` отдаёт
финальное слово скриптам; рендер читает `base.play_animation` напрямую). Паттерн описан
в `src/building/CLAUDE.md`.

- [ ] **G1. Проверить смену поведения weaponsmith.** Оригинальный `can_play_animation()`
      звал `building_impl::can_play_animation()` в обход `building_industry` (пропуская
      проверку mothball), хотя класс наследует `building_industry`. При конверсии заменено
      на `building_industry::update_animation()` (`building_weaponsmith.cpp:18-19`) — это
      тихо изменило поведение (теперь mothball учитывается). Сверить с оригиналом Pharaoh:
      если так и должно быть — ок; иначе откатить на `building_impl::update_animation()`.
- [ ] **G2. Хрупкий контракт `es(__func__)`.** Сейчас `es` вызывается вручную в каждом
      override `update_animation()`. Часть классов зовёт родителя (который уже делает `es`),
      часть — нет → риск двойного `es` или его отсутствия. Вынести вызов `es(__func__)`
      централизованно (в место, откуда вызывается `update_animation()`), убрав из override'ов.
- [ ] **G3. Аудит всех `update_animation()`-override на корректный родитель.** Пройтись по
      `src/building/building_*.cpp`: каждый override должен звать `update_animation()` своего
      **непосредственного** базового класса (industry-здания — `building_industry::`), а не
      `building_impl::` в обход. Связано с G1.
- [ ] **G4. Размещение/сериализация `play_animation`.** Поле объявлено сразу после
      `char runtime_data[186]` в `building` (`building.h:247-248`) и **не сериализуется**
      (пересчитывается в `update_animation()`; `on_post_load()` выставляет его до первого
      `update_day`). Задокументировать это допущение и убедиться, что соседство с
      фиксированным `runtime_data`-буфером (кастуется в `runtime_data_t` через `dcast`)
      никого не собьёт.
- [x] **G5. Property-механизм без compile-time проверки.** Пустой `Building.property.x = { }`
      молча возвращал `undefined`, если поле не добавлено в `ANK_CONFIG_PROPERTY(building, …)`
      (`building.h:449`). Добавлена рантайм-диагностика: `building_proto___property_getter/
      __property_setter` (`building_js.cpp`) логируют `logs::error` с именем свойства и типом
      здания при обращении к незарегистрированному полю. Compile-time проверки нет (ограничение
      DSL), но молчаливый сбой теперь виден в логе.
- [x] **G6. `js_push_value` не покрывает все примитивы.** Push `uint8_t` давал LNK2019
      (кастили к `int` в `__building_get_water_stored`). Добавлен общий фолбэк первичного
      шаблона `js_push_value` для арифметических/enum-типов (`js_game.h`) с `static_assert`
      для остальных: непокрытые примитивы (`uint8_t`/`uint16_t`/`unsigned int`/enums) теперь
      пушатся как число, а неарифметические типы ловятся на компиляции. Касты `(int)` в
      `__building_get_water_stored` убраны.

---

## Рекомендуемый порядок работ

1. **Вражеский флот (E3) + вражеские колесницы** — открывает морские миссии и полные вторжения.
2. **Монументы «первой необходимости»**: большая ступенчатая пирамида → истинные пирамиды →
   сфинкс/обелиски — без них не заскриптовать миссии 11+ (Old Kingdom — эпоха пирамид).
3. **Ветвление кампании + event-invasions** — инфраструктура для скриптования новых миссий.
4. **Миссии 11–17 (Old Kingdom)** — следующий блок кампании, использует пункты 1–3.
5. Параллельно с D-блоком (не вместо): **DX2** (degrade без Cleopatra) + **QA1** (mission
   golden dumps) — меньше повторных тикетов Linux/Pharaoh-only и рассинхрона скриптов.
6. Существа проклятий (locust/frogs) + недостающие оверлеи — параллелизуемая мелочёвка.
7. Дальше по эпохам до 37-й, затем контент Cleopatra (дамбы, поздние монументы, миссии 38–52).
8. По желанию / после обсуждения: **L1** (заглушки языков), **PC1** (редактор), **DX7/DX10**
   (first-run + community sticky).
