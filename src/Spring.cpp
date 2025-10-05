#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/Point.hpp"
#include "../include/Spring.hpp"

using namespace std;

Spring::Spring (Point* p1, Point* p2, const float &k) : p1(p1), p2(p2), k(k), damping(0.9) {
    restDistance = sqrt(pow((*p1).x - (*p2).x, 2) + pow((*p1).y - (*p2).y, 2));
}

Spring::Spring (Point* p1, Point* p2, const float &k, const float &damping) : p1(p1), p2(p2), k(k), damping(damping) {
    restDistance = sqrt(pow((*p1).x - (*p2).x, 2) + pow((*p1).y - (*p2).y, 2));
}

Spring::Spring (Point* p1, Point* p2, const float &k, const float &damping, const float &restDistance) : p1(p1), p2(p2), k(k), damping(damping), restDistance(restDistance) {}

void Spring::update (const float &dt) {
    dirLen = sqrt(pow((*p2).x - (*p1).x, 2) + pow((*p2).y - (*p1).y, 2));
    normDirX = ((*p2).x - (*p1).x)/dirLen;
    normDirY = ((*p2).y - (*p1).y)/dirLen;

    vRelX = (*p2).vx - (*p1).vx;
    vRelY = (*p2).vy - (*p1).vy;

    //  |----------SPRING FORCE-----------|  |------------------DAMPING FORCE------------------|
    (*p1).update(dt, 
        k*(dirLen - restDistance)*normDirX + damping*(vRelX*normDirX + vRelY*normDirY)*normDirX,
        k*(dirLen - restDistance)*normDirY + damping*(vRelX*normDirX + vRelY*normDirY)*normDirY,
    0);
    (*p2).update(dt, 
        -k*(dirLen - restDistance)*normDirX - damping*(vRelX*normDirX + vRelY*normDirY)*normDirX,
        -k*(dirLen - restDistance)*normDirY - damping*(vRelX*normDirX + vRelY*normDirY)*normDirY,
    0);
}

void Spring::render (SDL_Renderer* renderer) {
    // cout << "(" << (*p1).x << ", " << (*p1).y << ") to (" << (*p2).x << ", " << (*p2).y << ")\n";
    SDL_RenderLine(renderer, (*p1).x, (*p1).y, (*p2).x, (*p2).y);
}