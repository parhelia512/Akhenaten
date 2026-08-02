#pragma once

#include <cstdint>

// Enhanced invasion bribe (Emperor-style). Flag gameplay_enhanced_invasion_bribe.
// Post-spawn only: pay deben → despawn wave by invasion_sequence.

bool invasion_bribe_feature_on();
bool invasion_bribe_allowed(uint16_t seq);
int invasion_bribe_cost(uint16_t seq); // 0 if none / not allowed composition

// Pay + despawn. seq 0 = auto-resolve queue head (if any).
// Returns 1 on success, 0 on refuse (flag/treasury/policy/empty).
int invasion_bribe_try(uint16_t seq);
