#define _USE_MATH_DEFINES

#include "airplane.h"
#include "scheduler.h"
#include "utils.h"

#include <iostream>
#include <cmath>

static float _PI = static_cast<float>(M_PI);
static float _2PI = _PI * 2.0f;
static float _PI_2 = _PI / 2.0f;

Airplane::Airplane() : target_params(), mesh(nullptr), angle(0.0f) {
}

void Airplane::init(const Ship& ship, const Vector2& initTarget)
{
    assert( !mesh );
    mesh = scene::createAircraftMesh();
    position = ship.getPosition();
    angle = ship.getAngle();
    target = initTarget;
    target_params.target = initTarget;

    //schedule_position();

    state = TAKEOFF;
    scheduler.scheduleTask([&]() {
        computeTargetTangentParams();
        state = MOVE_TO_TARGET;
    }, 1.0f);
    scheduler.scheduleTask([&]() {
        state = MOVE_TO_LANDING;
    }, params::aircraft::MAX_AIRPLANE_FLY_TIME);
}


void Airplane::deinit()
{
    scene::destroyMesh( mesh );
    mesh = nullptr;
    scheduler.unscheduleAllTasks();
}

void Airplane::update(float dt)
{
    scheduler.update(dt);
    
    float linearSpeed = 0.0f;
    float angularSpeed = 0.0f;

    using namespace params::aircraft;
    // Logic there
    switch (state) {
    case TAKEOFF:
        linearSpeed = LINEAR_SPEED_MIN;
        angle = Ship::getShip()->getAngle();
        break;
    case MOVE_TO_TARGET:
        updateTargetTangent();
        if ((target_params.tangent - position).length() < 0.01f) {
            state = TURN_AROUND_TARGET;
            update(dt);
            return;
        }
        linearSpeed = LINEAR_SPEED_MAX * 0.8f;
    {
        float target_angle = (target_params.tangent-position).angle();
        float turn_angle = target_angle - angle;
        if (abs(turn_angle) >= abs(turn_angle + _2PI))
            turn_angle += _2PI;
        if (abs(turn_angle) >= abs(turn_angle - _2PI))
            turn_angle -= _2PI;
        
        angularSpeed = turn_angle * _PI;
    }
        break;
    case TURN_AROUND_TARGET:
        angularSpeed = target_params.turnAngularSpeed;
        linearSpeed = target_params.turnLinearSpeed;
        {
            /*float distanceErr = target_params.turnRadius - (target_params.target - position).length();
            angularSpeed *= 1.0f + distanceErr/target_params.turnRadius/10.0f;
            linearSpeed /= 1.0f + distanceErr/target_params.turnRadius/10.0f;*/
        }
        break;
    case MOVE_TO_LANDING: {
            Vector2 moveVector = Ship::getShip()->getPosition() - position;
            if (moveVector.length() < 1.0f) {
                state = LANDING;
                update(dt);
                return;
            }
            
            linearSpeed = LINEAR_SPEED_MAX * 0.8f;
            
            float target_angle = moveVector.angle();
            float turn_angle = target_angle - angle;
            if (abs(turn_angle) >= abs(turn_angle + _2PI))
                turn_angle += _2PI;
            if (abs(turn_angle) >= abs(turn_angle - _2PI))
                turn_angle -= _2PI;
            
            angularSpeed = turn_angle * _PI;
        }
        break;
    case LANDING:
        {
            Vector2 moveVector = Ship::getShip()->getPosition() - position;
            if (moveVector.length() <= 0.01) {
                state = END;
                return;
            }
            linearSpeed = LINEAR_SPEED_MIN;
        
            float target_angle = moveVector.angle();
            float turn_angle = target_angle - angle;
            if (abs(turn_angle) >= abs(turn_angle + _2PI))
                turn_angle += _2PI;
            if (abs(turn_angle) >= abs(turn_angle - _2PI))
                turn_angle -= _2PI;
        
            angularSpeed = turn_angle * _PI;
        }
        
        break;
    case END:
        return;
    }

    if (angularSpeed > ANGULAR_SPEED_MAX)
        angularSpeed = ANGULAR_SPEED_MAX;
    if (linearSpeed > LINEAR_SPEED_MAX)
        linearSpeed = LINEAR_SPEED_MAX;
    angle = angle + angularSpeed * dt;
    normalizeAngle(angle);
    position = position + linearSpeed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
    scene::placeMesh( mesh, position.x, position.y, angle );
}

void Airplane::changeTarget(const Vector2& newTarget) {
    if (state == MOVE_TO_LANDING || state == LANDING) return;
    if (state == TURN_AROUND_TARGET)
        state = MOVE_TO_TARGET;
    target = newTarget;
    target_params.target = newTarget;
    computeTargetTangentParams();
}

float Airplane::getTargetAngle(const Vector2& move_vector) {
    float target_angle = std::atan2(move_vector.y, move_vector.x);
    return target_angle;
}

void Airplane::normalizeAngle(float& angle) {
    while (angle >= _PI * 2.0f)
        angle -= _PI * 2.0f;
    while (angle <= -_PI * 2.0f)
        angle += _PI * 2.0f;
}

void Airplane::computeTargetTangentParams() {
    using namespace params::aircraft;
    target_params.turnAngularSpeed = utils::getRandom(0.75f, 1.0f) * ANGULAR_SPEED_MAX;
    target_params.turnLinearSpeed = LINEAR_SPEED_MIN + utils::getRandom(0.5f, 1.0f) * (LINEAR_SPEED_MAX - LINEAR_SPEED_MIN);
    target_params.turnRadius = target_params.turnLinearSpeed / target_params.turnAngularSpeed;

    target_params.isLeftTurn = utils::getRandom() > 0.5f;
    if (target_params.isLeftTurn)
        target_params.turnAngularSpeed *= -1.0f;
    updateTargetTangent();
}

void Airplane::updateTargetTangent() {
    Vector2 directionVector = target_params.target-position;
    if (directionVector.length() < target_params.turnRadius) {
        target_params.tangent = target_params.target;
        return;
    }
    float tangentAngle = std::asin(target_params.turnRadius/directionVector.length());
    float tangentVectorLength = std::sqrt(std::pow(directionVector.length(), 2.0f) - std::pow(target_params.turnRadius, 2.0f));
    float radiusVectorAngle = target_params.isLeftTurn ?
        directionVector.angle() + (_PI_2 - tangentAngle) :
        directionVector.angle() - (_PI_2 - tangentAngle);
    Vector2 radiusVector = Vector2::getVector(target_params.turnRadius, radiusVectorAngle);
    
    target_params.tangent = position + (directionVector + radiusVector);
    float angle = target_params.isLeftTurn ?
        directionVector.angle() + tangentAngle :
        directionVector.angle() - tangentAngle;
    target_params.tangent = position + Vector2::getVector(tangentVectorLength, angle);
}
