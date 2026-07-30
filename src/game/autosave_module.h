#pragma once

#include "core/bstring.h"
#include "core/core.h"

#include <cstdint>

struct autosave_module_t {
    static constexpr int k_max_slots = 10;

    static int clamp_slots(int slots);
    static bool is_monthly_filename(pcstr filename_short);
    /** Ironwill whitelist: ironwill.* checkpoint + autosave_replay.* system write. */
    static bool is_ironwill_exempt_save(pcstr filename_short);
    static bstring256 format_monthly_filename(int slots, int slot_1based, pcstr extension);
    static int pick_slot(int slots, const bool *exists, const uint64_t *mtime);
    static bstring256 next_monthly_filename(pcstr extension);

    void on_advance_month();
    void register_callbacks();

private:
    static pcstr basename_of(pcstr path);
    static bool starts_with_ci(pcstr s, pcstr prefix);
    static uint64_t file_mtime_or_max(pcstr full_path);
};
