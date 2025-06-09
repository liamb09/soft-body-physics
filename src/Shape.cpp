#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/Shape.hpp"

Shape::Shape () : points({}) {}
Shape::Shape (std::vector<Point> points, const bool &fixed) : points(std::move(points)), fixed(fixed) {
    if (fixed) {
        for (Point &p : this->points) {
            p.fixed = true;
        }
    }
}

std::vector<Point>& Shape::getPoints () {
    return points;
}

void Shape::update (const float &dt, const float xForce, const float yForce, const float gravity) {
    outermostPoints = {100000, -100000, -100000, 100000};
    for (Point &point : points) {
        if (!fixed) point.update(dt, xForce, yForce, gravity);
        outermostPoints[0] = std::min(outermostPoints[0], point.y);
        outermostPoints[1] = std::max(outermostPoints[1], point.x);
        outermostPoints[2] = std::max(outermostPoints[2], point.y);
        outermostPoints[3] = std::min(outermostPoints[3], point.x);
    }
}

void Shape::render (SDL_Renderer* &renderer) {
    for (int i = 0; i < points.size(); i++) {
        points[i].render(renderer);
        SDL_RenderLine(renderer, points[i].x, points[i].y, points[(i+1)%points.size()].x, points[(i+1)%points.size()].y);
    }
    // test renders diagonal of container
    SDL_RenderLine(renderer, outermostPoints[3], outermostPoints[0], outermostPoints[1], outermostPoints[2]);
}
void Shape::handleCollisions(Shape &otherShape) {
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
    std::vector<Point> &otherPoints = otherShape.getPoints();
    int numIntersects;
    float currentDist;
    float closestSideDist = 100000;
    int closestSideNum = 0;
    for (Point &otherPoint : otherPoints) {
        // std::cout << otherPoint.x << "  " << otherPoint.y << "\n";
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
        // std::cout << otherPoint.x << "  " << otherPoint.y << "\n";

        // Find closest side of this shape to otherPoint if otherPoint is in this shape
        for (int i = 0; i < points.size(); i++) {
            currentDist = pointToLineDistance(points[i], points[(i+1)%points.size()], otherPoint);
            if (currentDist < closestSideDist) {
                closestSideDist = currentDist;
                closestSideNum = i;
            }
        }
        // std::cout << "The closest side to (" << otherPoint.x << ", " << otherPoint.y << ") is side #" << closestSideNum << " from (" << points[closestSideNum].x << ", " << points[closestSideNum].y << ") to (" << points[(closestSideNum+1)%points.size()].x << ", " << points[(closestSideNum+1)%points.size()].y << ")\n";
        pointToLineDistance(points[closestSideNum], points[(closestSideNum+1)%points.size()], otherPoint, true);
    }
}
float Shape::pointToLineDistance (Point &lp1, Point &lp2, Point &point, const bool &movePoint) { // lp = line point
    // change l1 to leftmost point and l2 to rightmost because that's how I did the math
    Point &l1 = lp1.x < lp2.x ? lp1 : lp2;
    Point &l2 = lp1.x < lp2.x ? lp2 : lp1;

    // TODO: Update point velocities after handling collision

    // Calculates the intersect of the given line and the line with the slope perpendicular to the given line going through the given point.
    // Then, calculates the distance between that intersection and the given point.
    slope = (l2.y - l1.y)/(l2.x - l1.x);
    xInterception = (point.y*slope + point.x - l1.y*slope + l1.x*slope*slope)/(slope*slope + 1);
    yInterception = l1.y + slope*(xInterception - l1.x);
    if (!movePoint) return sqrt(pow(xInterception - point.x, 2) + pow(yInterception - point.y, 2));

    // Move point to side edge (if both shapes are fixed, they shouldn't be intersecting--it's an error when they were entered into the engine)

    // If only the side is fixed, just move point to the side
    if (l1.fixed && l2.fixed && !point.fixed) {
        point.x = xInterception;
        point.y = yInterception;
        return -1;
    }

    // If only the point is fixed, move the side to the edge of the point
    if (!l1.fixed && !l2.fixed && point.fixed) {
        lineVelocityX = (l1.vx + l2.vx)/2;
        lineVelocityY = (l1.vy + l2.vy)/2;
        l1.x -= lineVelocityX;
        l2.x -= lineVelocityX;
        l1.y -= lineVelocityY;
        l2.y -= lineVelocityY;
        l1.vx = 0;
        l1.vy = 0;
        l2.vx = 0;
        l2.vy = 0;
        return -1;
    }

    // If both the side and the points are fixed, do the math to move them, taking into account their mass
    const bool pointIsAboveSide = l1.y + slope*(point.x - l1.x) < point.y;

    centerX = (l1.m*l1.x + l2.m*l2.x + point.m*point.x)/(l1.m + l2.m + point.m);
    centerY = (l1.m*l1.y + l2.m*l2.y + point.m*point.y)/(l1.m + l2.m + point.m);

    dPointCenter = sqrt(pow(point.x - centerX, 2) + pow(point.y - centerY, 2)); // distance from point to center
    dIntPoint = sqrt(pow(xInterception - point.x, 2) + pow(yInterception - point.y, 2)); // distance from point to interception
    dPointL1 = sqrt(pow(point.x - l1.x, 2) + pow(point.y - l1.y, 2)); // distance from point to L1
    dPointL2 = sqrt(pow(point.x - l2.x, 2) + pow(point.y - l2.y, 2)); // distance from point to L2
    dIntL1 = sqrt(pow(xInterception - l1.x, 2) + pow(yInterception - l1.y, 2)); // distance from interception to lp1
    dIntL2 = sqrt(pow(xInterception - l2.x, 2) + pow(yInterception - l2.y, 2)); // distance from interception to lp2

    thetaXL2 = atan((l2.y - point.y)/(l2.x - point.x)); // angle between horizontal line going through point (the x-axis kinda) and PL2
    thetaPL1 = acos(dIntPoint/dPointL1); // angle between PI and PL2
    thetaPL2 = acos(dIntPoint/dPointL2) * (pointIsAboveSide ? -1 : 1); // angle between PI and PL2, negative if P is above side

    startLineLength = dPointCenter * (
        ((centerX - point.x)/dPointCenter) * (xInterception - point.x)/dIntPoint +
        ((centerY - point.y)/dPointCenter) * (yInterception - point.y)/dIntPoint
    );

    anchorX = point.x + startLineLength*cos(thetaPL2 + thetaXL2);
    anchorY = point.y + startLineLength*sin(thetaPL2 + thetaXL2);

    dAnchorL1 = sqrt(pow(anchorX - l1.x, 2) + pow(anchorY - l1.y, 2)); // distance from anchor to L1
    dAnchorL2 = sqrt(pow(anchorX - l2.x, 2) + pow(anchorY - l2.y, 2)); // distance from anchor to L2
    dAnchorInt = sqrt(pow(anchorX - xInterception, 2) + pow(anchorY - yInterception, 2)); // distance from anchor to interception
    thetaAnchorL1 = asin(dAnchorInt/dAnchorL1); // Angle between AL1 and line parallel to the side at A
    thetaAnchorL2 = asin(dAnchorInt/dAnchorL2); // Angle between AL2 and line parallel to the side at A

    thetaNS = 0.5 * (thetaAnchorL2*l2.m - thetaAnchorL1*l1.m)/(l1.m + l2.m) * (pointIsAboveSide ? -1 : 1); // angle relative to the side of the new side

    l1.x = dIntL1 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorX;
    l1.y = dIntL1 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorY;
    l2.x = dIntL2 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorX;
    l2.y = dIntL2 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorY;
    point.x = anchorX;
    point.y = anchorY;
    return -1;
}