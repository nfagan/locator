cmake_minimum_required(VERSION 2.8)
project(dynamic_array)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

include_directories(${CMAKE_SOURCE_DIR}/include/)

file(GLOB SOURCES "test/main.cpp" "include/*.hpp")

add_executable(dynamic_array ${SOURCES})

install(TARGETS dynamic_array DESTINATION ${CMAKE_SOURCE_DIR}/bin)