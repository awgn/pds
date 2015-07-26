cmake_minimum_required(VERSION 2.4)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -std=c++14")

include_directories(hdr .)

add_executable(test-loglog test/loglog.cpp)
add_executable(plot-loglog test/plot-loglog.cpp)
add_executable(test-sketch test/sketch.cpp)
