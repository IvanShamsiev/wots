#define _USE_MATH_DEFINES

#include "airplane.h"
#include "scheduler.h"
#include "utils.h"

#include <iostream>
#include <cmath>

static float _PI = static_cast<float>(M_PI);
static float _2PI = _PI * 2.0f;
static float _PI_2 = _PI / 2.0f;

Airplane::Airplane() : target_params(), mesh(nullptr), angle(0.0f), current_speed(0.0f) {
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
        prevMoveVector = target_params.tangent - position;
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
    switch (state) {
    case TAKEOFF:
        linearSpeed = LINEAR_SPEED_MIN;
        angle = Ship::getShip()->getAngle();
        break;
    /*case MOVE_TO_TARGET:
        updateTargetTangent();
    {
        Vector2 moveVector = target_params.tangent - position;
        if (moveVector.length() < 0.2f) {
            std::cout << "airplane " << id << " set turn around" << std::endl;
            state = TURN_AROUND_TARGET;
            update(dt);
            return;
        }
        prevMoveVector = moveVector;
        linearSpeed = LINEAR_SPEED_MAX * 0.8f;
        
        float target_angle = (target_params.tangent-position).angle();
        float turn_angle = target_angle - angle;
        if (abs(turn_angle) >= abs(turn_angle + _2PI))
            turn_angle += _2PI;
        if (abs(turn_angle) >= abs(turn_angle - _2PI))
            turn_angle -= _2PI;
        
        angularSpeed = turn_angle * ANGULAR_SPEED_MAX;
    }*/
        break;
    case MOVE_TO_TARGET:
    case TURN_AROUND_TARGET:
        updateTargetTangent();
        angularSpeed = target_params.turnAngularSpeed;
        linearSpeed = target_params.turnLinearSpeed;
        {
            float distanceErr = target_params.turnRadius - (target_params.target - position).length();
            //angularSpeed *= 1.0f + distanceErr/target_params.turnRadius;
            //linearSpeed /= 1.0f + distanceErr/target_params.turnRadius;
            std::cout << "airplane have distanceErr=" << distanceErr << std::endl;

        
        
            float target_angle = (target_params.tangent-position).angle();
            float turn_angle = target_angle - angle;
            if (abs(turn_angle) >= abs(turn_angle + _2PI))
                turn_angle += _2PI;
            if (abs(turn_angle) >= abs(turn_angle - _2PI))
                turn_angle -= _2PI;

            if (distanceErr > 0.0f) angularSpeed = 0.0f;
            else angularSpeed = turn_angle * ANGULAR_SPEED_MAX;
        }
        break;
    case MOVE_TO_LANDING: {
            Vector2 moveVector = Ship::getShip()->getPosition() - position;
            float timeToSlowDown = (current_speed - LINEAR_SPEED_MIN)/LINEAR_ACCELERATION;
            if (moveVector.length() < current_speed*(timeToSlowDown + 0.5f) - LINEAR_ACCELERATION * timeToSlowDown * timeToSlowDown) {
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
            
            angularSpeed = turn_angle * ANGULAR_SPEED_MAX;
            float distanceErr = linearSpeed/angularSpeed - (Ship::getShip()->getPosition() - position).length();
            if (distanceErr > 0.0f) angularSpeed = 0.0f;
        }
        break;
    case LANDING:
        {
            Vector2 moveVector = Ship::getShip()->getPosition() - position;
            if (moveVector.length() <= 0.1) {
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
        
            angularSpeed = turn_angle * ANGULAR_SPEED_MAX;
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
    if (current_speed < linearSpeed)
        current_speed += LINEAR_ACCELERATION * dt;
    if (current_speed > linearSpeed)
        current_speed -= LINEAR_ACCELERATION * dt;
    if (current_speed > LINEAR_SPEED_MAX)
        current_speed = LINEAR_SPEED_MAX;
    if (current_speed < LINEAR_SPEED_MIN && state != TAKEOFF && state != LANDING)
        current_speed = LINEAR_SPEED_MIN;
    position = position + current_speed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
    scene::placeMesh( mesh, position.x, position.y, angle );
}

void Airplane::changeTarget(const Vector2& newTarget) {
    if (state == MOVE_TO_LANDING || state == LANDING) return;
    if (state == TURN_AROUND_TARGET)
        state = MOVE_TO_TARGET;
    target = newTarget;
    target_params.target = newTarget;
    computeTargetTangentParams();
    
    prevMoveVector = target_params.tangent - position;
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
    target_params.turnAngularSpeed = utils::getRandom(0.4f, 0.8f) * ANGULAR_SPEED_MAX;
    target_params.turnLinearSpeed = LINEAR_SPEED_MIN + utils::getRandom(0.4f, 0.8f) * (LINEAR_SPEED_MAX - LINEAR_SPEED_MIN);
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
    
    //target_params.tangent = position + (directionVector + radiusVector);
    float angle = target_params.isLeftTurn ?
        directionVector.angle() + tangentAngle :
        directionVector.angle() - tangentAngle;
    target_params.tangent = position + Vector2::getVector(tangentVectorLength, angle);
}
