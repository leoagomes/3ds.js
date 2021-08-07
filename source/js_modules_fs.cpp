#include "js_modules_fs.hpp"

#include <stdio.h>

#define FILEHANDLE_THIS_HANDLE_KEY "file-handle/handle"
#define FILEHANDLE_PROTOTYPE_STASH_KEY "fs/stash/file-handle-prototype"

void check_stash_filehandle_prototype(duk_context* context);
void push_stash_filehandle_prototype(duk_context* context);
void push_fs_module_exports(duk_context* context);

extern "C" {
duk_ret_t fs_fopen(duk_context* context);
duk_ret_t fs_filehandle_close(duk_context* context);
duk_ret_t fs_filehandle_flush(duk_context* context);
duk_ret_t fs_filehandle_finalizer(duk_context* context);
duk_ret_t fs_filehandle_to_string(duk_context* context);
};

duk_ret_t js::modules::fs::open(duk_context* context) {
    check_stash_filehandle_prototype(context);
    push_fs_module_exports(context);
    return 1;
}

void push_new_file_handle_prototype(duk_context* context);
void check_stash_filehandle_prototype(duk_context* context) {
    duk_push_global_stash(context);
    if (duk_has_prop_literal(context, -1, FILEHANDLE_PROTOTYPE_STASH_KEY))
        return;
    push_new_file_handle_prototype(context);
    duk_put_prop_literal(context, -2, FILEHANDLE_PROTOTYPE_STASH_KEY);
}

void push_stash_filehandle_prototype(duk_context* context) {
    duk_push_global_stash(context);
    if (!duk_get_prop_literal(context, -1, FILEHANDLE_PROTOTYPE_STASH_KEY)) {
        duk_error(
            context,
            DUK_ERR_ERROR,
            "Could not find FileHandle prototype stash key."
        );
        return;
    }
}

void push_new_file_handle_prototype(duk_context* context) {
    duk_push_object(context);
    duk_function_list_entry function_list[] = {
        { .key = "close", .value = fs_filehandle_close, .nargs = 0 },
        { .key = "flush", .value = fs_filehandle_flush, .nargs = 0 },
        { .key = "toString", .value = fs_filehandle_to_string, .nargs = 0 },
        { 0 }
    };
    duk_put_function_list(context, -1, function_list);
    duk_push_c_function(context, fs_filehandle_finalizer, 2);
    duk_set_finalizer(context, -2);
}

void push_fs_module_exports(duk_context* context) {
    duk_push_object(context);
    duk_function_list_entry function_list[] = {
        { .key = "fopen", .value = fs_fopen, .nargs = 2 },
        { 0 }
    };
    duk_put_function_list(context, -1, function_list);
}

FILE* require_filehandle_this_handle(duk_context* context) {
    duk_push_this(context);
    duk_get_prop_literal(context, -1, FILEHANDLE_THIS_HANDLE_KEY);
    return static_cast<FILE*>(duk_require_pointer(context, -1));
}

duk_ret_t fs_fopen(duk_context* context) {
    const char* filename = duk_require_string(context, 0);
    const char* mode = duk_require_string(context, 1);

    FILE* fp = fopen(filename, mode);
    if (!fp) {
        duk_push_undefined(context);
        return 1;
    }

    duk_push_object(context); // set this.[handle] = fp
    duk_push_pointer(context, fp);
    duk_put_prop_literal(context, -2, FILEHANDLE_THIS_HANDLE_KEY);

    push_stash_filehandle_prototype(context);
    duk_set_prototype(context, -2);
    return 1;
}

duk_ret_t fs_filehandle_close(duk_context* context) {
    auto fp = require_filehandle_this_handle(context);
    auto result = fclose(fp);

    duk_push_this(context); // set this.[handle] = null
    duk_push_pointer(context, nullptr);
    duk_put_prop_literal(context, -2, FILEHANDLE_THIS_HANDLE_KEY);

    duk_push_boolean(context, result);
    return 1;
}

duk_ret_t fs_filehandle_flush(duk_context* context) {
    auto fp = require_filehandle_this_handle(context);
    auto result = fflush(fp);
    duk_push_boolean(context, result);
    return 1;
}

duk_ret_t fs_filehandle_finalizer(duk_context* context) {
    push_stash_filehandle_prototype(context);
    if (duk_equals(context, 0, -1)) // finalizer being called for prototype itself
        return 0;

    if (!duk_get_prop_literal(context, 0, FILEHANDLE_THIS_HANDLE_KEY)) {
        // god only knows what happened.
        return 0;
    }

    auto fp = static_cast<FILE*>(duk_require_pointer(context, -1));
    if (!fp) return 0;

    fclose(fp);

    duk_push_pointer(context, nullptr); // set file pointer to null
    duk_put_prop_literal(context, 0, FILEHANDLE_THIS_HANDLE_KEY);
    return 0;
}

duk_ret_t fs_filehandle_to_string(duk_context* context) {
    duk_push_string(context, "[FileHandle object]");
    return 1;
}
