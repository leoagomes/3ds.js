#include "ctr_os_interface.hpp"

#include <3ds.h>

namespace ctr {

uint64_t os_interface::get_monotonic_tick() {
    return svcGetSystemTick();
}

uint64_t os_interface::millis_to_ticks(uint64_t millis) {
    return CPU_TICKS_PER_MSEC * millis;
}

uint64_t os_interface::ticks_to_millis(uint64_t ticks) {
    return ticks / CPU_TICKS_PER_MSEC;
}

};
