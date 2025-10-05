#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Point.hpp"

using namespace std;

struct Spring {

    Point* p1;
    Point* p2;
    float k = 750; // spring constant
    float damping = 30;
    float restDistance;
    float dirLen, normDirX, normDirY, vRelX, vRelY;

    Spring (Point* p1, Point* p2, const float &k);
    Spring (Point* p1, Point* p2, const float &k, const float &damping);
    Spring (Point* p1, Point* p2, const float &k, const float &damping, const float &restDistance);

    void update (const float &dt);

    void render (SDL_Renderer* renderer);

};