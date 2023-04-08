cmake_minimum_required(VERSION 3.20)
project(Lab1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(CMAKE_CXX_STANDARD 14)

add_executable(Lab1 methodical_manual_task_2ex.cpp)