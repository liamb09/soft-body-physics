#pragma once
#include <iostream>

using namespace std;

struct Vector2D {

    float x, y;

    Vector2D (float x, float y);

    float magnitude ();

    float dot (Vector2D &v, bool normalizeFirst = false);
    float dot (float ox, float oy);

    void normalize ();

};