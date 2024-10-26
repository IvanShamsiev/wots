#include "vector.h"
#include <cmath>

Vector2::Vector2() :
    x( 0.f ),
    y( 0.f )
{
}


Vector2::Vector2( float vx, float vy ) :
    x( vx ),
    y( vy )
{
}


Vector2::Vector2( Vector2 const &other ) :
    x( other.x ),
    y( other.y )
{
}

Vector2 Vector2::getVector(float length, float angle) {
    return Vector2(length * std::cos(angle), length * std::sin(angle));
}

float Vector2::length() const {
    return std::sqrt(x*x + y*y);
}

float Vector2::angle() const {
    return std::atan2(y, x);
}


Vector2 operator + ( Vector2 const &left, Vector2 const &right )
{
    return Vector2( left.x + right.x, left.y + right.y );
}

Vector2 operator - ( Vector2 const &left, Vector2 const &right )
{
    return Vector2( left.x - right.x, left.y - right.y );
}


Vector2 operator * ( float left, Vector2 const &right )
{
    return Vector2( left * right.x, left * right.y );
}