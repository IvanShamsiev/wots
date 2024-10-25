#pragma once

#include <cassert>
#include <cmath>

#include "../framework/scene.hpp"
#include "../framework/game.hpp"

#include "vector.h"
#include "params.h"

//-------------------------------------------------------
//	Simple ship logic
//-------------------------------------------------------

class Ship
{
public:
    Ship();

    void init();
    void deinit();
    void update( float dt );
    void keyPressed( int key );
    void keyReleased( int key );
    void mouseClicked( Vector2 worldPosition, bool isLeftButton );

    Vector2 getPosition() const { return position; }
    float getAngle() const { return angle; }

    static void setShip(Ship& ship) { _ship = &ship; }
    static const Ship* getShip() { return _ship; }

private:
    
    static Ship* _ship;
    scene::Mesh *mesh;
    Vector2 position;
    float angle;

    bool input[ game::KEY_COUNT ];
};
