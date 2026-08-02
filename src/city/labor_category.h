#pragma once

#include "core/tokenum.h"

enum e_labor_category : int8_t {
    LABOR_CATEGORY_INVALID = -1,
    LABOR_CATEGORY_NONE = -1,
    LABOR_CATEGORY_FOOD_PRODUCTION = 0,
    LABOR_CATEGORY_INDUSTRY_COMMERCE = 1, // "Industry" when labor-category split is ON
    LABOR_CATEGORY_ENTERTAINMENT = 2,
    LABOR_CATEGORY_RELIGION = 3,
    LABOR_CATEGORY_EDUCATION = 4,
    LABOR_CATEGORY_WATER_HEALTH = 5,
    LABOR_CATEGORY_INFRASTRUCTURE = 6,
    LABOR_CATEGORY_GOVERNMENT = 7,
    LABOR_CATEGORY_MILITARY = 8,
    LABOR_CATEGORY_CULTURE = 9,
    LABOR_CATEGORY_HOUSE = 10,

    // Enhanced: Storage & distribution (SY + dock). Append-only — do not renumber 0..10.
    LABOR_CATEGORY_STORAGE = 11,

    LABOR_CATEGORY_SIZE,
};

// OG city_data binds categories[0..9] only (FOOD..CULTURE). HOUSE/STORAGE live outside that layout.
constexpr int LABOR_CATEGORY_SAVE_SLOTS = 10;

// User priority ranks 1..N for cats 0..MILITARY. Split ON adds one rank for STORAGE.
constexpr int LABOR_CATEGORY_PRIORITY_RANK_OG = 9;

using e_labor_category_tokens_t = token_holder<e_labor_category, LABOR_CATEGORY_FOOD_PRODUCTION, LABOR_CATEGORY_SIZE>;
