#pragma once

#include <algorithm>
#include <forward_list>
#include <memory>
#include <limits>

namespace events {

typedef std::function<void(void)> task_fn;

class task_queue {
private:
    std::forward_list<task_fn> current_tasks, next_tasks;

public:
    task_queue() : current_tasks(), next_tasks() {}

    unsigned int process(unsigned int max_tasks = std::numeric_limits<unsigned int>::max()) {
        unsigned int processed_count = 0;

        while (!current_tasks.empty() && processed_count < max_tasks) {
            auto task = current_tasks.front();
            task();

            current_tasks.pop_front();
            processed_count++;
        }

        if (current_tasks.empty())
            std::swap(current_tasks, next_tasks);

        return processed_count;
    }

    void add_task(task_fn callback) {
        next_tasks.push_front(callback);
    }
};

};
