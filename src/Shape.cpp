#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/Spring.hpp"
#include "../include/Shape.hpp"

using namespace std;

Shape::Shape (vector<Point> points, const bool &fixed) : points(std::move(points)), fixed(fixed) {
    if (!fixed) {
        // add springs
        for (int i = 0; i < this->points.size()-1; i++) {
            for (int j = i+1; j < this->points.size(); j++) {
                springs.emplace_back(&(this->points[i]), &(this->points[j]), 750, 30);
            }
        }
    } else {
        // fix each point
        for (Point &p : this->points) {
            p.fixed = true;
        }
    }
}
// Shape::Shape (vector<Point>* points, const bool &fixed) : Shape(*points, fixed) {}
Shape::Shape (vector<Point> points, const vector<pair<int, int>> &springPairs, const bool &fixed) : points(std::move(points)), fixed(fixed) {
    if (!fixed) {
        // add springs
        for (const pair<int, int> &pair : springPairs) {
            springs.emplace_back(&(this->points[pair.first]), &(this->points[pair.second]), 750, 30);
        }
    } else {
        // fix each point
        for (Point &p : this->points) {
            p.fixed = true;
        }
    }
}

vector<Point>& Shape::getPoints () {
    return points;
}
Point& Shape::getPoint (const int &index) {
    return points[index];
}

void Shape::update (const float &dt, const float xForce, const float yForce, const float gravity) {
    for (Spring &spring : springs) {
        spring.update(dt);
    }
    outermostPoints = {100000, -100000, -100000, 100000};
    for (Point &point : points) {
        if (!fixed) point.update(dt, xForce, yForce, gravity);
        outermostPoints[0] = min(outermostPoints[0], point.y);
        outermostPoints[1] = max(outermostPoints[1], point.x);
        outermostPoints[2] = max(outermostPoints[2], point.y);
        outermostPoints[3] = min(outermostPoints[3], point.x);
    }
}

void Shape::render (SDL_Renderer* &renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (Spring &spring : springs) {
        spring.render(renderer);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < points.size(); i++) {
        points[i].render(renderer);
        SDL_RenderLine(renderer, points[i].x, points[i].y, points[(i+1)%points.size()].x, points[(i+1)%points.size()].y);
    }
    // test renders diagonal of container
    // SDL_RenderLine(renderer, outermostPoints[3], outermostPoints[0], outermostPoints[1], outermostPoints[2]);
}
void Shape::handleCollisions(Shape &otherShape, const float &ELASTICITY) {
    // If the other shape's container (the smallest rectangle that fits around it) is
    //   outside of this shape's, then skip all the extra steps
    // Checks if the shapes do intersect first and flips the bool
    if (!(
        (outermostPoints[3] < otherShape.outermostPoints[3] && otherShape.outermostPoints[3] < outermostPoints[1] &&
            outermostPoints[0] < otherShape.outermostPoints[0] && otherShape.outermostPoints[0] < outermostPoints[2]) || // otherShape's topright point is in this shape
        (outermostPoints[3] < otherShape.outermostPoints[1] && otherShape.outermostPoints[1] < outermostPoints[1] &&
            outermostPoints[0] < otherShape.outermostPoints[2] && otherShape.outermostPoints[2] < outermostPoints[2]) || // otherShape's bottomleft point is in this shape
        (otherShape.outermostPoints[3] < outermostPoints[3] && outermostPoints[3] < otherShape.outermostPoints[1] &&
            otherShape.outermostPoints[0] < outermostPoints[0] && outermostPoints[0] < otherShape.outermostPoints[2]) || // this shape's topright point is in otherShape
        (otherShape.outermostPoints[3] < outermostPoints[1] && outermostPoints[1] < otherShape.outermostPoints[1] &&
            otherShape.outermostPoints[0] < outermostPoints[2] && outermostPoints[2] < otherShape.outermostPoints[2]) // this shape's bottomleft point is in othershape 
    )) return;

    // Checks which of the points from another shape is in this shape.
    // To do so, for each point in the other shape, it calculates how many times the
    //   line going right intersects with the border of the shape.
    // If this number of intersections is odd, the other point is in this shape.
    vector<Point> &otherPoints = otherShape.getPoints();
    int numIntersects;
    float currentDist;
    float closestSideDist = 100000;
    int closestSideNum = 0;
    for (Point &otherPoint : otherPoints) {
        // Test if otherPoint is in this shape
        numIntersects = 0;
        for (int i = 0; i < points.size(); i++) {
            // Calculate where current side segment would intersect with horizontal line
            intersectX = points[i].x + (otherPoint.y - points[i].y) * (points[(i+1)%points.size()].x - points[i].x)/(points[(i+1)%points.size()].y - points[i].y);
            // Test whether this intersection is within the domain of the horizontal line
            if (otherPoint.x < intersectX && intersectX < outermostPoints[1] + 10 && !((points[(i+1)%points.size()].y > otherPoint.y && points[i].y > otherPoint.y) || (points[(i+1)%points.size()].y < otherPoint.y && points[i].y < otherPoint.y))) {
                numIntersects++;
            }
        }
        if (numIntersects % 2 == 0) {
            continue;
        }

        // Find closest side of this shape to otherPoint if otherPoint is in this shape
        for (int i = 0; i < points.size(); i++) {
            currentDist = pointToLineDistance(points[i], points[(i+1)%points.size()], otherPoint, ELASTICITY);
            if (currentDist < closestSideDist) {
                closestSideDist = currentDist;
                closestSideNum = i;
            }
        }
        resolveVelocities(points[closestSideNum], points[(closestSideNum+1)%points.size()], otherPoint, ELASTICITY);
    }
}
float Shape::pointToLineDistance (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY) { // lp = line point
    // change l1 to leftmost point and l2 to rightmost because that's how I did the math
    Point &l1 = lp1.x < lp2.x ? lp1 : lp2;
    Point &l2 = lp1.x < lp2.x ? lp2 : lp1;

    // Calculates the intersect of the given line and the line with the slope perpendicular to the given line going through the given point.
    // Then, calculates the distance between that intersection and the given point.
    slope = (l2.y - l1.y)/(l2.x - l1.x);
    xInterception = (point.y*slope + point.x - l1.y*slope + l1.x*slope*slope)/(slope*slope + 1);
    yInterception = l1.y + slope*(xInterception - l1.x);
    return sqrt(pow(xInterception - point.x, 2) + pow(yInterception - point.y, 2));
}

void Shape::resolveVelocities (Point &lp1, Point &lp2, Point &point, const float &ELASTICITY) {
    // change l1 to leftmost point and l2 to rightmost because that's how I did the math
    Point &l1 = lp1.x < lp2.x ? lp1 : lp2;
    Point &l2 = lp1.x < lp2.x ? lp2 : lp1;

    // Calculates the intersect of the given line and the line with the slope perpendicular to the given line going through the given point.
    // Then, calculates the distance between that intersection and the given point.
    slope = (l2.y - l1.y)/(l2.x - l1.x);
    xInterception = (point.y*slope + point.x - l1.y*slope + l1.x*slope*slope)/(slope*slope + 1);
    yInterception = l1.y + slope*(xInterception - l1.x);

    // If both are fixed (ex: wall touches ground) then exit
    if (l1.fixed && l2.fixed && point.fixed) return;

    // If only the side is fixed, just move point to the side
    if (l1.fixed && l2.fixed && !point.fixed) {
        point.x = xInterception;
        point.y = yInterception;

        // collision normal, represented by the vector between l1 and l2 that has been rotated 90 degrees (normalized)
        float cnx = -(l2.y - l1.y);
        float cny = (l2.x - l1.x);
        cnx /= sqrt(cnx*cnx + cny*cny);
        cny /= sqrt(cnx*cnx + cny*cny);

        // old calculations: weird angle stuff
        // float thetaNormV = atan2(point.vy, point.vx) - atan2(cny, cnx); // angle between normal vector and velocity vector
        // float newVx = (point.vx*cos(M_PI-2*thetaNormV)*ELASTICITY - point.vy*sin(M_PI-2*thetaNormV)*ELASTICITY);
        // float newVy = point.vx*sin(M_PI-2*thetaNormV)*ELASTICITY + point.vy*cos(M_PI-2*thetaNormV*ELASTICITY);

        // new calculations: reflect point v over collsion normal
        float newVx = point.vx - (1 + ELASTICITY)*(point.vx*cnx + point.vy*cny)*cnx;
        float newVy = point.vy - (1 + ELASTICITY)*(point.vx*cnx + point.vy*cny)*cny;
        point.vx = newVx;
        point.vy = newVy;
        return;
    }

    // If only the point is fixed, move the side to the edge of the point
    lineVelocityX = (l1.vx + l2.vx)/2;
    lineVelocityY = (l1.vy + l2.vy)/2;
    if (!l1.fixed && !l2.fixed && point.fixed) {
        l1.x += point.x - xInterception;
        l2.x += point.x - xInterception;
        l1.y += point.y - yInterception;
        l2.y += point.y - yInterception;
        
        // collision normal, represented by the vector between l1 and l2 that has been rotated 90 degrees
        float cnx = -(l2.y - l1.y);
        float cny = (l2.x - l1.x);
        cnx /= sqrt(cnx*cnx + cny*cny);
        cny /= sqrt(cnx*cnx + cny*cny);
        float newVx = lineVelocityX - (1 + ELASTICITY)*(lineVelocityX*cnx + lineVelocityY*cny)*cnx;
        float newVy = lineVelocityY - (1 + ELASTICITY)*(lineVelocityX*cnx + lineVelocityY*cny)*cny;
        
        l1.vx = newVx;
        l1.vy = newVy;
        l2.vx = newVx;
        l2.vy = newVy;
        return;
    }

    // old math
    // If neither the side nor the points are fixed, do the math to move them, taking into account their mass
    // const bool pointIsAboveSide = l1.y + slope*(point.x - l1.x) < point.y;
    // centerX = (l1.m*l1.x + l2.m*l2.x + point.m*point.x)/(l1.m + l2.m + point.m);
    // centerY = (l1.m*l1.y + l2.m*l2.y + point.m*point.y)/(l1.m + l2.m + point.m);
    // dPointCenter = sqrt(pow(point.x - centerX, 2) + pow(point.y - centerY, 2)); // distance from point to center
    // dIntPoint = sqrt(pow(xInterception - point.x, 2) + pow(yInterception - point.y, 2)); // distance from point to interception
    // dPointL1 = sqrt(pow(point.x - l1.x, 2) + pow(point.y - l1.y, 2)); // distance from point to L1
    // dPointL2 = sqrt(pow(point.x - l2.x, 2) + pow(point.y - l2.y, 2)); // distance from point to L2
    // dIntL1 = sqrt(pow(xInterception - l1.x, 2) + pow(yInterception - l1.y, 2)); // distance from interception to lp1
    // dIntL2 = sqrt(pow(xInterception - l2.x, 2) + pow(yInterception - l2.y, 2)); // distance from interception to lp2
    // thetaXL2 = atan((l2.y - point.y)/(l2.x - point.x)); // angle between horizontal line going through point (the x-axis kinda) and PL2
    // thetaPL1 = acos(dIntPoint/dPointL1); // angle between PI and PL2
    // thetaPL2 = acos(dIntPoint/dPointL2) * (pointIsAboveSide ? -1 : 1); // angle between PI and PL2, negative if P is above side
    // startLineLength = dPointCenter * (
    //     ((centerX - point.x)/dPointCenter) * (xInterception - point.x)/dIntPoint +
    //     ((centerY - point.y)/dPointCenter) * (yInterception - point.y)/dIntPoint
    // );
    // anchorX = point.x + startLineLength*cos(thetaPL2 + thetaXL2);
    // anchorY = point.y + startLineLength*sin(thetaPL2 + thetaXL2);
    // dAnchorL1 = sqrt(pow(anchorX - l1.x, 2) + pow(anchorY - l1.y, 2)); // distance from anchor to L1
    // dAnchorL2 = sqrt(pow(anchorX - l2.x, 2) + pow(anchorY - l2.y, 2)); // distance from anchor to L2
    // dAnchorInt = sqrt(pow(anchorX - xInterception, 2) + pow(anchorY - yInterception, 2)); // distance from anchor to interception
    // thetaAnchorL1 = asin(dAnchorInt/dAnchorL1); // Angle between AL1 and line parallel to the side at A
    // thetaAnchorL2 = asin(dAnchorInt/dAnchorL2); // Angle between AL2 and line parallel to the side at A
    // thetaNS = 0.5 * (thetaAnchorL2*l2.m - thetaAnchorL1*l1.m)/(l1.m + l2.m) * (pointIsAboveSide ? -1 : 1); // angle relative to the side of the new side
    // l1.x = dIntL1 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorX;
    // l1.y = dIntL1 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorY;
    // l2.x = dIntL2 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorX;
    // l2.y = dIntL2 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorY;
    // point.x = anchorX;
    // point.y = anchorY;

    // new, better math w/ vectors

    // intersection vector
    intVecX = xInterception - point.x;
    intVecY = yInterception - point.y;

    // psuedoPointX is the imaginary point that this point collides with, and represents the side it is colliding with
    // for simplicity in the new velocity calculations, and because these points technically have no volume, the points will be assumed to have a diameter of 1
    pseudoPointX = point.x - intVecX/sqrt(pow(intVecX, 2) + pow(intVecY, 2));
    pseudoPointY = point.y - intVecY/sqrt(pow(intVecX, 2) + pow(intVecY, 2));

    // how close the intercept is to l1
    projectionFactor = sqrt(pow(l1.x - xInterception, 2) + pow(l1.y - yInterception, 2)) / sqrt(pow(l1.x - l2.x, 2) + pow(l1.y - l2.y, 2));

    // weights (how much each point should be moved), which are then normalized
    wPoint = 1/point.m;
    wL1 = (1 - projectionFactor)/l1.m;
    wL2 = projectionFactor/l2.m;
    weightTotal = wPoint + wL1 + wL2;
    wPoint /= weightTotal;
    wL1 /= weightTotal;
    wL2 /= weightTotal;

    // adjust point positions
    l1.x -= (xInterception-point.x)*wL1;
    l1.y -= (yInterception-point.y)*wL1;
    l2.x -= (xInterception-point.x)*wL2;
    l2.y -= (yInterception-point.y)*wL2;
    point.x += (xInterception-point.x)*wPoint;
    point.y += (yInterception-point.y)*wPoint;
    // if necessary, fix point to side here

    // adjust point velocities
    // normal vector
    normX = pseudoPointX - point.x;
    normY = pseudoPointY - point.y;
    normLen = sqrt(pow(normX, 2) + pow(normY, 2));
    normX /= normLen;
    normY /= normLen;

    // relative velocity, which represents the velocity of point relative to the line
    relVX = point.vx - lineVelocityX;
    relVY = point.vy - lineVelocityY;
    // relative velocity along normal
    relVN = relVX*normX + relVY*normY;

    // (J) magnitude of the impulse vector J (the second term of the denominator is 2/(...) because I'm taking the average mass of L1 and L2)
    impulseMagnitude = (1+ELASTICITY)*(relVN)/(1/point.m + 2/(l1.m + l2.m));
    point.vx -= impulseMagnitude/point.m*normX;
    point.vy -= impulseMagnitude/point.m*normY;
    l1.vx += impulseMagnitude/l1.m*normX;
    l1.vy += impulseMagnitude/l1.m*normY;
    l2.vx += impulseMagnitude/l2.m*normX;
    l2.vy += impulseMagnitude/l2.m*normY;
}