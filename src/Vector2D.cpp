#include <iostream>
#include <cmath>
#include "../include/Vector2D.hpp"

using namespace std;

Vector2D::Vector2D (float x, float y) : x(x), y(y) {}

float Vector2D::magnitude () {
    return sqrt(x*x + y*y);
}

float Vector2D::dot (Vector2D &v, bool normalizeFirst) {
    if (normalizeFirst)
        return x/magnitude()*v.x/v.magnitude() + y/magnitude()*v.y/v.magnitude();
    else
        return x*v.x + y*v.y;
}
float Vector2D::dot (float ox, float oy) {
    return x*ox + y*oy;
}

void Vector2D::normalize () {
    float mag = magnitude();
    x /= mag;
    y /= mag;
}

Vector2D operator+(Vector2D lhs, Vector2D rhs) {
    return Vector2D(lhs.x+rhs.x, lhs.y+rhs.y);
}
Vector2D operator-(Vector2D lhs, Vector2D rhs) {
    return Vector2D(lhs.x-rhs.x, lhs.y-rhs.y);
}
Vector2D operator*(float lhs, Vector2D rhs) {
    return Vector2D(lhs*rhs.x, lhs*rhs.y);
}
ostream& operator<<(ostream& lhs, Vector2D rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << ")";
    return lhs;
}