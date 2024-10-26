#pragma once

#include <set>
#include <chrono>
#include <functional>

struct Task {
    const std::function<void()> function;
    const uint64_t time_millis;
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
    void scheduleTask(const std::function<void()>& function, float time);
    void unscheduleAllTasks();
    void update(float dt);

private:
    std::multiset<Task> tasks;
};