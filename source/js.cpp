#include "js.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "ctr_fs_interface.hpp"
#include "js_console.hpp"
#include "js_module.hpp"
#include "js_modules.hpp"

// globals
duk_context* js::context = nullptr;

// forward declarations
static void init_context();
static void terminate_context();

void js::init() {
    init_context();

    js::console::init(js::context, DUK_CONSOLE_FLUSH);
    js::modules::native_loaders(js::context);
    js::module::fs = std::make_shared<ctr::fs_interface>();
    js::module::init(js::context);
}

void js::terminate() {
    terminate_context();
}

duk_ret_t js::run_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();

    duk_push_string(js::context, filename.c_str());
    if (duk_pcompile_string_filename(js::context, 0, contents.c_str())) {
        return 1;
    }

    return duk_pcall(js::context, 0);
}

static void init_context() {
    js::context = duk_create_heap_default();
    if (!js::context)
        exit(1);
}

static void terminate_context() {
    duk_destroy_heap(js::context);
}
