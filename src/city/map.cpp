#include "map.h"

#include "city/data_private.h"
#include "grid/grid.h"

tile2i& city_map_entry_point() {
    return city_data.map.entry_point;
}

tile2i& city_map_exit_point() {
    return city_data.map.exit_point;
}

tile2i& city_map_entry_flag() {
    return city_data.map.entry_flag;
}

tile2i& city_map_exit_flag() {
    return city_data.map.exit_flag;
}

void city_map_set_entry_point(tile2i tile) {
    city_data.map.entry_point = tile;
}

void city_map_set_exit_point(tile2i tile) {
    city_data.map.exit_point = tile;
}

int city_map_set_entry_flag(tile2i tile) {
    city_data.map.entry_flag = tile;
    return city_data.map.entry_flag.grid_offset();
}

int city_map_set_exit_flag(tile2i tile) {
    city_data.map.exit_flag = tile;
    return city_data.map.exit_flag.grid_offset();
}

int city_map_road_network_index(int network_id) {
    for (int n = 0; n < 10; n++) {
        if (city_data.map.largest_road_networks[n].id == network_id)
            return n;
    }
    return 11;
}

void city_map_clear_largest_road_networks() {
    for (int i = 0; i < 10; i++) {
        city_data.map.largest_road_networks[i].id = 0;
        city_data.map.largest_road_networks[i].size = 0;
    }
}

void city_map_add_to_largest_road_networks(int network_id, int size) {
    for (int n = 0; n < 10; n++) {
        if (size > city_data.map.largest_road_networks[n].size) {
            // move everyone down
            for (int m = 9; m > n; m--) {
                city_data.map.largest_road_networks[m].id = city_data.map.largest_road_networks[m - 1].id;
                city_data.map.largest_road_networks[m].size = city_data.map.largest_road_networks[m - 1].size;
            }
            city_data.map.largest_road_networks[n].id = network_id;
            city_data.map.largest_road_networks[n].size = size;
            break;
        }
    }
}
