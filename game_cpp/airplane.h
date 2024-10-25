#pragma once

#include <cassert>
#include <cmath>

#include "../framework/scene.hpp"

#include "vector.h"
#include "ship.h"

class Airplane {
public:
    Airplane();

    void init(const Ship& ship);
    void deinit();
    void update(float dt);

    void changeTarget(const Vector2& newTarget);

private:
    scene::Mesh *mesh;
    Vector2 position;
    float angle;

    Vector2 target;
};
