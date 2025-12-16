cmake_minimum_required(VERSION 3.10)

project(SoftBodyPhysics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED)

add_executable(main 
    src/main.cpp
    src/Point.cpp
    src/Shape.cpp
    src/SoftBodyPhysics2D.cpp
    src/Spring.cpp
    src/Vector2D.cpp
)

target_link_libraries(main PRIVATE SDL3::SDL3)