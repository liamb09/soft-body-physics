#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/Point.hpp"

Point::Point (float x, float y, const float &m, const bool fixed) : x(x), y(y), vx(0), vy(0), m(m), fixed(fixed) {}
Point::Point (float x, float y, float vx, float vy, const float &m, const bool fixed) : x(x), y(y), vx(vx), vy(vy), m(m), fixed(fixed) {}

void Point::update (const float &dt, const float xForce, const float yForce, const float gravity) {
    // std::cout << y << "\n";
    // "F = ma" therefore "a = F/m"
    if (fixed) return;
    vx += xForce/m*dt;
    vy += yForce/m*dt - gravity*dt;
    x += vx*dt;
    y += vy*dt;
}

void Point::render (SDL_Renderer* &renderer, const int &divisions) {
    for (int i = 0; i < divisions; i++) {
        SDL_Vertex vertices[] = {
            {SDL_FPoint{x, y}, {0, 0, 0, 255}},
            {SDL_FPoint{x + m*cosf(2*M_PI/divisions*i), y + m*sinf(2*M_PI/divisions*i)}, {0, 0, 0, 255}},
            {SDL_FPoint{x + m*cosf(2*M_PI/divisions*(i+1)), y + m*sinf(2*M_PI/divisions*(i+1))}, {0, 0, 0, 255}}
        };
        SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
    }
}