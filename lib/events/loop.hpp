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

    const std::shared_ptr<os_interface>& get_os_interface() { return os; }

    void process(
        uint64_t max_processing_millis = 10000,
        unsigned int task_batch_size = 100
    ) {
        auto current_tick = os->get_monotonic_tick();
        timers.process(current_tick);

        auto max_processing_ticks = os->millis_to_ticks(max_processing_millis);
        uint64_t max_tick = current_tick + max_processing_ticks;
        while (os->get_monotonic_tick() < max_tick && !tasks.empty()) {
            tasks.process(task_batch_size);
        }
    }

    std::shared_ptr<timer> add_timer(
        uint64_t interval_millis,
        timer_callback callback = nullptr
    ) {
        auto interval_ticks = os->millis_to_ticks(interval_millis);
        auto t = std::make_shared<timer>(interval_ticks, callback);
        enroll_timer(t);
        return t;
    }

    void enroll_timer(std::shared_ptr<timer>& timer) {
        auto current_tick = os->get_monotonic_tick();
        timer->arm(current_tick);
        timers.add(timer);
    }

    bool remove_timer(std::shared_ptr<timer>& timer) {
        return timers.remove(timer);
    }

    void add_task(task_fn task) {
        tasks.add_task(task);
    }
};

};
