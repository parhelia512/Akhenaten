#include <game/time.h>
#include <map/tiles.h>
#include <map/building.h>
#include <map/terrain.h>
#include "floods.h"

#include "city/constants.h"
#include "city/data_private.h"
#include "scenario/data.h"
#include "buildings.h"
#include "message.h"

static floods_data data;

floods_data* give_me_da_floods_data() {
    return &data;
}

static int cycle = 0;
static int cycle_tick = 0;

const double randomizing_double = 391.68; //0x40787ae147ae147b; // hardcoded
static int randomizing_int_1 = 0;
static int randomizing_int_2 = 0;

void floodplains_init() {
    data.floodplain_width = map_floodplain_rebuild_shoreorder();
}

int floodplains_current_cycle_tick() {
    return cycle_tick;
}
int floodplains_current_cycle() {
    return cycle;
}
int floodplains_flooding_start_cycle() {
//    int h = (int)(long long)((float)give_me_da_time()->year - (short)scenario_data.start_year) * (float)randomizing_double;
    int h = 0;
    return data.season * 1.05 + 15 + h;
}
int floodplains_flooding_end_cycle() {
    return floodplains_flooding_start_cycle() + data.duration + data.floodplain_width * 2;
}
int floodplains_flooding_rest_period_cycle() {
    return (float)data.quality_last * (float)data.floodplain_width * 0.01;
}

bool floodplains_is(int state) {
    return data.state == state;
}

int floodplains_expected_quality() {
    return data.quality_next;
}
int floodplains_expected_month() {
    return (data.season_initial / 15) - 10;
}

void floodplains_tick_update() {
    int total_ticks = game_time_absolute_tick();
    cycle = total_ticks / 25;
    cycle_tick = total_ticks % 25;
    // TODO: the cycles don't match up PERFECTLY with the original game... but close enough?

    // ???
//    if (data.flood_progress == 0) {
//        data.state = FLOOD_STATE_FARMABLE;
//        data.quality = 0;
//        return;
//    }

    // clamp and update flood quality
    if (game_time_tick() == 1 && data.state != FLOOD_STATE_FLOODING) {
        if (data.quality > data.quality_next) {
            data.quality -= 5;
            if (data.quality < data.quality_next) // clamp if over-shooting
                data.quality = data.quality_next;
        }
        if (data.quality < data.quality_next) {
            data.quality += 5;
            if (data.quality > data.quality_next) // clamp if over-shooting
                data.quality = data.quality_next;
        }
    }

    // fetch cycle & time variables
    int cycle_flooding_start = floodplains_flooding_start_cycle();
    int cycle_flooding_end = floodplains_flooding_end_cycle();
    int rest_period = floodplains_flooding_rest_period_cycle();
    // ????
//    if ((double)(cycle_flooding_start - cycle) > randomizing_double * 0.5) {
//        cycle_flooding_start -= (int)randomizing_double;
//        cycle_flooding_end += (int)randomizing_double;
//    }

    // ???
    data.unk01 = data.season / 30;

    if (cycle < cycle_flooding_start - 28) {
        // normal farming period
        data.state = FLOOD_STATE_FARMABLE;
    } else if (cycle < cycle_flooding_start) {
        // flooding imminent!
        // tell all farms to DROP EVERYTHING and deliver food
        data.state = FLOOD_STATE_IMMINENT;
    } else if (cycle < cycle_flooding_start + rest_period) {
        // flooding in progress
        data.state = FLOOD_STATE_FLOODING;
        data.flood_progress = 29 - (cycle - cycle_flooding_start);
    } else if (cycle < cycle_flooding_end - rest_period) {
        // fully flooded
        data.state = FLOOD_STATE_INUNDATED;
        data.flood_progress = 29 - rest_period;
    } else if (cycle < cycle_flooding_end) {
        // contracting
        data.state = FLOOD_STATE_CONTRACTING;
        data.flood_progress = 30 - (cycle_flooding_end - cycle);
    } else if (cycle < cycle_flooding_end + 28) {
        // contracting done, resting
        data.state = FLOOD_STATE_RESTING;
        data.flood_progress = 30;
        if (cycle == cycle_flooding_end + 18 && game_time_tick() == 0) {
            // send nilometer message!
            if (data.quality_next == 100)
                city_message_post(true, MESSAGE_FLOOD_PERFECT, 0, 0);
            else if (data.quality_next >= 75)
                city_message_post(true, MESSAGE_FLOOD_EXCELLENT, 0, 0);
            else if (data.quality_next >= 50)
                city_message_post(true, MESSAGE_FLOOD_GOOD, 0, 0);
            else if (data.quality_next >= 25)
                city_message_post(true, MESSAGE_FLOOD_MEDIOCRE, 0, 0);
            else if (data.quality_next > 0)
                city_message_post(true, MESSAGE_FLOOD_POOR, 0, 0);
            else
                city_message_post(true, MESSAGE_FLOOD_FAIL, 0, 0);
        }
    } else {
        // flooding over, farmlands available again
        data.state = FLOOD_STATE_FARMABLE;
    }

    // clamp flood progress
    if (data.flood_progress < 0)
        data.flood_progress = 0;
    else if (data.flood_progress > 30)
        data.flood_progress = 30;

    // update at every full cycle
    if (cycle_tick == 0) {
        if (cycle == cycle_flooding_start - 49) {
            // todo: FUN_00489310();
        }
        else if (cycle == cycle_flooding_start - 23) {
            // harvest!
        }
        else if (cycle == cycle_flooding_start - 1) {
            // update values
            data.season = data.season_initial;      // reset to initial
            data.duration = data.duration_initial;  // reset to initial

            data.quality_last = data.quality;
            if (data.quality_last > 100)
                data.quality_last = 100; // clamp!

            data.quality_next = 40; // todo: quality?

            // todo: FUN_004bd0b0(data.season_initial);
        }
        else if (cycle == cycle_flooding_start + data.floodplain_width) {
            int a = 2;
            // todo ?????
        }
        else if (cycle == cycle_flooding_end + 1) {
            int a = 2;
            // todo: FUN_004be2b0(city_data_ptr)
        }
    }

    // update tiles!!
    if (cycle >= cycle_flooding_start && cycle <= cycle_flooding_start + rest_period)
        map_update_floodplain_inundation(1, (29 - data.flood_progress) * 25 + cycle_tick);
    else if (cycle >= cycle_flooding_end - rest_period && cycle <= cycle_flooding_end)
        map_update_floodplain_inundation(-1, (30 - data.flood_progress) * 25 - cycle_tick);

    // update grass growth
    if (cycle_tick % 10 == 0 && (cycle < cycle_flooding_start - 27 || cycle >= cycle_flooding_end - rest_period))
        map_advance_floodplain_growth();
}

io_buffer *iob_floodplain_settings = new io_buffer([](io_buffer *iob) {
    iob->bind(BIND_SIGNATURE_INT32, &data.season_initial);
    iob->bind(BIND_SIGNATURE_INT32, &data.duration_initial);
    iob->bind(BIND_SIGNATURE_INT32, &data.quality_initial);
    iob->bind(BIND_SIGNATURE_INT32, &data.season);
    iob->bind(BIND_SIGNATURE_INT32, &data.duration);
    iob->bind(BIND_SIGNATURE_INT32, &data.quality);
    iob->bind(BIND_SIGNATURE_INT32, &data.unk00);
    iob->bind(BIND_SIGNATURE_INT32, &data.quality_next);
    iob->bind(BIND_SIGNATURE_INT32, &data.quality_last);

    data.flood_progress = 30;
    data.unk00 = 0;
    data.state = FLOOD_STATE_FARMABLE;
    data.floodplain_width = 10;
});