#pragma once

#include "grid/point.h"

class building;

// Flat buildings view helpers (Enhanced QoL). Session flag: g_screen_city.buildings_flat_view.

bool city_flat_feature_enabled();
bool city_flat_view_active();
void city_flat_view_set(bool on);
void city_flat_view_toggle();
void city_flat_view_reset(); // Off + clear raised
void city_flat_view_sync_feature();
// Main-thread, before city draw (incl. parallel workers): sync feature + snapshot raised set.
void city_flat_prepare_draw();

bool city_flat_is_raised(int main_building_id);
void city_flat_toggle_raised(int main_building_id);
void city_flat_clear_raised();
void city_flat_erase_raised(int main_building_id); // building delete / destroy

int city_flat_flatten_id(const building &b);
int city_flat_building_texture_id(const building &b);

bool city_flat_should_flatten_building(const building &b);
bool city_flat_should_skip_tall_ornaments(const building &b);
