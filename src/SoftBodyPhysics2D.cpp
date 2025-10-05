#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include "../include/SoftBodyPhysics2D.hpp"

using namespace std;

SoftBodyPhysics2D::SoftBodyPhysics2D () : shapes({}), GRAVITY(1), ELASTICITY(1) {}
SoftBodyPhysics2D::SoftBodyPhysics2D (const float &GRAVITY) : shapes({}), GRAVITY(GRAVITY), ELASTICITY(1) {}
SoftBodyPhysics2D::SoftBodyPhysics2D (const float &GRAVITY, const float &ELASTICITY) : shapes({}), GRAVITY(GRAVITY), ELASTICITY(ELASTICITY) {}

vector<Shape>& SoftBodyPhysics2D::getShapes () {
    return shapes;
}

void SoftBodyPhysics2D::update (const float &dt) {
    for (Shape &shape : shapes) {
        shape.update(dt, 0, 0, GRAVITY);
    }
    for (Spring &spring : springs) {
        spring.update(dt);
    }
    handleCollisions();
}
void SoftBodyPhysics2D::handleCollisions() {
    vector<bool> collisions;
    for (int i = 0; i < shapes.size(); i++) {
        for (int j = 0; j < shapes.size(); j++) {
            if (i != j) {
                shapes[i].handleCollisions(shapes[j], ELASTICITY);
            }
        }
    }
}
void SoftBodyPhysics2D::render (SDL_Renderer* &renderer) {
    for (Shape &shape : shapes) {
        shape.render(renderer);
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (Spring &spring : springs) {
        // cout << "rendering...\n";
        spring.render(renderer);
    }
}

void SoftBodyPhysics2D::addShape (Shape shape) {
    shapes.push_back(move(shape));
}
void SoftBodyPhysics2D::addShape (vector<Point> points, const bool &fixed) {
    shapes.emplace_back(Shape(move(points), fixed));
}
// void SoftBodyPhysics2D::addShape (vector<Point>* points, const bool &fixed) {
//     shapes.emplace_back(Shape(points, fixed));
// }

void SoftBodyPhysics2D::addRect (const float &x, const float &y, const float &width, const float &height, const float &m, const bool &fixed) {
    addShape(vector<Point>{
        Point(x, y, m),
        Point(x+width, y, m),
        Point(x+width, y+height, m),
        Point(x, y+height, m),
    }, fixed);
}

void SoftBodyPhysics2D::addLinkedShape (vector<vector<Point>> pointSets, vector<vector<pair<int, int>>> links, const vector<bool> &fixed) {
    for (int i = 0; i < pointSets.size(); i++) {
        addShape(pointSets[i], fixed[i]);
    }
    for (const vector<pair<int, int>> &link : links) {
        addLink(
            -(pointSets.size() - link[0].first), link[0].second,
            -(pointSets.size() - link[1].first), link[1].second
        );
    }
}
void SoftBodyPhysics2D::addLink (int iShape1, int iPoint1, int iShape2, int iPoint2) {
    // account for negative shape indices
    iShape1 = (iShape1 + shapes.size())%shapes.size();
    iShape2 = (iShape2 + shapes.size())%shapes.size();
    springs.emplace_back(&(shapes[iShape1].getPoint(iPoint1)), &(shapes[iShape2].getPoint(iPoint2)), 750, 30);
}

// void SoftBodyPhysics2D::addGridShape (const float &x, const float &y, const float &width, const float &height, const float &numRows, const float &numColumns, const float &m, const bool &fixed) {
//     vector<Point> newPoints = {};
//     for (int i = x; i < x+width; i += width/numColumns) {
//         cout << i << " " << y << "\n";
//         newPoints.push_back(Point(i, y, m, fixed));
//     }
//     for (int i = y; i < y+height; i += height/numRows) {
//         cout << x+width << " " << i << "\n";
//         newPoints.push_back(Point(x+width, i, m, fixed));
//     }
//     for (int i = x+width; i > x; i -= width/numColumns) {
//         cout << i << " " << y+height << "\n";
//         newPoints.push_back(Point(i, y, m, fixed));
//     }
//     for (int i = y+height; i > y; i -= height/numRows) {
//         cout << x << " " << i << "\n";
//         newPoints.push_back(Point(x+width, i, m, fixed));
//     }
//     cout << newPoints.size() << "\n";
//     addShape(newPoints, fixed);
// }