#pragma once

#include <cassert>
#include <cmath>
#include <functional>

#include "scheduler.h"
#include "../framework/scene.hpp"

#include "vector.h"
#include "ship.h"

class Airplane {
public:

    enum State {
        TAKEOFF,
        MOVE_TO_TARGET,
        TURN_AROUND_TARGET,
        MOVE_TO_LANDING,
        LANDING,
        END
    } state = TAKEOFF;

    struct TargetParams {
        Vector2 target;
        Vector2 tangent;
        float turnAngularSpeed;
        float turnLinearSpeed;
        float turnRadius;
        bool isLeftTurn;
    } target_params;
    
    Airplane();

    void init(const Ship& ship, const Vector2& initTarget);
    void deinit();
    void setOnLandingCallback(const std::function<void(const Airplane* airpln)>& callback);
    void update(float dt);

    void schedule_position();

    void changeTarget(const Vector2& newTarget);
    static float getTargetAngle(const Vector2& move_vector);
    static void normalizeAngle(float& angle);

    void computeTargetTangentParams();
    void updateTargetTangent();

private:
    Scheduler scheduler;
    
    scene::Mesh *mesh;
    Vector2 position;
    float angle;
    float current_speed;

    Vector2 prevMoveVector;

    Vector2 target;
};
