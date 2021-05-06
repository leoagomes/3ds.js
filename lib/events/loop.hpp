#pragma once

#include <cstdint>
#include <memory>

#include "events/os_interface.hpp"
#include "events/tasks/queue.hpp"
#include "events/timers/queue.hpp"

namespace events {

class loop {
private:
    std::shared_ptr<os_interface> os;
    timer_queue timers;
    task_queue tasks;

public:
    loop(std::shared_ptr<os_interface> os) : os(os), timers(), tasks() {}

    void process(
        uint64_t max_processing_millis = 10000,
        unsigned int task_batch_size = 100
    ) {
        auto current_tick = os->get_monotonic_tick();
        timers.process(current_tick);

        auto max_processing_ticks = os->millis_to_ticks(max_processing_millis);
        uint64_t max_tick = current_tick + max_processing_ticks;
        while (os->get_monotonic_tick() < max_tick) {
            tasks.process(task_batch_size);
        }
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

    void add_task(task_fn task) {
        tasks.add_task(task);
    }
};

};
