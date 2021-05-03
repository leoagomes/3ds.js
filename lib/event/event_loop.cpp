#include "event_loop.hpp"

#include <algorithm>
#include <vector>

namespace event {

/* loop */

loop::loop(std::shared_ptr<os_interface> os) : os(os) {}

void loop::process() {
    auto current_tick = os->get_monotonic_tick();
    timers.process(current_tick);
}

void loop::add_timer(uint64_t interval_millis, timer_callback callback) {
    auto current_tick = os->get_monotonic_tick();
    auto interval_ticks = os->millis_to_ticks(interval_millis);

    timers.add(current_tick, interval_ticks, callback);
}

/* timers */

timer_queue::timer_queue() : queue() {}

unsigned int timer_queue::process(uint64_t current_tick) {
    if (queue.empty()) return 0;

    unsigned int processed_count = 0;
    while (queue.top().timeout <= current_tick) {
        auto timer = queue.top();
        queue.pop();

        process_timer(current_tick, timer);
        processed_count++;
    }

    return processed_count;
}

void timer_queue::process_timer(uint64_t current_tick, timer& t) {
    auto result = t.callback();

    if (result == timer_op::rearm) {
        t.rearm();
        queue.push(t);
    };
}

void timer_queue::add(
    uint64_t current_tick,
    uint64_t interval,
    timer_callback callback
) {
    queue.emplace(interval, current_tick + interval, callback);
}

bool operator>(const timer& lhs, const timer& rhs) {
    return lhs.timeout > rhs.timeout;
}

};
