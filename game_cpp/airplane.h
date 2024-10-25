#pragma once

#include <cassert>
#include <cmath>

#include "vector.h"
#include "../framework/scene.hpp"

class Airplane {
public:
    Airplane();

    void init();
    void deinit();
    void update( float dt );
    

private:
    scene::Mesh *mesh;
    Vector2 position;
    float angle;
};





Airplane::Airplane() :
    mesh( nullptr )
{
}


void Airplane::init()
{
    assert( !mesh );
    mesh = scene::createShipMesh();
    position = Vector2( 0.f, 0.f );
    angle = 0.f;
    for ( bool &key : input )
        key = false;
}


void Airplane::deinit()
{
    scene::destroyMesh( mesh );
    mesh = nullptr;
}


void Airplane::update( float dt )
{
    float linearSpeed = 0.f;
    float angularSpeed = 0.f;

    // Logic there

    angle = angle + angularSpeed * dt;
    position = position + linearSpeed * dt * Vector2( std::cos( angle ), std::sin( angle ) );
    scene::placeMesh( mesh, position.x, position.y, angle );
}