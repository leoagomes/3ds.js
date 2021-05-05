#pragma once

#include <cstdint>
#include <memory>

#include "events/os_interface.hpp"
#include "events/timers/queue.hpp"

namespace events {

class loop {
private:
    std::shared_ptr<os_interface> os;
    timer_queue timers;

public:
    loop(std::shared_ptr<os_interface> os) : os(os) {}

    void process() {
        auto current_tick = os->get_monotonic_tick();
        timers.process(current_tick);
    }

    std::shared_ptr<timer> add_timer(
        uint64_t interval_millis,
        timer_callback callback
    ) {
        auto current_tick = os->get_monotonic_tick();
        auto interval_ticks = os->millis_to_ticks(interval_millis);

        return timers.add(current_tick, interval_ticks, callback);
    }

    bool remove_timer(std::shared_ptr<timer>& timer) {
        return timers.remove(timer);
    }
};

};
