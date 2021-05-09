#pragma once

#include <functional>

namespace events {

typedef std::function<void(void)> task_fn;

class task {
private:
    task_fn callback;

public:
    task(task_fn callback) : callback(callback) {}

    void operator()() {
        return callback();
    }
};

};
