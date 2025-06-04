#pragma once
#include <iostream>
#include <SDL3/SDL.h>

struct Point {

    float x, y, vx, vy;
    float m;

    Point (float x, float y, const float &m);
    Point (float x, float y, float vx, float vy, const float &m);

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer, const int &divisions = 15);

};