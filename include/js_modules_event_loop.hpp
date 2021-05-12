#pragma once

#include <memory>

#include "duktape.h"
#include "events/loop.hpp"

namespace js::modules::event_loop {

duk_ret_t open(duk_context* context);

};
