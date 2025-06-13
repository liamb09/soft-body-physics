// TO RUN: g++ src/main.cpp src/Point.cpp src/Shape.cpp src/SoftBodyPhysics2D.cpp -std=c++11 `pkg-config --libs --cflags sdl3`; ./a.out

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include "../include/Point.hpp"
#include "../include/Shape.hpp"
#include "../include/SoftBodyPhysics2D.hpp"

void calculateNewPointPositions (Point &lp1, Point &lp2, Point &point) {
    Point &l1 = lp1.x < lp2.x ? lp1 : lp2;
    Point &l2 = lp1.x < lp2.x ? lp2 : lp1;

    const float slope = (l2.y - l1.y)/(l2.x - l1.x);
    const float xInterception = (point.y*slope + point.x - l1.y*slope + l1.x*slope*slope)/(slope*slope + 1);
    const float yInterception = l1.y + slope*(xInterception - l1.x);

    const bool pointIsAboveSide = l1.y + slope*(point.x - l1.x) < point.y;

    const float centerX = (l1.m*l1.x + l2.m*l2.x + point.m*point.x)/(l1.m + l2.m + point.m);
    const float centerY = (l1.m*l1.y + l2.m*l2.y + point.m*point.y)/(l1.m + l2.m + point.m);

    const float dPointCenter = sqrt(pow(point.x - centerX, 2) + pow(point.y - centerY, 2)); // distance from point to center
    const float dIntPoint = sqrt(pow(xInterception - point.x, 2) + pow(yInterception - point.y, 2)); // distance from point to interception
    const float dPointL1 = sqrt(pow(point.x - l1.x, 2) + pow(point.y - l1.y, 2)); // distance from point to L1
    const float dPointL2 = sqrt(pow(point.x - l2.x, 2) + pow(point.y - l2.y, 2)); // distance from point to L2
    const float dIntL1 = sqrt(pow(xInterception - l1.x, 2) + pow(yInterception - l1.y, 2)); // distance from interception to lp1
    const float dIntL2 = sqrt(pow(xInterception - l2.x, 2) + pow(yInterception - l2.y, 2)); // distance from interception to lp2

    const float thetaXL2 = atan((l2.y - point.y)/(l2.x - point.x)); // angle between horizontal line going through point (the x-axis kinda) and PL2
    const float thetaPL1 = acos(dIntPoint/dPointL1); // angle between PI and PL2
    const float thetaPL2 = acos(dIntPoint/dPointL2) * (pointIsAboveSide ? -1 : 1); // angle between PI and PL2, negative if P is above side

    const float startLineLength = dPointCenter * (
        ((centerX - point.x)/dPointCenter) * (xInterception - point.x)/dIntPoint +
        ((centerY - point.y)/dPointCenter) * (yInterception - point.y)/dIntPoint
    );

    const float anchorX = point.x + startLineLength*cos(thetaPL2 + thetaXL2);
    const float anchorY = point.y + startLineLength*sin(thetaPL2 + thetaXL2);

    const float dAnchorL1 = sqrt(pow(anchorX - l1.x, 2) + pow(anchorY - l1.y, 2)); // distance from anchor to L1
    const float dAnchorL2 = sqrt(pow(anchorX - l2.x, 2) + pow(anchorY - l2.y, 2)); // distance from anchor to L2
    const float dAnchorInt = sqrt(pow(anchorX - xInterception, 2) + pow(anchorY - yInterception, 2)); // distance from anchor to interception
    const float thetaAnchorL1 = asin(dAnchorInt/dAnchorL1); // Angle between AL1 and line parallel to the side at A
    const float thetaAnchorL2 = asin(dAnchorInt/dAnchorL2); // Angle between AL2 and line parallel to the side at A

    const float thetaNS = 0.5 * (thetaAnchorL2*l2.m - thetaAnchorL1*l1.m)/(l1.m + l2.m) * (pointIsAboveSide ? -1 : 1); // angle relative to the side of the new side

    l1.x = dIntL1 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorX;
    l1.y = dIntL1 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? M_PI : 0)) + anchorY;
    l2.x = dIntL2 * cos(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorX;
    l2.y = dIntL2 * sin(thetaXL2 + thetaPL2 + M_PI_2 + thetaNS + (pointIsAboveSide ? 0 : M_PI)) + anchorY;
    point.x = anchorX;
    point.y = anchorY;
}

int main(int argc, char* argv[]) {

    // Setup window
    SDL_Window *window;
    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);

    const float WIDTH = 800;
    const float HEIGHT = 600;

    window = SDL_CreateWindow(
        "Soft Body Physics",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // Setup renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize simulation
    SoftBodyPhysics2D engine(.001, 1);
    // square
    engine.addShape(std::vector<Point>{
        Point(150, 50, 5),
        Point(350, 50, 5),
        Point(350, 150, 10),
        Point(150, 150, 5),
    });
    // // square2
    // engine.addShape(std::vector<Point>{
    //     Point(425, 150, 15),
    //     Point(625, 150, 15),
    //     Point(625, 250, 15),
    //     Point(425, 250, 15),
    // }, true);
    // triangle
    engine.addShape(std::vector<Point>{
        Point(250, 300, 5),
        Point(400, 400, 5),
        Point(100, 400, 5),
    }, true);

    Uint64 startTime;
    float dt;
    SDL_Event event;
    while (!done) {
        startTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    done = true;
                    break;
            }
        }

        // Set background coor
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Update engine
        engine.update(dt);

        // Render content
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        engine.render(renderer);

        // Render frame
        SDL_RenderPresent(renderer);

        // Calculate dt
        dt = (float) (SDL_GetTicks() - startTime);
        // std::cout << dt << "\n";
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}