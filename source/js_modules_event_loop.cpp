#include "js_modules_event_loop.hpp"

#include <cstdio>
#include <string>

#include "ev.hpp"

#define TIMERS_STASH_KEY "::event_loop::timers::"
#define TASKS_STASH_KEY "::event_loop::tasks::"

#define TIMER_POINTER_KEY "pointer"
#define TIMER_CALLBACK_KEY "callback"

extern "C" {
int add_timer(duk_context* context);
int remove_timer(duk_context* context);
int add_task(duk_context* context);
int timer_finalizer(duk_context* context);
};


void initialize_heap_stash(duk_context* context);
void push_event_loop_module(duk_context* context);
std::string get_timer_id(std::shared_ptr<events::timer>& t);
void push_new_timer_object(duk_context* context, std::shared_ptr<events::timer>& timer);
std::shared_ptr<events::timer>* get_timer_pointer(
    duk_context* context,
    duk_idx_t index
);

namespace js::modules::event_loop {

duk_ret_t open(duk_context* context) {
    initialize_heap_stash(context);
    push_event_loop_module(context);
    return 1;
}

};

void initialize_heap_stash(duk_context* context) {
    duk_push_heap_stash(context);
    duk_push_object(context);
    duk_put_prop_literal(context, -2, TIMERS_STASH_KEY);
    duk_push_object(context);
    duk_put_prop_literal(context, -2, TASKS_STASH_KEY);
    duk_pop(context);
}

void push_event_loop_module(duk_context* context) {
    duk_push_object(context);

    duk_function_list_entry function_list[] = {
        { .key = "addTimer", .value = add_timer, .nargs = 3 },
        { .key = "removeTimer", .value = remove_timer, .nargs = 1 },
        { .key = "addTask", .value = add_task, .nargs = 1 },
        { 0 }
    };
    duk_put_function_list(context, -1, function_list);

    duk_push_number(context, events::timer_op::discard);
    duk_put_prop_literal(context, -2, "TIMER_DISCARD");
    duk_push_number(context, events::timer_op::rearm);
    duk_put_prop_literal(context, -2, "TIMER_REARM");
}

void require_timers_object(duk_context* context) {
    duk_push_heap_stash(context);
    duk_get_prop_literal(context, -1, TIMERS_STASH_KEY);
    if (!duk_is_object(context, -1))
        duk_error(context, DUK_ERR_REFERENCE_ERROR, "timers object does not exist.");
    duk_remove(context, -2);
}

/* [js] addTimer(interval: number, callback: () => void): Timer
 * 
 * interval - minimum number of milliseconds before callback activation
 * callback - the function to be called
 *
 * returns the timer id
 */
int add_timer(duk_context* context) {
    auto interval = duk_require_number(context, 0);;
    duk_require_function(context, 1);
    require_timers_object(context);

    auto timer = ev::loop->add_timer(interval);
    auto timer_id = get_timer_id(timer);

    // create timer object
    push_new_timer_object(context, timer);
    duk_put_prop_string(context, -2, timer_id.c_str());

    // update timer callback
    timer->set_callback([context, timer_id]() {
        int returned_timer_op = events::timer_op::discard;

        require_timers_object(context);
        auto timers_object_index = duk_get_top_index(context);

        duk_get_prop_string(context, -1, timer_id.c_str());
        if (!duk_is_object(context, -1)) // timer isn't even object... weird
            return events::timer_op::discard;

        duk_get_prop_literal(context, -1, TIMER_CALLBACK_KEY);
        if (!duk_is_function(context, -1))
            goto discard_timer;

        printf("calling callback.\n");

        duk_pcall(context, 0);
        returned_timer_op = duk_get_int_default(
            context,
            -1,
            events::timer_op::discard
        );

        if (returned_timer_op == events::timer_op::rearm)
            return events::timer_op::rearm;

        // should discard timer

        discard_timer:
        printf("discarding timer...");
        duk_del_prop_string(context, timers_object_index, timer_id.c_str());
        return events::timer_op::discard;
    });

    duk_push_string(context, timer_id.c_str());
    return 1;
}

/* [js] removeTimer(id: string) => void): bool
 * 
 * id - the id of the timer
 *
 * returns whether or not the timer was removed (true = removed, false = not found)
 */
int remove_timer(duk_context* context) {
    auto timer_id = duk_require_string(context, 0);

    require_timers_object(context);
    if (!duk_get_prop_string(context, -1, timer_id)) {
        duk_push_false(context);
        return 1;
    }

    auto* timer_pointer = get_timer_pointer(context, -1);
    bool removed = false;
    if (timer_pointer != nullptr) {
        removed = ev::loop->remove_timer(*timer_pointer);
    }
    duk_del_prop_string(context, -1, timer_id);

    duk_push_boolean(context, removed);
    return 1;
}

int add_task(duk_context* context) {
    return 0;
}

int timer_finalizer(duk_context* context) {
    duk_require_object(context, 0);
    auto* timer_pointer = get_timer_pointer(context, -1);
    if (timer_pointer != nullptr) {
        delete timer_pointer;
    }
    return 0;
}

std::string get_timer_id(std::shared_ptr<events::timer>& t) {
    void* p = t.get();
    char timer_id[sizeof(p) * 2 + 1] = {0};
    std::snprintf(timer_id, sizeof(timer_id), "%p", p);
    return std::string(timer_id);
}

void push_new_timer_object(duk_context* context, std::shared_ptr<events::timer>& timer) {
    duk_push_object(context);
    auto* timer_ptr = new std::shared_ptr<events::timer>(timer);
    duk_push_pointer(context, timer_ptr);
    duk_put_prop_literal(context, -2, TIMER_POINTER_KEY);
    duk_dup(context, 1);
    duk_put_prop_literal(context, -2, TIMER_CALLBACK_KEY);
    duk_push_c_function(context, timer_finalizer, 1);
    duk_set_finalizer(context, -2);
}

std::shared_ptr<events::timer>* get_timer_pointer(
    duk_context* context,
    duk_idx_t index
) {
    duk_get_prop_literal(context, index, TIMER_POINTER_KEY);
    void* timer_void_pointer = duk_get_pointer_default(context, -1, nullptr);
    duk_pop(context);
    return static_cast<std::shared_ptr<events::timer>*>(timer_void_pointer);
}

