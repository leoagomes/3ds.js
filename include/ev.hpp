#pragma once

#include <memory>

#include "event_loop.hpp"

namespace ev {

extern std::unique_ptr<event::loop> loop;

void init();
void terminate();

};
