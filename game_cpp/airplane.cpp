#define _USE_MATH_DEFINES

#include "airplane.h"
#include "scheduler.h"

#include <iostream>
#include <cmath>

static float _PI = static_cast<float>(M_PI);
static float _2PI = _PI * 2.0f;

Airplane::Airplane() :
    mesh(nullptr)
{
}


void Airplane::init(const Ship& ship, const Vector2& initTarget)
{
    assert( !mesh );
    mesh = scene::createAircraftMesh();
    position = ship.getPosition();
    angle = ship.getAngle();
    target = initTarget;

    schedule_position();

    state = MOVE_TO_TARGET;
}


void Airplane::deinit()
{
    scene::destroyMesh( mesh );
    mesh = nullptr;
    Scheduler::getScheduler().unscheduleTasks(this);
}


void Airplane::update( float dt )
{
    float linearSpeed = 3.0f;
    float angularSpeed = 0.0f;

    using namespace params::aircraft;
    // Logic there
    switch (state) {
    case TAKEOFF:
        linearSpeed = LINEAR_SPEED_MIN;
        break;
    case MOVE_TO_TARGET:
        if (linearSpeed < LINEAR_SPEED_MAX)
            linearSpeed = LINEAR_SPEED_MIN/5.0f;
    {
        float target_angle = getTargetAngle(target-position);
        float turn_angle = target_angle - angle;
        if (abs(turn_angle) >= abs(turn_angle + _2PI))
            turn_angle += _2PI;
        if (abs(turn_angle) >= abs(turn_angle - _2PI))
            turn_angle -= _2PI;
        
        angularSpeed = turn_angle * _PI;
        if (angularSpeed > ANGULAR_SPEED_MAX)
            angularSpeed = ANGULAR_SPEED_MAX;
    }
        break;
    case MOVE_TO_LANDING:
        break;
    }

    angle = angle + angularSpeed * dt;
    normalizeAngle(angle);
    position = position + linearSpeed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
    scene::placeMesh( mesh, position.x, position.y, angle );
}

void Airplane::schedule_position() {
    std::cout << "position x=" << position.x << "; y=" << position.y << std::endl;
    Scheduler::getScheduler().scheduleTask(this, [&]() {schedule_position();}, 1.0f);
}

void Airplane::changeTarget(const Vector2& newTarget) {
    target = newTarget;
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
