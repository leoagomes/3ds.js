#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace events {

enum timer_op { discard, rearm };
typedef std::function<timer_op(void)> timer_callback;

struct timer {
    uint64_t timeout, interval;
    timer_callback callback;

    timer(
        uint64_t interval,
        uint64_t timeout,
        timer_callback callback
    ) : timeout(timeout),
        interval(interval),
        callback(callback) {}

    inline timer_op operator()() { return callback(); }

    inline void rearm() {
        timeout += interval;
    }
};

};
