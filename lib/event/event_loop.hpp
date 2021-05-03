#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <functional>

#include <stdint.h>

namespace event {

struct task {};

class task_queue {
private:
    std::queue<task> tasks;

public:
    task_queue();
};

enum timer_op { discard, rearm };

typedef std::function<timer_op(void)> timer_callback;

struct timer {
    uint64_t timeout, interval;
    timer_callback callback;

    timer(uint64_t interval, uint64_t timeout, timer_callback callback)
        : timeout(timeout), interval(interval), callback(callback) {}

    inline void rearm() {
        timeout += interval;
    }
};

bool operator>(const timer& lhs, const timer& rhs);

class timer_queue {
private:
    std::priority_queue<timer, std::vector<timer>, std::greater<timer>> queue;

    void process_timer(uint64_t current_tick, timer& t);

public:
    timer_queue();

    unsigned int process(uint64_t current_tick);

    void add(uint64_t current_tick, uint64_t interval, timer_callback callback);
};

class os_interface {
public:
    virtual uint64_t get_monotonic_tick() = 0;
    virtual uint64_t millis_to_ticks(uint64_t millis) { return millis; }
    virtual uint64_t ticks_to_millis(uint64_t ticks) { return ticks; }
};

class loop {
private:
    std::shared_ptr<os_interface> os;
    timer_queue timers;

public:
    loop(std::shared_ptr<os_interface> os);

    void process();

    void add_timer(uint64_t interval_millis, timer_callback callback);
};


};
