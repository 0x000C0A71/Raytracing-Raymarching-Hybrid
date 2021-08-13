cmake_minimum_required(VERSION 3.19)
project(Raytracing_Raymarching_Hybrid)

set(CMAKE_CXX_STANDARD 14)

add_executable(Raytracing_Raymarching_Hybrid main.cpp Raytracing.cpp Raytracing.h Pathtracing.cpp Pathtracing.h)