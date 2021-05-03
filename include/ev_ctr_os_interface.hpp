#pragma once

#include "event_loop.hpp"

namespace ev {

class ctr_os_interface : public event::os_interface {
public:
    uint64_t get_monotonic_tick();
    uint64_t millis_to_ticks(uint64_t millis);
    uint64_t ticks_to_millis(uint64_t ticks);
};

};
