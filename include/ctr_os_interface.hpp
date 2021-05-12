#pragma once

#include "events/loop.hpp"

namespace ctr {

class os_interface : public events::os_interface {
public:
    uint64_t get_monotonic_tick();
    uint64_t millis_to_ticks(uint64_t millis);
    uint64_t ticks_to_millis(uint64_t ticks);
};

};
