#pragma once

#include <memory>

#include "../framework/game.hpp"
#include "../framework/scene.hpp"

#include "vector.h"

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

    static void setInstance(const std::shared_ptr<Ship>& ship) {
        _ship = ship;
    }
    static std::shared_ptr<Ship> getInstance() { return _ship; }

private:
    
    static std::shared_ptr<Ship> _ship;
    scene::Mesh *mesh;
    Vector2 position;
    float angle;

    bool input[ game::KEY_COUNT ];
};
