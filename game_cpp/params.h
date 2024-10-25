#pragma once

//-------------------------------------------------------
//	game parameters
//-------------------------------------------------------

namespace params
{
    namespace ship
    {
        constexpr float LINEAR_SPEED = 0.5f;
        constexpr float ANGULAR_SPEED = 0.5f;
    }

    namespace aircraft
    {
        constexpr float LINEAR_SPEED = 2.f;
        constexpr float ANGULAR_SPEED = 2.5f;

        constexpr int MAX_AIRPLANE_COUNT = 5;
    }
}