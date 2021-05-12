#pragma once

#include "duktape.h"

/* Use a proxy wrapper to make undefined methods (console.foo()) no-ops. */
#define DUK_CONSOLE_PROXY_WRAPPER  (1U << 0)

/* Flush output after every call. */
#define DUK_CONSOLE_FLUSH          (1U << 1)

/* Send output to stdout only (default is mixed stdout/stderr). */
#define DUK_CONSOLE_STDOUT_ONLY    (1U << 2)

/* Send output to stderr only (default is mixed stdout/stderr). */
#define DUK_CONSOLE_STDERR_ONLY    (1U << 3)

namespace js::console {

/* Initialize the console system */
extern void init(duk_context *ctx, duk_uint_t flags);

};
