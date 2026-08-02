#include "game_events_history.h"

#include <cstring>

events_history::event_history_s event_history;

const events_history::event_history_s &events_history::get_event_history() {
    return event_history;
}

events_history::event_history_s &events_history::ref_event_history() {
    return event_history;
}

void events_history::_append_record(const event_history_record_s &record) {
    event_history.events.push_tail(record);
}

namespace {
    pcstr file_basename(pcstr path) {
        if (!path || !*path) {
            return "";
        }
        pcstr base = path;
        for (pcstr p = path; *p; ++p) {
            if (*p == '/' || *p == '\\') {
                base = p + 1;
            }
        }
        return base;
    }
}

const bstring1024 &events_history::_event_to_string(const event_history_record_s &record) {
    const tm *loctime = localtime(&record.timestamp);
    bstring32 timestamp;
    if (loctime) {
        strftime(timestamp, bstring32::capacity, "%FT%TZ", loctime);
    } else {
        timestamp = "?";
    }

    static bstring1024 event_str;
    event_str.clear();
    event_str.append(timestamp);
    event_str.append_fmt(" %s", record.name.c_str());
    if (!record.description.empty()) {
        event_str.append_fmt(" %s", record.description.c_str());
    }
    if (record.source_file && *record.source_file) {
        event_str.append_fmt(" %s:%d", file_basename(record.source_file), record.source_line);
    }
    return event_str;
}
