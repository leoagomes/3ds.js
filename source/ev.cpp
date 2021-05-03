#include "ev.hpp"

#include "ev_ctr_os_interface.hpp"

namespace ev {

std::unique_ptr<event::loop> loop = nullptr;

void init() {
    auto os_interface = std::make_shared<ctr_os_interface>();
    loop = std::make_unique<event::loop>(os_interface);
}

void terminate() {}

};
