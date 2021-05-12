#pragma once

#include <memory>

#include "events/loop.hpp"

namespace ev {

extern std::shared_ptr<events::loop> loop;

void init();
void terminate();

};
