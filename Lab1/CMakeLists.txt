cmake_minimum_required(VERSION 3.16)
project(Lab1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


find_library(lb_location NAMES libminirt.a)
message(STATUS ${lb_location})
set(CMAKE_CXX_STANDARD 14)

add_library(zmq STATIC IMPORTED)
add_executable(Lab1 minirt_test_openmp.cpp)

TARGET_LINK_LIBRARIES(Lab1 PUBLIC minirt)
