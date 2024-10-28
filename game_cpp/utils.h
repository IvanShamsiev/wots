#pragma once
#include <stdlib.h>
#include <random>

namespace utils
{
    // Returns random value [0, 1]
    static float getRandom() {
        return static_cast<float>(std::rand())/RAND_MAX;
    }

    // Returns random value [min, max]
    static float getRandom(float min, float max) {
        float rand = getRandom();
        return min + rand * (max-min);
    }

    static uint64_t getCurrentTimeMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
}
