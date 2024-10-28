#define _USE_MATH_DEFINES

#include "airplane.h"
#include "scheduler.h"
#include "utils.h"
#include "params.h"
#include "ship.h"

#include <iostream>
#include <cmath>
#include <cassert>

static float _PI = static_cast<float>(M_PI);
static float _2PI = _PI * 2.0f;
static float _PI_2 = _PI / 2.0f;

Airplane::Airplane() : target_params(), mesh(nullptr), angle(0.0f), current_speed(0.0f) {
}

void Airplane::init(const Vector2& initTarget) {
    assert( !mesh );
    mesh = scene::createAircraftMesh();
    position = Ship::getInstance()->getPosition();
    angle = Ship::getInstance()->getAngle();
    target_params.target = initTarget;

    state = TAKEOFF;
    scheduler.scheduleTask([&]() {
        computeTargetTangentParams();
        state = MOVE_TO_TARGET;
    }, 1.5f);
    scheduler.scheduleTask([&]() {
        state = MOVE_TO_LANDING;
    }, params::aircraft::MAX_AIRPLANE_FLY_TIME);
}


void Airplane::deinit() {
    scene::destroyMesh(mesh);
    mesh = nullptr;
    scheduler.unscheduleAllTasks();
}

void Airplane::update(float dt) {
    scheduler.update(dt);
    
    float targetLinearSpeed = 0.0f;
    float angularSpeed = 0.0f;

    // logic dependent of current state
    using namespace params::aircraft;
    switch (state) {
    case TAKEOFF:
        targetLinearSpeed = LINEAR_SPEED_MAX / 2.0f;
        angle = Ship::getInstance()->getAngle();
        break;
    case MOVE_TO_TARGET: {
        updateTargetTangent();
        
        if ((target_params.tangent - position).length() < 0.2f) {
            state = TURN_AROUND_TARGET;
            break;
        }
        targetLinearSpeed = LINEAR_SPEED_MAX;
        angularSpeed = getAngularSpeed(target_params.tangent, 0.7f);
        
        float distanceErr = target_params.turnRadius - (target_params.target - position).length();
        if (distanceErr > 0.0f) angularSpeed = 0.0f;
        break;
    }
    case TURN_AROUND_TARGET: {
        updateTargetTangent();
        //angularSpeed = target_params.turnAngularSpeed;
        targetLinearSpeed = target_params.turnLinearSpeed;
        angularSpeed = getAngularSpeed(target_params.tangent, 2.5f);

        float distanceErr = target_params.turnRadius - (target_params.target - position).length();
        if (distanceErr > 0.0f) angularSpeed = 0.0f;
        
        std::cout << "airplane have distanceErr=" << distanceErr << std::endl;
        break;
    }
    case MOVE_TO_LANDING: {
        Vector2 moveVector = Ship::getInstance()->getPosition() - position;
        if (moveVector.length() < 2.0f) {
            state = LANDING;
            return;
        }
        
        targetLinearSpeed = LINEAR_SPEED_MAX;
        angularSpeed = getAngularSpeed(Ship::getInstance()->getPosition(), 1.0f);
        
        float distanceErr = targetLinearSpeed/angularSpeed - (Ship::getInstance()->getPosition() - position).length();
        if (distanceErr > 0.0f) angularSpeed = 0.0f;
        break;
    }
    case LANDING: {
        Vector2 moveVector = Ship::getInstance()->getPosition() - position;
        if (moveVector.length() <= 0.1f) {
            state = END;
            return;
        }
        if (moveVector.length() > 2.0f) {
            state = MOVE_TO_LANDING;
            return;
        }

        targetLinearSpeed = LINEAR_SPEED_MIN / (moveVector.length() + 1.0f);
        // ship can't swim away from airplane
        if (targetLinearSpeed < params::ship::LINEAR_SPEED)
            targetLinearSpeed = params::ship::LINEAR_SPEED + 0.2f;
        float target_angle = moveVector.angle();
        float turn_angle = normalizeTurnAngle(target_angle - angle);
        angularSpeed = turn_angle * ANGULAR_SPEED_MAX;
        
        break;
    }
    case END:
        return;
    }

    // Check angular/linear speed restrictions
    if (angularSpeed > ANGULAR_SPEED_MAX)
        angularSpeed = ANGULAR_SPEED_MAX;
    if (angularSpeed < -ANGULAR_SPEED_MAX)
        angularSpeed = -ANGULAR_SPEED_MAX;
    if (targetLinearSpeed > LINEAR_SPEED_MAX)
        targetLinearSpeed = LINEAR_SPEED_MAX;
    if (targetLinearSpeed < 0.0f)
        targetLinearSpeed = 0.0f;

    // Set current speed
    if (current_speed < targetLinearSpeed)
        current_speed += LINEAR_ACCELERATION * dt;
    if (current_speed > targetLinearSpeed)
        current_speed -= LINEAR_ACCELERATION * dt;
    if (current_speed > LINEAR_SPEED_MAX)
        current_speed = LINEAR_SPEED_MAX;
    if (current_speed < LINEAR_SPEED_MIN && state != TAKEOFF && state != LANDING)
        current_speed = LINEAR_SPEED_MIN;
    
    angle = normalizeAngle(angle + angularSpeed * dt);
    position = position + current_speed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
    scene::placeMesh( mesh, position.x, position.y, angle );
}

void Airplane::changeTarget(const Vector2& newTarget) {
    if (state == MOVE_TO_LANDING || state == LANDING) return;
    if (state == TURN_AROUND_TARGET)
        state = MOVE_TO_TARGET;
    target_params.target = newTarget;
    computeTargetTangentParams();
}

float Airplane::normalizeAngle(float angle) {
    while (angle >= _2PI)
        angle -= _2PI;
    while (angle <= -_2PI)
        angle += _2PI;
    return angle;
}

float Airplane::normalizeTurnAngle(float turn_angle) {
    if (abs(turn_angle) >= abs(turn_angle + _2PI))
        turn_angle += _2PI;
    if (abs(turn_angle) >= abs(turn_angle - _2PI))
        turn_angle -= _2PI;
    return turn_angle;
}

float Airplane::getAngularSpeed(const Vector2& target, float CORRECT_ERR) const {
    using namespace params::aircraft;
    Vector2 targetVector = target-position;
    float turnAngle = normalizeTurnAngle(targetVector.angle() - angle);
    float angularSpeed = (turnAngle * CORRECT_ERR) * ANGULAR_SPEED_MAX;
    return angularSpeed;
}

void Airplane::computeTargetTangentParams() {
    using namespace params::aircraft;
    target_params.turnAngularSpeed = utils::getRandom(0.4f, 0.8f) * ANGULAR_SPEED_MAX;
    target_params.turnLinearSpeed = LINEAR_SPEED_MIN + utils::getRandom(0.4f, 0.8f) * (LINEAR_SPEED_MAX - LINEAR_SPEED_MIN);

    // v = wR -> R = v/w
    target_params.turnRadius = target_params.turnLinearSpeed / target_params.turnAngularSpeed;

    // set optimal turning side
    Vector2 directionVector = target_params.target-position;
    float tangentAngle = std::asin(target_params.turnRadius/directionVector.length());
    float firstAngle = directionVector.angle() + tangentAngle - normalizeTurnAngle(angle);
    float secondAngle = directionVector.angle() - tangentAngle - normalizeTurnAngle(angle);
    target_params.isLeftTurn = abs(normalizeTurnAngle(firstAngle)) > abs(normalizeTurnAngle(secondAngle));
    if (!target_params.isLeftTurn)
        target_params.turnAngularSpeed = -target_params.turnAngularSpeed;
    updateTargetTangent();
}

void Airplane::updateTargetTangent() {
    Vector2 directionVector = target_params.target-position;

    // Can't get tangent inside of circle
    if (directionVector.length() < target_params.turnRadius) {
        target_params.tangent = target_params.target;
        return;
    }

    // triangle: hypot = directionVector, cat1 = turnRadius, cat2 = tangent
    float tangentAngle = std::asin(target_params.turnRadius/directionVector.length());
    float tangentVectorLength = std::sqrt(std::pow(directionVector.length(), 2.0f) -
        std::pow(target_params.turnRadius, 2.0f));
    float turnAngle = target_params.isLeftTurn ?
        directionVector.angle() - tangentAngle :
        directionVector.angle() + tangentAngle;
    
    target_params.tangent = position + Vector2::getVector(tangentVectorLength, turnAngle);
}
