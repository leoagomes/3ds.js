#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace events {

enum timer_op { discard, rearm };
typedef std::function<timer_op(void)> timer_callback;

struct timer {
private:
    uint64_t timeout, interval_ticks;
    timer_callback callback;

public:
    timer(
        uint64_t interval_ticks,
        timer_callback callback = nullptr
    ) : interval_ticks(interval_ticks),
        callback(callback) {}

    inline timer_op operator()() { return callback(); }

    void arm(uint64_t current_tick) {
        timeout = current_tick + interval_ticks;
    }

    inline void rearm() {
        timeout += interval_ticks;
    }

    void set_callback(timer_callback callback) {
        this->callback = callback;
    }

    uint64_t get_timeout() { return timeout; }
};

};
