#include "scheduler.h"

void Scheduler::scheduleTask(const std::function<void()>& function, float time) {
    auto current_time = getCurrentTimeMillis();
    tasks.emplace(Task{ function, current_time + static_cast<int>(time * 1000.0f)});
}

void Scheduler::unscheduleAllTasks() {
    tasks.clear();
}

void Scheduler::update(float dt) {
    auto current_time = getCurrentTimeMillis();
    for (auto& task : tasks) {
        if (task.time_millis <= current_time)
            task.function();
        else break;
    }
    tasks.erase(tasks.begin(), tasks.upper_bound(Task{nullptr, current_time}));
}
