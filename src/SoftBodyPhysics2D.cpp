#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/SoftBodyPhysics2D.hpp"

SoftBodyPhysics2D::SoftBodyPhysics2D () : GRAVITY(1), shapes({}) {}
SoftBodyPhysics2D::SoftBodyPhysics2D (const float &GRAVITY) : shapes({}), GRAVITY(GRAVITY) {}

void SoftBodyPhysics2D::update (const float &dt) {
    for (Shape &shape : shapes) {
        shape.update(dt, 0, 0); //-GRAVITY);
    }
}

void SoftBodyPhysics2D::render (SDL_Renderer* &renderer) {
    for (Shape &shape : shapes) {
        shape.render(renderer);
    }
}

void SoftBodyPhysics2D::addShape (Shape &shape) {
    shapes.push_back(shape);
}
void SoftBodyPhysics2D::addShape (std::vector<Point> points) {
    shapes.emplace_back(std::move(points));
}