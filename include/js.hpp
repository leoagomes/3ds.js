#pragma once

#include <string>

#include "duktape.h"

namespace js {

extern duk_context* context;

void init();
void terminate();

duk_ret_t run_file(const std::string& name);

};
