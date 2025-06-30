#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Point.hpp"
#include "Spring.hpp"
#include "Shape.hpp"

class SoftBodyPhysics2D {

    std::vector<Shape> shapes;
    std::vector<Spring> springs;
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

    void addShape (Shape shape);
    void addShape (std::vector<Point> points, const bool &fixed = false);
    void addShape (std::vector<Point>* points, const bool &fixed = false);

    // adds a rectangle with its top left corner at (x,y)
    void addRect (const float &x, const float &y, const float &width, const float &height, const float &m = 5, const bool &fixed = false);

    void addLinkedShape (std::vector<std::vector<Point>> pointSets, std::vector<std::vector<std::pair<int, int>>> links, const std::vector<bool> &fixed);
    void addLink (int iShape1, int iPoint1, int iShape2, int iPoint2); // indexes for shape and point to link to, iShape1 and iShape2 can be negative to go to last item (-1=last item, -2=second-to-last item)

};