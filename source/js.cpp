#include "js.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "js_console.hpp"

// globals
duk_context* js::context = nullptr;

// forward declarations
static void init_context();
static void init_console();
static void terminate_context();

void js::init() {
    init_context();
    init_console();
}

void js::terminate() {
    terminate_context();
}

int js::run_file(std::string& filename) {
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
    if (!js::context) { exit(1); }
}

static void init_console() {
    duk_console_init(js::context, DUK_CONSOLE_FLUSH);
}

static void terminate_context() {
    duk_destroy_heap(js::context);
}
