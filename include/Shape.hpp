#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include "Spring.hpp"
#include "Point.hpp"

using namespace std;

class Shape {

    vector<Point> points;
    vector<Spring> springs;
    bool fixed = false;

    // declaring vars for collision handling to save memory
    float slope, xInterception, yInterception, intersectX, lineVelocityX, lineVelocityY, intVecX, intVecY, pseudoPointX, pseudoPointY, projectionFactor, wPoint, wL1, wL2, weightTotal, normX, normY, normLen, relVX, relVY, relVN, impulseMagnitude;

public:

    vector<float> outermostPoints; // top right bottom left

    Shape (vector<Point> points, const bool &fixed = false);
    // Shape (vector<Point>* points, const bool &fixed = false);
    Shape (vector<Point> points, const vector<pair<int, int>> &springPairs, const bool &fixed = false);

    vector<Point>& getPoints ();
    Point& getPoint (const int &index);

    void update (const float &dt, const float xForce, const float yForce, const float gravity);
    void render (SDL_Renderer* &renderer);

    void handleCollisions (Shape &otherShape, const float &ELASTICITY);
    float pointToLineDistance (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY);
    void resolveVelocities (Point &l1, Point &l2, Point &point, const float &ELASTICITY);
    void handleCircleToCircleCollision (Point &a, Point &b, const float &ELASTICITY);
    Point determineVirtualPoint(Point& c, Point& s1, Point& s2);

};