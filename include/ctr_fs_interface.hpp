#pragma once

#include "js_module.hpp"

namespace ctr {

class fs_interface : public js::module::fs_interface {
private:
public:
    bool exists(const std::filesystem::path& path);
    bool is_file(const std::filesystem::path& path);
    bool is_directory(const std::filesystem::path& path);
};

};
