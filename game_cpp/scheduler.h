#pragma once

#include <set>
#include <chrono>
#include <functional>

// Simple implement of Task class
struct Task {
    std::function<void()> function;
    uint64_t time_millis;

    bool operator<(const Task& other) const;
};

// Simple implement of Scheduler class
class Scheduler {
public:
    void scheduleTask(const std::function<void()>& function, float time);
    void unscheduleAllTasks();
    void update(float dt);

private:
    // can have several tasks with same time_millis
    std::multiset<Task> tasks;
};