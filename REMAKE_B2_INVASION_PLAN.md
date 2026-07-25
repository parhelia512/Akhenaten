# B2 — invasions: JS + bind tags (native cancelled)

**Статус:** **CANCELLED native** `EVENT_TYPE_INVASION` calendar/favour (2026-07-25).  
**Текущая модель:** JS calendar/favour spawn + bindable resolve tags + history v173.  
**Трекер:** `REMAKE_TASKS_P1.md` § B2 · `REMAKE_EMPIRE_MISSIONS_PLAN.md`.

## Решение

| Было (native B2 Phase 1–7) | Стало |
|----------------------------|--------|
| Calendar/favour в `EVENT_TYPE_INVASION` | **JS** calendar + `mission_pharaoh_favour_invasion_tick` |
| `on_*` на `event_ph_t` INVASION | **Bind** tags на `start_foreign_army_invasion` |
| Pending по `event_id` (v172) | Active binds по `invasion_id`/`seq` (v173); v172 stub |
| History нет | Ring **64** history (audit/UI only) |

```javascript
city.create_chain_event({ tag_id: 801, type: EVENT_TYPE_REPUTATION_INCREASE, amount: 2 })
city.start_foreign_army_invasion({
  invasion_id: 1,
  enemy: ENEMY_5_HYKSOS,
  size: 22,
  want_destroy_buildings: 22,
  on_completed_tag: 801,  // wipe
  on_refusal_tag: 802,    // destroy-goal (omit → ok-only, Behdet)
})
```

**Resolve (month):** `enemies_seen` → formations gone → destroy-goal+refuse_tag → refuse;
else → completed. Tag `0` = skip branch. Favour: `ATTACK_TYPE_ENEMIES` + `ENEMY_3_EGYPTIAN`
(+ `message_kingdome_army_attack`).

`fire_chain_by_tag` drains ONLY_VIA→ACTIVATED clones the same month (bind tick runs after
`process_events`).

**Keep from old B2:** `want_destroy` → `buildings_to_destroy`; REQUEST `chain_action_next = NONE`;
randomize div0 guard.

**Missions:** Behdet y15 +8 ok-only; Selima yearly ×9 ok-only + y7×22 ok/refuse→siege/troops;
Abu favour chain2 uses ENEMIES+Egyptian (JS poll for 40→20→20 stays).

**Integral:** `tests/50_invasion_bind_resolve.js`.

**Next:** B3 invasion warnings save; B4 phrase_id. Distant battle — not B2.

---

## Архив

Native Phase 1–7 (`EVENT_TYPE_INVASION` spawn/favour/gate/`use_native_invasion_events`) was
playtested worse than JS and removed. Do not reintroduce without a new decision. Older
sections of this file lived in git history around the native experiment.
