// TO RUN: g++ src/main.cpp src/Point.cpp src/Spring.cpp src/Shape.cpp src/SoftBodyPhysics2D.cpp -std=c++11 `pkg-config --libs --cflags sdl3` -march=x86-64 -mtune=generic; ./a.out

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include "../include/Point.hpp"
#include "../include/Spring.hpp"
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
    SoftBodyPhysics2D engine(-200, 1);

    // rectangle ground
    engine.addShape(std::vector<Point>{
        Point(100, HEIGHT-100, 5),
        Point(100, HEIGHT-200, 5),
        Point(WIDTH-100, HEIGHT-200, 5),
        Point(WIDTH-100, HEIGHT-100, 5),
    }, true);

    engine.addShape(std::vector<Point>{
        Point(25, HEIGHT-100, 5),
        Point(25, HEIGHT-350, 5),
        Point(100, HEIGHT-200, 5),
        Point(100, HEIGHT-100, 5)
    }, true);
    engine.addShape(std::vector<Point>{
        Point(WIDTH-25, HEIGHT-100, 5),
        Point(WIDTH-25, HEIGHT-350, 5),
        Point(WIDTH-100, HEIGHT-200, 5),
        Point(WIDTH-100, HEIGHT-100, 5)
    }, true);

    // Parallelogram
    // engine.addShape(std::vector<Point>{
    //     Point(50, HEIGHT/2-100),
    //     Point(50, HEIGHT/2+50-100),
    //     Point(250, HEIGHT/2+100-100),
    //     Point(250, HEIGHT/2+50-100)
    // }, true);

    // for (int i = 150; i < WIDTH-200; i += 125) {
    //     for (int j = 50; j < HEIGHT-275; j += 125) {
    //         engine.addRect(i, j, rand()%41+10, rand()%41+10);
    //     }
    // }

    engine.addRect(160, 150, 30, 30, 5, true);
    engine.addRect(400, 300, 30, 30, 5, true);
    
    // engine.addLinkedShape(
    //     std::vector<std::vector<Point>>{
    //         std::vector<Point>{
    //             Point(150, 50),
    //             Point(200, 50),
    //             Point(200, 100),
    //             Point(150, 100)
    //         },
    //         std::vector<Point>{
    //             Point(225, 50),
    //             Point(275, 50),
    //             Point(275, 100),
    //             Point(225, 100)
    //         }
    //     },
    //     std::vector<std::vector<std::pair<int, int>>>{
    //         std::vector<std::pair<int, int>>{{0, 1}, {1, 0}},
    //         std::vector<std::pair<int, int>>{{0, 2}, {1, 3}},
    //     },
    //     {false, false}
    // );

    engine.addShape(Shape(
        std::vector<Point>{
            Point(150, 50),
            Point(200, 50),
            Point(250, 50),
            Point(300, 50),
            Point(300, 100),
            Point(250, 100),
            Point(200, 100),
            Point(150, 100)
        },
        std::vector<std::pair<int, int>>{
            {0, 1},
            {1, 2},
            {2, 3},
            {3, 4},
            {4, 5},
            {5, 6},
            {6, 7},
            {0, 7},
            {0, 6},
            {1, 7},
            {1, 6},
            {1, 5},
            {2, 6},
            {2, 5},
            {2, 4},
            {3, 5},
        }
    ));

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
        engine.render(renderer);

        // Render frame
        SDL_RenderPresent(renderer);

        // Calculate dt
        dt = std::min((float) (SDL_GetTicks() - startTime)/1000, 0.05f); // clamp dt to 0.05s to prevent integration errors
        // std::cout << dt << "\n";
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}