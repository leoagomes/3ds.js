#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "duktape.h"

namespace js::module {

class fs_interface {
public:
    virtual bool exists(const std::filesystem::path& path) = 0;
    virtual bool is_file(const std::filesystem::path& path) = 0;
    virtual bool is_directory(const std::filesystem::path& path) = 0;
};

extern std::shared_ptr<fs_interface> fs;

void init(duk_context* context);

};
