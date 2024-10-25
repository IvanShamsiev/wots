#include "airplane.h"

Airplane::Airplane() :
    mesh(nullptr)
{
}


void Airplane::init(const Ship& ship)
{
    assert( !mesh );
    mesh = scene::createAircraftMesh();
    position = ship.getPosition();
    angle = ship.getAngle();
    target = position;
}


void Airplane::deinit()
{
    scene::destroyMesh( mesh );
    mesh = nullptr;
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

void Airplane::changeTarget(const Vector2& newTarget) {
    target = newTarget;
}
