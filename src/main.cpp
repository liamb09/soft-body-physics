// TO RUN: g++ src/main.cpp src/Point.cpp src/Shape.cpp src/SoftBodyPhysics2D.cpp -std=c++11 `pkg-config --libs --cflags sdl3`; ./a.out

#include <iostream>
#include <vector>
#include <utility>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include "../include/Point.hpp"
#include "../include/Shape.hpp"
#include "../include/SoftBodyPhysics2D.hpp"

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
    SoftBodyPhysics2D engine(.001);
    // square
    engine.addShape(std::vector<Point>{
        Point(100, 100, 5),
        Point(300, 100, 5),
        Point(300, 300, 5),
        Point(100, 300, 5),
    });
    // triangle
    engine.addShape(std::vector<Point>{
        Point(400, 200, 5),
        Point(400, 400, 5),
        Point(100, 400, 5),
    });

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
        // cout << dt << "\n";
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}