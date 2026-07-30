#pragma once

enum e_transport_pick_mode {
    TRANSPORT_PICK_MOVE = 0,
    TRANSPORT_PICK_FORMATION = 1,
    TRANSPORT_PICK_LANDING = 2,
};

void window_city_transport_show(int transport_figure_id, e_transport_pick_mode mode);
e_transport_pick_mode window_city_transport_pick_mode();
