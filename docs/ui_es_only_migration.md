# Plan: UI callbacks → ES-only

Migrate UI from runtime closures / `onclick:` function refs to declarative
`*_event` names plus `[es=(widget, name)]` handlers.

**Reference:** `src/scripts/ui_empire_window.js`

```js
help_button({ pos[0, 0], onclick_event: "help" })
close_button({ pos[0, 0], onclick_event: "close" })
```

(`onclick_event` wins over helper default `onclick:` in C++.)

## Goal (short)

- Push callbacks: `onclick_event` / `ondraw_event` / `onhover_event` /
  `oninput_event` / `ondoubleclick_event`
- Logic in `[es=(widget, name)]` only
- No `.onclick =` in init, no `onclick: function(){…}`, no factory
  `onclick: show_window_by_id("…")` in new/migrated UI

**Not in C++ yet:** `onrclick_event` (use named `onrclick:` until needed).

**Do not mix into these commits:** `textfn`/`checkedfn`, `memory=frame` draw
canon, top-menu item model, mass wipe of every `onclick: named`.

Canon also in: `src/js/CLAUDE.md` → «UI callbacks (ES-only)».

---

## Recipe (every small commit)

1. Replace `onclick: …` / `.onclick =` with `onclick_event: "name"`.
2. Add `[es=(window_id, name)]` handler.
3. Smoke: open window, click changed buttons, hot-reload if possible.
4. One window (or one clear theme) per commit.

Pattern:

```js
// element id is the ES event when onclick_event is omitted
arrow_game_up : arrowup({ pos[216, 60], tiny:false, allow_repeat: true })

[es=(speed_options_window, arrow_game_up)]
function speed_options_arrow_game_up(window) {
    emit event_change_gamespeed{ increase: true }
}

// explicit onclick_event when the name should differ from the element id
help_button({ onclick_event: "help" })
```

Indexed `button` rows (`param1` already works):

```js
cat_0: button({ …, param1: 0, onclick_event: "toggle_cat" })

[es=(popup_messages_window, toggle_cat)]
function popup_messages_toggle_cat_es(window, ev) {
    popup_messages_toggle_cat(ev.param1)
}
```

For `arrow` / `image_button`: use **unique event names** (`inc_tax` /
`dec_tax`) until Step H1. Do not rely on `param1` there yet.

---

## Commit checklist (simple → hard)

Do in order. Each checkbox ≈ one small commit (or one tiny PR).

### A — Rules + init (easiest)

- [x] **A1** Write short ES-only rule in `src/js/CLAUDE.md` (new UI: `*_event`
      only; no `.onclick =`; no anonymous/factory `onclick`; helpers like empire).
- [x] **A2** `ui_mission_briefing_window.js` — remove `window.back.onclick =`.
- [x] **A3** `ui_hold_festival_window.js` — help only
      (`button_help.onclick =` → `onclick_event` / `help_button` + `[es=…]`).
      Gods already ES.
- [x] **A4** `ui_mission_end_window.js` —
      `replay_mission.onclick = named` → `onclick_event`.

### B — One anonymous window at a time

Unique event names for arrows (no C++ change). Leave anonymous `checkedfn`
alone.

- [x] **B1** `ui_sidebar_window.js` — speed arrows only
      (leave `show_window_by_id` factories for later).
- [x] **B2** `ui_speed_options_window.js` — arrows + middle-mouse checkbox
      *click* (not `checkedfn`).
- [x] **B3** `ui_tax_collector_window.js` — tax arrows.
- [x] **B4** `ui_palace_window.js` — tax arrows.
- [x] **B5** `ui_sound_options_window.js` — ± / toggles.
- [x] **B6** `ui_donate_to_city_window.js` — amounts / ±.
- [x] **B7** `ui_popup_messages_window.js` — `cat_0…11` → one `toggle_cat` +
      `param1` (plain `button`).
- [x] **B8** `ui_main_menu.js` — Discord / Patreon / update anonymous only.
- [x] **B9** `ui_hotkey_editor_window.js`.
- [x] **B10** `ui_granary_orders_window.js`.
- [x] **B11** `ui_storage_yard_orders_window.js`.
- [x] **B12** `ui_advisor_monuments.js`.
- [x] **B13** `ui_trade_opened_window.js`.
- [x] **B14** `ui_messages_window.js` — help override.

**Done when A+B clear:** no `onclick: function` and no easy `.onclick =` left
(except mission_choice / features).

### C — Factories → named or event (still small, one file)

Replace `onclick: show_window_by_id("X")` with either:

- `onclick_event: "…"` + ES that `emit event_show_window{ id: "X" }`, or
- a **named** wrapper `function open_mods_window() { … }` + `onclick: open_mods_window`
  (legacy OK until section E).

Prefer event when touching the file anyway.

- [x] **C1** `ui_main_menu.js` — remaining `show_window_by_id`.
- [x] **C2** `ui_sidebar_window.js` — remaining factories (messages, overlay, bug).
- [x] **C3** `ui_dynasty_menu.js`.
- [x] **C4** `ui_advisor_imperial.js` / trade / mansion — `show_window_by_id`.
- [x] **C5** `ui_bazaar_window.js` (and similar one-liner orders openers if any).
- [x] **C6** Other stray `show_window_by_id(` in `ui_*.js` (grep sweep; one
      commit per file or one sweep commit if tiny).
      Done: `ui_resource_settings_window.js` help. Left for **H5**: top menu
      (`top_menu_show_window_by_id`). Helper `show_window_by_id` in `ui_common.js` stays.

### D — Named `onclick:` → event (boring, optional pacing)

Only when convenient; not required to finish A–C. One window per commit.
Examples: dynasty leftovers, victory, player_selection, file-dialog leftovers,
building-info `onclick: named` still on helpers. Skip top menu and dynamic windows.

- [x] **D1** `ui_dynasty_menu.js` — remaining named `onclick`.
- [x] **D2** `ui_player_selection.js` — named button `onclick` (list item
      callbacks left as-is).
- [x] **D3** `ui_victory_dialog.js` — named button `onclick`.
- [x] **D4** `ui_festival_square_window.js` — hold festival button.
- [x] **D5** `ui_new_career.js` — back button.
- [x] **D6** file-dialog cancel → `onclick_event: "go_back"` (chrome + per-dialog ES).
- [x] **D7** `ui_industry_office_window.js` — mothball / unmothball.
- [x] **D8** `ui_invasion_quick_battle_window.js` — resolve / bribe / wait.
- [x] **D9** `ui_difficulty_options_window.js` — difficulty arrows.
- [x] **D10** `ui_donate_to_city_window.js` — donate / cancel.
- [x] **D11** `ui_display_options_window.js` — cancel → `go_back`.
- [x] **D12** `ui_main_menu.js` — continue / editor / quit.
- [x] **D13** `ui_popup_messages_window.js` — ok / cancel.
- [x] **D14** `ui_advisor_monuments.js` — burial dispatch buttons.
- [x] **D15** `ui_mission_briefing_window.js` — difficulty arrows + ironwill.
- [x] **D16** `ui_report_bug_window.js` — cancel → `go_back`.
- [x] **D17** `ui_speed_options_window.js` — ok / cancel.
- [x] **D18** `ui_sound_options_window.js` — ok / cancel.
- [x] **D19** `ui_hold_festival_window.js` — cancel.
- [x] **D20** `ui_set_salary_window.js` — cancel (incl. mansion).
- [x] **D21** `ui_resource_settings_window.js` — trade import/export controls.
- [x] **D22** `ui_bazaar_window.js` — overlay / mothball.
- [ ] **D*** Pick next file with `onclick: named_fn`, wrap in id/`onclick_event` +
      thin `[es=…]`, commit, repeat.

---

## Later (hard / design — do last)

Keep these at the end. Do not interleave with A–C.

### H1 — C++: forward `param1`/`param2` on `image_button` + `arrow`

Today only `button`/`large_button` put params in the ES payload. Needed before
shared handlers on advisors / arrows via `param1`.

- [ ] **H1** Patch `ui.cpp` draw paths for image/arrow to match `egeneric_button`.

### H2 — Advisor strip (`show_advisor_window(…)`)

- [ ] **H2** After H1: `ui_advisors_window.js` — one `onclick_event` + `param1`,
      or unique events per advisor button.

### H3 — Campaign period factories + hover

- [ ] **H3** `ui_scenario_selection_campaign.js` —
      `campaign_period_select(n)` / `onhover` factories → events + params
      (needs H1 if on `large_button` params already OK; hover may need
      `onhover_event`).

### H4 — Dynamic runtime callbacks

- [ ] **H4a** `ui_mission_choice_window.js` — loop `point_btn.onclick = …`.
- [ ] **H4b** `ui_window_features.js` — `option.onclick = f.toggle` (+ later
      `checkedfn`).

Approaches: static slots + `param1`; one ES handler reading window state;
or named global + `param1` (no anonymous).

### H5 — Top menu (different system)

Not element `onclick_event`. Activation is JS:

```js
if (item.onclick) item.onclick(item.parameter)
```

- [ ] **H5** Separate design: item `onclick_event` + dispatch from
      `top_menu_activate_item`, or only remove factories
      (`top_menu_show_window_by_id("…")`). Do not pretend it is a `ui { }`
      field rename.

### H6 — Helper defaults + CI

Empire override works anytime without this.

- [ ] **H6a** Decide: C++ fallback for missing `close`/`help` ES, **or** keep
      JS defaults and only override per window.
- [ ] **H6b** Optional mass default change on `help_button` / `close_button`.
- [ ] **H6c** CI grep: fail `onclick:\s*function`, `\.onclick\s*=`,
      `onclick:\s*\w+\(` (allowlist H4 leftovers).

### H7 — Pull APIs + draw (separate project)

- [ ] **H7a** Ban anonymous `textfn` / `checkedfn`; named OK until
      `text_event` / `checked_event`.
- [ ] **H7b** Draw canon / `memory=frame` — separate PRs from click migration.
- [ ] **H7c** Optional: `onrclick_event` in C++ when first needed.
- [ ] **H7d** Optional: deprecate remaining named `onclick:` in C++ log.

---

## Acceptance

**After A–C (click migration MVP):**

- [ ] No `onclick: function` in `src/scripts`
- [ ] No easy `.onclick =` outside H4 allowlist
- [ ] No inline factory `onclick: show_window_by_id(` (and kin) in migrated files
- [ ] Smoke: main menu → city → advisors → empire → options → one orders window

**After H\* (full cleanup):**

- [ ] CI grep gate
- [ ] Dynamic + top menu + advisors strip + campaign
- [ ] No anonymous pull callbacks; draw path policy as decided

---

## Risks (keep in mind)

- `arrow` / `image_button`: empty ES payload until H1 — use unique event names.
- `onclick_event` overrides `onclick` — helper default on that element is dead.
- Naive CI `onclick:\s*function` misses factories — also gate `onclick:\s*\w+\(`.
- Top menu ≠ element tree. Dynamic lists ≠ static rename.
