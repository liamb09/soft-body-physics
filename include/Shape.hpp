#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Point.hpp"

struct Shape {

    std::vector<Point> points;

public:

    Shape ();
    Shape (std::vector<Point> points);

    std::vector<Point>& getPoints ();

    void update (const float &dt, const float xForce, const float yForce);
    void render (SDL_Renderer* &renderer);

    bool pointInShape(const Point &p);

};