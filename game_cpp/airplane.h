#pragma once

#include <cassert>
#include <cmath>

#include "../framework/scene.hpp"

#include "vector.h"
#include "ship.h"

class Airplane {
public:

    enum State {
        TAKEOFF,
        TURN_TO_TARGET,
        MOVE_TO_TARGET,
        TURN_AROUND_TARGET,
        MOVE_TO_LANDING,
        LANDING
    };
    Airplane();

    void init(const Ship& ship, const Vector2& initTarget);
    void deinit();
    void update(float dt);

    void schedule_position();

    void changeTarget(const Vector2& newTarget);
    static float getTargetAngle(const Vector2& move_vector);
    static void normalizeAngle(float& angle);

private:
    scene::Mesh *mesh;
    Vector2 position;
    float angle;

    Vector2 target;
    State state = TAKEOFF;
};
