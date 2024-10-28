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
        constexpr float LINEAR_SPEED_MAX = 2.f;
        constexpr float ANGULAR_SPEED_MAX = 2.5f;
        
        constexpr float LINEAR_SPEED_MIN = LINEAR_SPEED_MAX / 5.0f;
        constexpr float LINEAR_ACCELERATION = LINEAR_SPEED_MAX / 2.0f;

        constexpr int MAX_AIRPLANE_COUNT = 5;
        constexpr float MAX_AIRPLANE_FLY_TIME = 15.0f;
        constexpr float AIRPLANE_REST_TIME = 1.0f;
    }
}