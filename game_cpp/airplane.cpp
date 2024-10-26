#include "airplane.h"
#include "scheduler.h"
#include <iostream>

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
    float angularSpeed = 1.0f;

    // Logic there

    angle = angle + angularSpeed * dt;
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
