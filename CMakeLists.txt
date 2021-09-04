cmake_minimum_required(VERSION 3.19)
project(Raytracing_Raymarching_Hybrid)

set(CMAKE_CXX_STANDARD 14)
set(glm_DIR ${CMAKE_SOURCE_DIR}/lib/cmake/glm)
find_package(glm REQUIRED)

add_executable(Raytracing_Raymarching_Hybrid main.cpp Raytracing.cpp Raytracing.h Pathtracing.cpp Pathtracing.h Raymarching.cpp Raymarching.h Raymarching_primitives.h fast_square_root.h Renderer.cpp Renderer.h)
target_link_libraries(Raytracing_Raymarching_Hybrid glm::glm)