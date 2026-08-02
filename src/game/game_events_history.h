#pragma once

#include <ctime>

#include "core/xstring.h"
#include "core/circullar_buffer.h"
#include "core/typename.h"
#include "core/bstring.h"

namespace events_history_adl {
    template<typename EventType>
    inline void describe_into(bstring256 &, const EventType &) {}
}

namespace events_history {
    struct event_history_record_s {
        time_t timestamp = 0;
        xstring name;
        xstring description;
        pcstr source_file = "";
        int source_line = 0;
    };

    // circular_buffer 2nd param is pow2 exponent: 1<<8 = 256 slots
    struct event_history_s {
        circular_buffer<event_history_record_s, 8> events;
    };

    const event_history_s &get_event_history();
    event_history_s &ref_event_history();

    void _append_record(const event_history_record_s &record);

    const bstring1024 &_event_to_string(const event_history_record_s &record);

    template<typename EventType>
    void log_event(pcstr source_file, int source_line, EventType const &ev) {
        const char *event_name = type_simplified_name(type_name<EventType>().data());
        bstring256 desc;
        using events_history_adl::describe_into;
        describe_into(desc, ev);
        const event_history_record_s record = {
            time(nullptr),
            event_name,
            desc.c_str(),
            source_file ? source_file : "",
            source_line
        };
        ref_event_history().events.push_tail(record);
    }
}
