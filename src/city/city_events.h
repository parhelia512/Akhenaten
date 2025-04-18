#pragma once 

#include "core/eventbus.h"

extern events::typed_queue g_city_events;

namespace events {
    template<typename T>
    inline void emit(T &&event) {
        g_city_events.enqueue(std::forward<T>(event));
    }

    template<typename T>
    inline void subscribe(T subscriber) {
        g_city_events.subscribe(subscriber);
    }

    template<typename T>
    inline void unsubscribe(T subscriber) {
        g_city_events.unsubscribe(subscriber);
    }
} // namespace events