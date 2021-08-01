#pragma once

#include <string>

#include "duktape.h"

namespace js {

extern duk_context* context;

void init();
void terminate();

int run_file(const std::string& name);

};
