#include "js_modules.hpp"

#include "duktape.h"

#include "js_modules_event_loop.hpp"

#define native_module(name, open) { .key = name, .value = open, .nargs = 0 }

namespace js::modules {

void native_loaders(duk_context* context) {
    duk_function_list_entry list[] = {
        native_module("event_loop", js::modules::event_loop::open),
        {0},
    };

    duk_push_object(context);
    duk_put_function_list(context, -1, list);
}

};
