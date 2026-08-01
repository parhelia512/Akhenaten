#include "city/campaign_carry.h"

#include "core/profiler.h"
#include "js/js_game.h"

#include <algorithm>

void __campaign_carry_clear() {
    g_campaign_carry.clear();
}
ANK_FUNCTION(__campaign_carry_clear);

void __campaign_carry_snapshot_troops() {
    g_campaign_carry.snapshot_from_formations();
}
ANK_FUNCTION(__campaign_carry_snapshot_troops);

void __campaign_carry_snapshot_monuments() {
    g_campaign_carry.snapshot_monuments_from_city();
}
ANK_FUNCTION(__campaign_carry_snapshot_monuments);

void __campaign_carry_activate(int mask) {
    g_campaign_carry.activate_for_mission((uint8_t)mask);
}
ANK_FUNCTION_1(__campaign_carry_activate);

void __campaign_carry_apply_monuments() {
    g_campaign_carry.apply_monuments();
}
ANK_FUNCTION(__campaign_carry_apply_monuments);

int __campaign_carry_pending_mask() {
    return g_campaign_carry.pending_mask();
}
ANK_FUNCTION(__campaign_carry_pending_mask);

int __campaign_carry_slot_figures(int slot) {
    if (slot < 0 || slot >= TROOP_CARRY_MAX) {
        return 0;
    }
    const auto &s = g_campaign_carry.troops[slot];
    return (s.pending || s.has_data) ? s.num_figures : 0;
}
ANK_FUNCTION_1(__campaign_carry_slot_figures);

int __campaign_carry_slot_pending(int slot) {
    if (slot < 0 || slot >= TROOP_CARRY_MAX) {
        return 0;
    }
    return g_campaign_carry.troops[slot].pending ? 1 : 0;
}
ANK_FUNCTION_1(__campaign_carry_slot_pending);

void __campaign_carry_set_slot(int slot, int figures, int training, int morale) {
    if (slot < 0 || slot >= TROOP_CARRY_MAX) {
        return;
    }
    auto &s = g_campaign_carry.troops[slot];
    s.has_data = figures > 0 ? 1 : 0;
    s.pending = 0;
    s.num_figures = (uint8_t)std::clamp(figures, 0, 16);
    s.has_military_training = training ? 1 : 0;
    s.morale = (int16_t)std::clamp(morale, 0, 100);
    s.experience = 0;
}
ANK_FUNCTION_4(__campaign_carry_set_slot);

void __campaign_carry_set_monument(int index, int type, int tile_x, int tile_y, int orientation, int variant) {
    if (index < 0 || index >= CAMPAIGN_CARRY_MONUMENT_MAX) {
        return;
    }
    auto &s = g_campaign_carry.monuments[index];
    s.has_data = type > 0 ? 1 : 0;
    s.type = (uint16_t)type;
    s.tile_x = (int16_t)tile_x;
    s.tile_y = (int16_t)tile_y;
    s.orientation = (uint8_t)orientation;
    s.variant = (uint8_t)variant;
    s.pad = 0;
}
ANK_FUNCTION_6(__campaign_carry_set_monument);

int __campaign_carry_monument_type(int index) {
    if (index < 0 || index >= CAMPAIGN_CARRY_MONUMENT_MAX) {
        return 0;
    }
    const auto &s = g_campaign_carry.monuments[index];
    return s.has_data ? (int)s.type : 0;
}
ANK_FUNCTION_1(__campaign_carry_monument_type);

void __campaign_carry_post_notice() {
    g_campaign_carry.post_notice_if_needed();
}
ANK_FUNCTION(__campaign_carry_post_notice);

int __campaign_carry_try_apply_fort(int building_type, int formation_id) {
    return g_campaign_carry.try_apply_to_fort((e_building_type)building_type, formation_id) ? 1 : 0;
}
ANK_FUNCTION_2(__campaign_carry_try_apply_fort);

void __campaign_carry_expire_troops() {
    g_campaign_carry.expire_pending_troops();
}
ANK_FUNCTION(__campaign_carry_expire_troops);
