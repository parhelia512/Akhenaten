# Plan: UI callbacks → ES-only

Migrate UI from runtime closures / `onclick:` function refs to declarative
`*_event` names plus `[es=(widget, name)]` handlers.

Reference implementation: `src/scripts/ui_empire_window.js` (empire map buttons +
`ondraw_event`).

## Goal

Canon for UI trees:

- only `onclick_event` / `ondraw_event` / `onhover_event` / `onrclick_event` (etc.);
- logic only in `[es=(widget, name)]` functions;
- no `.onclick = …` in `init`.

Out of scope for the first pass: banning global functions entirely, or rewriting
`textfn` / `checkedfn` (pull APIs that return values).

## What counts as not ES-only

| Pattern | Example | Priority |
|--------|---------|----------|
| Assign in `init` | `window.btn.onclick = function(){…}` | high |
| Anonymous in config | `onclick: function(){…}` | high |
| Named ref | `onclick: window_go_back` | medium (style + one call path) |
| Helper defaults | `help_button` → `onclick: window_show_help` | medium (one change, many windows) |
| Runtime dynamics | `btn.onclick = …` in loops | high, separate design |
| `textfn` / `checkedfn` | pull API with return | low / phase 2 (needs `text_event` / `checked_event` in C++) |

Rough inventory (at plan time): dozens of files with `onclick:…`, 15+ with
anonymous `function(){}`, a handful of `.onclick =` in init. ES-only already on
empire and parts of file dialogs / some windows.

---

## Phase 0 — Rules and infrastructure

1. Short canon in wiki / `src/js/CLAUDE.md`:
   - new UI: only `*_event`;
   - no `.onclick =` in init;
   - `onclick: named_fn` is legacy — do not add.
2. Per-window migration checklist:
   - replace fields → add `[es=(window, …)]` → remove init assignments → click/draw smoke.
3. Optional: debug assert/log if an element has both `onclick` and `onclick_event`
   (event already wins), or if a draw path invokes a closure ref.
4. Keep C++ `onclick` dual path for legacy until later phases.

**Done when:** rule is written; empire remains the reference.

---

## Phase 1 — Shared helpers (small diff, wide reach)

Files: `ui_common.js` (`help_button`, `close_button`), maybe `next_button` / advisor.

Problem: ES handlers are bound to a concrete widget id (`empire_window`, not `*`).
Options:

- Per-window `[es=(that_window, close)]` → `window_go_back` (verbose without codegen);
- C++ fallback: `onclick_event` with no handler → call a conventional global;
- Defer helper conversion until enough windows already use `close` / `help` events.

**Recommendation:** do not break all close/help in phase 1. Use phase 1 for the
rule + checklist; convert helpers in phase 5 (or when a C++ fallback exists).

**Done when:** new windows do not copy `onclick: function`.

---

## Phase 2 — Hot path and init assignments (highest value first)

In priority order:

1. Already mostly done: empire (`ondraw_event` + click ES).
2. `.onclick =` in init (few files — quick wins):
   - `ui_mission_briefing_window.js`
   - `ui_hold_festival_window.js`
   - `ui_mission_end_window.js` (`onclick = named` → event)
3. Sidebar / speed / tax (frequent clicks, anonymous emit):
   - `ui_sidebar_window.js` (speed arrows)
   - `ui_speed_options_window.js`
   - `ui_tax_collector_window.js`, `ui_palace_window.js`
4. Options with many ±1 handlers:
   - `ui_sound_options_window.js`
   - `ui_donate_to_city_window.js`
   - `ui_popup_messages_window.js` (`cat_0…11` → one `toggle_cat` + `param1` / payload)

Migration pattern:

```js
// before
arrowup({ onclick: function() { emit event_change_gamespeed{ increase: true } } })

// after
arrowup({ onclick_event: "inc_gamespeed" })

[es=(speed_options_window, inc_gamespeed)]
function speed_options_inc_gamespeed(window) {
    emit event_change_gamespeed{ increase: true }
}
```

For indexed rows, use existing button `param1` / `param2` if
`dispatch_autoconfig_es_event` forwards them; verify on the first window, extend
C++ if needed.

**Done when:** no `.onclick =` in init; no anonymous `onclick: function` in
sidebar / speed / options listed above.

---

## Phase 3 — Static windows (waves)

Waves of 3–8 files per PR:

| Wave | Area | Examples |
|------|------|----------|
| 3a | Menus | `ui_main_menu`, dynasty, player_selection, victory |
| 3b | Advisors | imperial / trade / finance / … |
| 3c | Building info / orders | granary, storage, bazaar, dock, workshop |
| 3d | Top menu / editor | `ui_top_menu_*` |
| 3e | File dialogs | already partly ES — finish remaining `onclick:` |

Per PR: scripts only (+ tiny C++ if params missing). Smoke: open window, click all
buttons, hot-reload the file.

Named `onclick: foo`: switch to `onclick_event` + thin `[es=…]` wrapper, or leave
until phase 5 if `foo` is already a stable global.

---

## Phase 4 — Dynamic callbacks (separate design)

Hard cases:

- `ui_mission_choice_window.js` — buttons in a loop, `point_btn.onclick = function(){…}`
- `ui_window_features.js` — `option.onclick = f.toggle`, many `checkedfn`
- Any list where the handler depends on a runtime id

Pick one approach per feature:

1. Static slots + `param1 = id` (like popup category buttons).
2. One ES handler that reads `window.selected_*` / event map payload.
3. Keep named global `onclick: toggle_feature` with id in `param1` — no anonymous
   closures, but not pure event-name-on-widget yet.

Do not force a unique ES name per dynamic button without param support in dispatch.

**Done when:** no anonymous closures in dynamic UI; `onclick: global_fn` + param
allowed until phase 5.

---

## Phase 5 — Helpers + wipe remaining `onclick:` (optional)

1. `help_button` / `close_button` → `onclick_event` only.
2. Mass-add `[es=(W, close/help)]` or C++ fallback to `window_go_back` /
   `window_show_help`.
3. CI grep gate in `src/scripts`: fail on `onclick:\s*function` and `\.onclick\s*=`
   (allowlist for unfinished dynamic windows).
4. Gradually convert `onclick: named`; when grep is clean, optionally log deprecated
   `onclick` in C++.

---

## Phase 6 — `textfn` / `checkedfn` (separate project)

These are pull APIs: C++ expects a return value. ES events do not support that
without:

- `text_event` / `checked_event` with a return channel, or
- allowing **named** `textfn: my_label_fn` (global, not anonymous) as an exception.

Plan: ban only `textfn: function(){…}` / `checkedfn: function(){…}`; allow named
globals until `*_event` exists.

---

## Recommended order

```text
0   rules
2   init assignments + sidebar/speed/tax/sound/donate/popup   ← most value
3a–3e  window waves
4   dynamic (mission_choice, features)
1/5 helpers close/help + CI grep
6   textfn/checkedfn (optional)
```

Phase 1 helpers intentionally after waves 2–3, to avoid dozens of identical
`close` handlers before a fallback exists.

---

## Acceptance criteria

- [ ] No `onclick: function` / `ondraw: function` in scripts
- [ ] No `.onclick =` in init (except temporary allowlist)
- [ ] Draw path only via `ondraw_event` (+ `memory=frame` where needed)
- [ ] CI grep for anonymous callbacks
- [ ] Smoke: main menu → city → advisors → empire → options → one building orders window

---

## Risks

- Duplicated `close` / `help` handlers per window without a C++ fallback.
- ES param plumbing: confirm `dispatch_autoconfig_es_event` passes `param1`;
  otherwise popup cats / features block.
- `onclick_event` overrides `onclick`: do not leave a dead helper default after
  migration.
- Do not mix “ES-only” and “all draw on `memory=frame`” in the same PR.

---

## Suggested first PR

Same size as empire click migration:

1. Remove all `.onclick =` (briefing, festival, mission_end).
2. Sidebar + speed arrows → `onclick_event`.
3. Write the short rule into `src/js/CLAUDE.md` or UI wiki.
