#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Point.hpp"

struct Shape {

    std::vector<Point> points;
    bool fixed;

    // declaring vars for collision handling so memory is allocated each time
    float slope, xInterception, yInterception, centerX, centerY, dPointCenter, dIntPoint, dPointL1, dPointL2, dIntL1, dIntL2, thetaXL2, thetaPL1, thetaPL2, startLineLength, anchorX, anchorY, dAnchorL1, dAnchorL2, dAnchorInt, thetaAnchorL1, thetaAnchorL2, thetaNS;
    bool pointIsAboveSide;

public:

    std::vector<float> outermostPoints; // top right bottom left

    Shape ();
    Shape (std::vector<Point> points, const bool &fixed = false);

    std::vector<Point>& getPoints ();

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer);

    void handleCollisions (Shape &otherShape);
    float pointToLineDistance (Point &lp1, Point &lp2, Point &point, const bool &movePoint = false);
};