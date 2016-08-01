cmake_minimum_required(VERSION 2.4)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g0 -Wall -Wextra -std=c++14")

include_directories(hdr .)

add_executable(test-loglog test/loglog.cpp)
add_executable(plot-loglog test/plot-loglog.cpp)
add_executable(test-sketch test/sketch.cpp)
add_executable(test-range  test/range.cpp)
add_executable(test-hash   test/hash.cpp)
add_executable(test-tuple  test/tuple.cpp)
add_executable(test-annotated  test/annotated.cpp)
add_executable(test-cartesian test/cartesian.cpp)
add_executable(test-reversible test/reversible.cpp)
