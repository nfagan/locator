cmake_minimum_required(VERSION 2.8)
project(locator)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp" "src/*.hpp" "src/*.h")

add_library(locator STATIC ${SOURCES})

install(TARGETS locator DESTINATION ${CMAKE_SOURCE_DIR}/lib)