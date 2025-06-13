#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/SoftBodyPhysics2D.hpp"

SoftBodyPhysics2D::SoftBodyPhysics2D () : shapes({}), GRAVITY(1), ELASTICITY(1) {}
SoftBodyPhysics2D::SoftBodyPhysics2D (const float &GRAVITY) : shapes({}), GRAVITY(GRAVITY), ELASTICITY(1) {}
SoftBodyPhysics2D::SoftBodyPhysics2D (const float &GRAVITY, const float &ELASTICITY) : shapes({}), GRAVITY(GRAVITY), ELASTICITY(ELASTICITY) {}

std::vector<Shape>& SoftBodyPhysics2D::getShapes () {
    return shapes;
}

void SoftBodyPhysics2D::update (const float &dt) {
    for (Shape &shape : shapes) {
        shape.update(dt, 0, 0, GRAVITY);
    }
    handleCollisions();
}
void SoftBodyPhysics2D::handleCollisions() {
    std::vector<bool> collisions;
    for (int i = 0; i < shapes.size(); i++) {
        for (int j = 0; j < shapes.size(); j++) {
            if (i != j) {
                shapes[i].handleCollisions(shapes[j]);
            }
        }
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
void SoftBodyPhysics2D::addShape (std::vector<Point> points, const bool &fixed) {
    shapes.emplace_back(Shape(std::move(points), fixed));
}