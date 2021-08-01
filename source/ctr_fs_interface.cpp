#include "ctr_fs_interface.hpp"

#include <3ds.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

namespace ctr {

bool fs_interface::exists(const std::filesystem::path& path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

bool fs_interface::is_file(const std::filesystem::path& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISREG(buffer.st_mode);
    }
    return false;
}

bool fs_interface::is_directory(const std::filesystem::path& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    }
    return false;
}


};
