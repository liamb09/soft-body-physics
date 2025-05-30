#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/Shape.hpp"

Shape::Shape () : points({}) {}
Shape::Shape (std::vector<Point> points) : points(std::move(points)) {}

std::vector<Point>& Shape::getPoints () {
    return points;
}

void Shape::update (const float &dt, const float xForce, const float yForce) {
    for (Point &point : points) {
        point.update(dt, xForce, yForce);
    }
}

void Shape::render (SDL_Renderer* &renderer) {
    for (int i = 0; i < points.size(); i++) {
        points[i].render(renderer);
        SDL_RenderLine(renderer, points[i].x, points[i].y, points[(i+1)%points.size()].x, points[(i+1)%points.size()].y);
    }
}

// bool pointInShape(const Point &p) {

// }