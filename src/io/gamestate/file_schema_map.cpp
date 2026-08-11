#include "file_schemas.h"

#include "chunks.h"
#include "grid/image.h"
#include "io/chunk_serializer.h"

void file_schema_map(const int file_version) {
    g_chunk_io.push_chunk(4, false, "scenario_mission_index", iob_scenario_mission_id);
    g_chunk_io.push_chunk(4, false, "file_version", iob_file_version);
    g_chunk_io.push_chunk(6004, false, "chunks_schema", iob_chunks_schema);

    g_chunk_io.push_chunk(207936, false, "image_grid", &io_image_grid::instance());
    g_chunk_io.push_chunk(51984, false, "edge_grid", iob_edge_grid);
    g_chunk_io.push_chunk(207936, false, "terrain_grid", iob_terrain_grid);
    g_chunk_io.push_chunk(51984, false, "bitfields_grid", iob_bitfields_grid);
    g_chunk_io.push_chunk(51984, false, "random_grid", iob_random_grid);
    g_chunk_io.push_chunk(51984, false, "elevation_grid", iob_elevation_grid);

    g_chunk_io.push_chunk(8, false, "random_iv", iob_random_iv);
    g_chunk_io.push_chunk(8, false, "city_view_camera", iob_city_view_camera);
    g_chunk_io.push_chunk(1592, false, "scenario_info", iob_scenario_info);

    g_chunk_io.push_chunk(51984, false, "soil_fertility_grid", iob_soil_fertility_grid);
    g_chunk_io.push_chunk(18600, false, "scenario_events", iob_scenario_events);
    g_chunk_io.push_chunk(28, false, "scenario_events_extra", iob_scenario_events_extra);
    g_chunk_io.push_chunk(1280, true, "junk11", iob_junk11);
    g_chunk_io.push_chunk(file_version < 160 ? 15200 : 19600, true, "empire_map_objects", iob_empire_map_objects);
    g_chunk_io.push_chunk(16200, true, "empire_map_routes", iob_empire_map_routes);
    g_chunk_io.push_chunk(51984, false, "vegetation_growth", iob_vegetation_growth); // not sure what's the point of this in MAP...

    g_chunk_io.push_chunk(file_version < 147 ? 32 : 36, true, "floodplain_settings", iob_floodplain_settings);
    g_chunk_io.push_chunk(288, false, "trade_prices", iob_trade_prices);
    g_chunk_io.push_chunk(51984, true, "moisture_grid", iob_moisture_grid);
}
