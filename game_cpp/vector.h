#pragma once

//-------------------------------------------------------
//	Basic Vector2 class
//-------------------------------------------------------

class Vector2
{
public:
    float x;
    float y;

    Vector2();
    Vector2( float vx, float vy );
    Vector2( Vector2 const &other );

    static Vector2 getVector(float length, float angle);

    float length() const;
    float angle() const;
};

Vector2 operator + ( Vector2 const &left, Vector2 const &right );
Vector2 operator - ( Vector2 const &left, Vector2 const &right );
Vector2 operator * ( float left, Vector2 const &right );