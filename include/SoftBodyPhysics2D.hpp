#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Shape.hpp"

class SoftBodyPhysics2D {

    std::vector<Shape> shapes;
    float GRAVITY;
    float ELASTICITY; // 1 = perfectly elastic

public:

    SoftBodyPhysics2D ();
    SoftBodyPhysics2D (const float &GRAVITY);
    SoftBodyPhysics2D (const float &GRAVITY, const float &ELASTICITY);

    std::vector<Shape>& getShapes ();

    void update (const float &dt);
    void handleCollisions();
    void render (SDL_Renderer* &renderer);

    void addShape (Shape &shape);
    void addShape (std::vector<Point> points, const bool &fixed = false);

};