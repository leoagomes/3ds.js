#pragma once

#include <string>

#include "duktape.h"

namespace js::utils {

bool read_file(duk_context* context, const std::string& path);

};
