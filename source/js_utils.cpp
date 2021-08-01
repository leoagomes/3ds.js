#include "js_utils.hpp"

#include <fstream>
#include <sstream>

bool js::utils::read_file(duk_context* context, const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();
    duk_push_string(context, path.c_str());
    return 1;
}
