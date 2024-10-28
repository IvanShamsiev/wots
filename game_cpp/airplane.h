#pragma once

#include "scheduler.h"
#include "../framework/scene.hpp"

#include "vector.h"

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

    void init(const Vector2& initTarget);
    void deinit();
    void update(float dt);

    static float normalizeAngle(float angle);
    static float normalizeTurnAngle(float turn_angle);

    // CORRECT_ERR higher -> less turn-around error but less rotation smoothless
    float getAngularSpeed(const Vector2& target, float CORRECT_ERR) const;
    
    void changeTarget(const Vector2& newTarget);
    void computeTargetTangentParams();
    void updateTargetTangent();

private:
    Scheduler scheduler;
    scene::Mesh *mesh;
    
    Vector2 position;
    float angle;
    float current_speed;
};
