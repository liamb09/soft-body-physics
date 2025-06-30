#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Spring.hpp"
#include "Point.hpp"

class Shape {

    std::vector<Point> points;
    std::vector<Spring> springs;
    bool fixed = false;

    // declaring vars for collision handling to save memory
    float slope, xInterception, yInterception, intersectX, lineVelocityX, lineVelocityY, intVecX, intVecY, pseudoPointX, pseudoPointY, projectionFactor, wPoint, wL1, wL2, weightTotal, normX, normY, normLen, relVX, relVY, relVN, impulseMagnitude;

public:

    std::vector<float> outermostPoints; // top right bottom left

    Shape (std::vector<Point> points, const bool &fixed = false);
    // Shape (std::vector<Point>* points, const bool &fixed = false);
    Shape (std::vector<Point> points, const std::vector<std::pair<int, int>> &springPairs, const bool &fixed = false);

    std::vector<Point>& getPoints ();
    Point& getPoint (const int &index);

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer);

    void handleCollisions (Shape &otherShape, const float &ELASTICITY);
    float pointToLineDistance (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY);
    void resolveVelocities (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY);

};