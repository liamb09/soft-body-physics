#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include "Vector2D.hpp"

using namespace std;

struct Point {

    float x, y;
    Vector2D v;
    float m;
    bool fixed;

    Point (float x, float y, const float &m = 5, const bool fixed = false);
    Point (float x, float y, float vx, float vy, const float &m = 5, const bool fixed = false);

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer, const int &divisions = 15);

};