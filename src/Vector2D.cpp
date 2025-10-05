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

void Vector2D::normalize () {
    x /= magnitude();
    y /= magnitude();
}