#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Point.hpp"

struct Spring {

    Point* p1;
    Point* p2;
    float k; // spring constant
    float damping; // between 0 and 1; 0 = no damping
    float restDistance;
    float dirLen, normDirX, normDirY, vRelX, vRelY;

    Spring (Point* p1, Point* p2, const float &k);
    Spring (Point* p1, Point* p2, const float &k, const float &damping);
    Spring (Point* p1, Point* p2, const float &k, const float &damping, const float &restDistance);

    void update (const float &dt);

    void render (SDL_Renderer* renderer);

};