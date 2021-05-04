#pragma once

#include <cstdint>

namespace events {

class os_interface {
public:
    virtual uint64_t get_monotonic_tick() = 0;
    virtual uint64_t millis_to_ticks(uint64_t millis) { return millis; }
    virtual uint64_t ticks_to_millis(uint64_t ticks) { return ticks; }
};

};
