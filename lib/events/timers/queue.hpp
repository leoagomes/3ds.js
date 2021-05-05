#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "events/timers/timer.hpp"

namespace events {

inline bool timer_pointer_greater(std::shared_ptr<timer>& lhs, std::shared_ptr<timer>& rhs) {
    return lhs->timeout > rhs->timeout;
}

class timer_queue {
private:
    std::vector<std::shared_ptr<timer>> timers;

    void push_timer(std::shared_ptr<timer>& t) {
        timers.push_back(t);
        std::push_heap(
            timers.begin(),
            timers.end(),
            timer_pointer_greater
        );
    }

    std::shared_ptr<timer> pop_timer() {
        std::pop_heap(
            timers.begin(),
            timers.end(),
            timer_pointer_greater
        );
        auto timer = timers.back();
        timers.pop_back();
        return timer;
    }

    void remake_timer_heap() {
        std::make_heap(timers.begin(), timers.end(), timer_pointer_greater);
    }

public:
    timer_queue() : timers() {}

    unsigned int process(uint64_t current_tick) {
        if (timers.empty()) return 0;

        unsigned int processed_count = 0;
        while (timers.front()->timeout <= current_tick) {
            auto timer = pop_timer();

            if (timer->operator()() == timer_op::rearm) {
                timer->rearm();
                push_timer(timer);
            }
            processed_count++;
        }

        return processed_count;
    }

    std::shared_ptr<timer> add(
        uint64_t current_tick,
        uint64_t interval,
        timer_callback callback
    ) {
        auto t = std::make_shared<timer>(
            interval,
            current_tick + interval,
            callback
        );
        push_timer(t);
        return t;
    }

    bool remove(std::shared_ptr<timer>& timer) {
        /*
         * XXX: This removal is less efficient than it **can** be.
         *
         * If instead of simply re-making the heap, we fix it by swapping the
         * removed element with the last one and fix the heap up/down from the
         * erased element's position, it **should** be more efficient.
         *
         * I'll consider it outside the scope of the current implementation,
         * however, since this would make this portion of the code significantly
         * more complex (I'd have to implement heap operations) and I'm not yet
         * convinced that timers are removed at a frequency high enough to
         * justify this added code complexity.
         */
        for (auto it = timers.begin(); it != timers.end(); it++) {
            if (*it == timer) {
                timers.erase(it);
                remake_timer_heap();
                return true;
            }
        }
        return false;
    }
};

};
