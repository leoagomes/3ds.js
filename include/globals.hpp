#pragma once

#include <memory>

#include "duktape.h"
#include "event_loop.hpp"

extern std::unique_ptr<event::loop> event_loop;
