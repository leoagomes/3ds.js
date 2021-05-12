#include "ev.hpp"

#include "ctr_os_interface.hpp"

namespace ev {

std::shared_ptr<events::loop> loop = nullptr;

void init() {
    auto os_interface = std::make_shared<ctr::os_interface>();
    loop = std::make_shared<events::loop>(os_interface);
}

void terminate() {}

};
