#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Spring.hpp"
#include "Point.hpp"

struct Shape {

    std::vector<Point> points;
    std::vector<Spring> springs;
    bool fixed = false;

    // declaring vars for collision handling to save memory
    float slope, xInterception, yInterception, centerX, centerY, dPointCenter, dIntPoint, dPointL1, dPointL2, dIntL1, dIntL2, thetaXL2, thetaPL1, thetaPL2, startLineLength, anchorX, anchorY, dAnchorL1, dAnchorL2, dAnchorInt, thetaAnchorL1, thetaAnchorL2, thetaNS, lineVelocityX, lineVelocityY, intersectX;
    bool pointIsAboveSide;

public:

    std::vector<float> outermostPoints; // top right bottom left

    Shape (std::vector<Point> points, const bool &fixed = false); // if springs is empty, each point will have a spring between each other point

    std::vector<Point>& getPoints ();

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer);

    void handleCollisions (Shape &otherShape, const float &ELASTICITY);
    float pointToLineDistance (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY, const bool &movePoint = false);

};