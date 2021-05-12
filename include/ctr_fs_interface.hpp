#pragma once

#include "js_module.hpp"

namespace ctr {

class fs_interface : public js::module::fs_interface {
private:
public:
    bool exists(std::filesystem::path& path);
    bool is_file(std::filesystem::path& path);
    bool is_directory(std::filesystem::path& path);
};

};
