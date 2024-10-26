#pragma once

#include <set>
#include <chrono>
#include <functional>
#include <vector>

struct Task {
    void* sourceObject;
    std::function<void()> function;
    uint64_t time_millis;
};

inline bool operator<(const Task& t1, const Task& t2) {
    return t1.time_millis < t2.time_millis;
}

static uint64_t getCurrentTimeMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

class Scheduler {
public:
    void scheduleTask(void* sourceObject, const std::function<void()>& function, float time) {
        auto current_time = getCurrentTimeMillis();
        tasks.emplace(Task{sourceObject, function, current_time + static_cast<int>(time * 1000)});
    }

    void unscheduleTasks(void* sourceObject) {
        std::set<Task> eraseTasks;
        for (auto& task : tasks)
            if (task.sourceObject == sourceObject)
                eraseTasks.emplace(task);
        for (auto& erase_task : eraseTasks)
            tasks.erase(erase_task);
    }

    void update(float dt) {
        auto current_time = getCurrentTimeMillis();
        for (auto& task :tasks) {
            if (task.time_millis <= current_time)
                task.function();
        }
        tasks.erase(tasks.begin(), tasks.upper_bound(Task{nullptr, nullptr, current_time}));
    }

    static Scheduler& getScheduler() { return _scheduler; }
private:
    std::set<Task> tasks;
    static Scheduler _scheduler;
};