/*
 * A weird mix between a CommonJS module loader and Node.js semantics.
 * 
 * It does really poor package name resolution and prioritizes `js_modules`
 * instead of `node_modules`. Keep in mind you should try your best to keep
 * these applications simple at the moment.
 * 
 * Heavily based on:
 * - https://nodejs.org/api/modules.html
 * - https://github.com/svaarala/duktape/tree/master/extras/module-duktape
 * - https://wiki.duktape.org/howtomodules
 */

#include "duktape.h"
#include "js_module.hpp"
#include "js_utils.hpp"
#include "duk_module_duktape.h"

#include <string>
#include <cstdio>

#define CORE_MODULES_STASH_KEY "module/core-modules"

extern "C" {
duk_ret_t cb_module_search(duk_context* context);
};

std::shared_ptr<js::module::fs_interface> js::module::fs = nullptr;

void set_core_module_loader_stash(duk_context* context);
void set_module_search_function(duk_context* context);
void set_module_dirs(duk_context* context);
// expects a core module map to be on the top of the stack
void js::module::init(duk_context* context) {
    set_core_module_loader_stash(context);
    duk_module_duktape_init(context);
    set_module_search_function(context);
    set_module_dirs(context);
}

void set_core_module_loader_stash(duk_context* context) {
    duk_push_global_stash(context);
    duk_dup(context, -2);
    duk_put_prop_literal(context, -2, CORE_MODULES_STASH_KEY);
}

void set_module_search_function(duk_context* context){
    duk_get_global_literal(context, "Duktape");
    duk_push_c_function(context, cb_module_search, 4);
    duk_put_prop_string(context, -2, "modSearch");
    duk_pop(context);
}

void set_module_dirs(duk_context* context) {
    duk_get_global_literal(context, "Duktape");
    auto array_index = duk_push_array(context);
    int item_index = 0;
    for (const auto& dir : {
        "romfs:/js_modules/",
        "romfs:/node_modules/",
        "js_modules/",
        "node_modules/",
        "/js_modules/",
        "/node_modules/",
    }) {
        duk_push_string(context, dir);
        duk_put_prop_index(context, array_index, item_index++);
    }
    duk_put_prop_literal(context, -2, "modDirs");
    duk_pop(context);
}

enum loaded_file_type {
    type_none = 0,
    type_js   = 1,
    type_json = 2,
};

bool load_core_module(duk_context* context, const char* module_id);
loaded_file_type load_as_file(
    duk_context* context,
    const std::filesystem::path& module_path
);
loaded_file_type load_as_directory(
    duk_context* context,
    const std::filesystem::path& path
);
loaded_file_type load_module_dirs(
    duk_context* context,
    const std::filesystem::path& path
);
duk_ret_t cb_module_search(duk_context* context) {
    auto module_id = duk_require_string(context, 0);
    auto module_path = std::filesystem::path(module_id);

    module_path = std::string("romfs:/") + module_path.string();

    if (load_core_module(context, module_id)) {
        // module.exports should be on top of the stack
        duk_put_prop_literal(context, 3 /* module object*/, "exports");
        return 0; // no js code
    }

    switch (load_as_file(context, module_path)) {
    case loaded_file_type::type_js:   goto return_js;
    case loaded_file_type::type_json: goto return_json;
    default: break;
    }

    switch (load_as_directory(context, module_path)) {
    case loaded_file_type::type_js:   goto return_js;
    case loaded_file_type::type_json: goto return_json;
    default: break;
    }

    switch (load_module_dirs(context, module_path)) {
    case loaded_file_type::type_js:   goto return_js;
    case loaded_file_type::type_json: goto return_json;
    default: break;
    }

    return duk_error(context, DUK_ERR_ERROR, "module '%s' can not be resolved.", module_id);

    return_js:
    return 1;

    return_json:
    duk_put_prop_literal(context, 3 /* module object */, "exports");
    return 0;
}

bool load_core_module(duk_context* context, const char* module_id) {
    duk_push_global_stash(context);
    duk_get_prop_literal(context, -1, CORE_MODULES_STASH_KEY);
    duk_require_object(context, -1);
    if (duk_get_prop_string(context, -1, module_id)) {
        duk_call(context, 0);
        return true;
    }
    return false;
}

static bool try_load_js(
    duk_context* context,
    const std::filesystem::path& path
);
static bool try_load_json(
    duk_context* context,
    const std::filesystem::path& path
);
loaded_file_type load_as_file(
    duk_context* context,
    const std::filesystem::path& module_path
) {
    auto module_extension = module_path.extension();
    if (module_extension == ".js" &&
        try_load_js(context, module_path))
        return loaded_file_type::type_js; // file contents on top of stack
    if (module_extension == ".json" &&
        try_load_json(context, module_path)) {
        duk_put_prop_literal(context, 3 /* module object */, "exports");
        return loaded_file_type::type_json;
    }

    /* this is where we will try to load native modules some day */

    auto copy_path = std::filesystem::path(module_path);
    copy_path += ".js";
    if (try_load_js(context, copy_path))
        return loaded_file_type::type_js;

    copy_path.replace_extension(".json");
    if (try_load_json(context, copy_path))
        return loaded_file_type::type_json;

    /* this is where we will try to load native modules some day */

    return loaded_file_type::type_none;
}

loaded_file_type load_index(
    duk_context* context,
    const std::filesystem::path& path
);
loaded_file_type load_as_directory(
    duk_context* context,
    const std::filesystem::path& path
) {
    auto module_path = std::filesystem::path(path) / "package.json";
    if (try_load_json(context, module_path)) {
        duk_get_prop_literal(context, -1, "main");
        if (duk_is_string(context, -1)) {
            auto main_file_path =
                std::filesystem::path(path) / duk_get_string(context, -1);
            auto result = load_as_file(context, main_file_path);
            if (result != loaded_file_type::type_none) return result;
            result = load_as_directory(context, main_file_path);
            if (result != loaded_file_type::type_none) return result;
        }
    }

    return load_index(context, path);
}

loaded_file_type load_module_dirs(
    duk_context* context,
    const std::filesystem::path& path
) {
    /* get module dirs array */
    duk_get_global_literal(context, "Duktape");
    duk_get_prop_literal(context, -1, "modDirs");
    if (!duk_is_array(context, -1))
        return loaded_file_type::type_none;

    auto array_index = duk_get_top_index(context);

    auto length = duk_get_length(context, -1);
    for (unsigned int i = 0; i < length; i++) {
        duk_get_prop_index(context, array_index, i);
        auto dir = duk_require_string(context, -1);
        auto dir_path = std::filesystem::path(dir);
        dir_path /= path;

        auto result = load_as_file(context, dir_path);
        if (result != loaded_file_type::type_none)
            return result;

        result = load_as_directory(context, dir_path);
        if (result != loaded_file_type::type_none)
            return result;

        duk_pop(context);
    }

    return loaded_file_type::type_none;
}

loaded_file_type load_index(
    duk_context* context,
    const std::filesystem::path& path
) {
    auto index_path = std::filesystem::path(path) / "index.js";
    if (try_load_js(context, index_path))
        return loaded_file_type::type_js;

    index_path.replace_extension(".json");
    if (try_load_json(context, index_path))
        return loaded_file_type::type_json;

    return loaded_file_type::type_none;
}

bool load_file_contents(
    duk_context* context,
    const std::filesystem::path& path
);
static bool try_load_js(
    duk_context* context,
    const std::filesystem::path& path
) {
    return load_file_contents(context, path);
}

static bool try_load_json(
    duk_context* context,
    const std::filesystem::path& path
) {
    if (!load_file_contents(context, path))
        return false;
    duk_json_decode(context, -1);
    return true;
}

bool load_file_contents(
    duk_context* context,
    const std::filesystem::path& path
) {
    if (!js::module::fs->is_file(path))
        return false;

    js::utils::read_file(context, path);
    return true;
}
