#pragma once

#include "core/buffer.h"
#include "empire/empire_city.h"
#include "game/resource.h"

class building;

// New Era per-good trip cap when gameplay_change_trader_per_good_1600 is ON; 0 = disabled.
int empire_trader_per_good_cap();
// When true, total buy/sell bag (max_capacity) is not enforced.
bool empire_trader_ignore_total_bag();

struct empire_trader_handle {
    uint8_t handle = 0;

    int record_bought_resource(e_resource resource);
    int record_sold_resource(e_resource resource);

    int bought_resources(e_resource resource);
    int sold_resources(e_resource resource);

    bool buy_full(e_resource resource) const;
    bool sell_full(e_resource resource) const;
    // Units still allowed under per-good cap; large value when per-good mode is OFF.
    int buy_room(e_resource resource) const;
    int sell_room(e_resource resource) const;

    bool has_traded();
    bool has_traded_max(int capacity);

    bool valid() const { return handle != 0; }
    void back_to_city();

    e_resource get_buy_resource(building* storageyard, empire_city_handle city, int amount);
    e_resource get_sell_resource(building* warehouse, empire_city_handle city);
};

empire_trader_handle empire_create_trader();
