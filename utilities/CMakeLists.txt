cmake_minimum_required(VERSION 2.8)
project(utilities)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)

file(GLOB SOURCES "src/*.hpp" "src/*.cpp")
file(GLOB TEST_SOURCES "test/main.cpp")

add_executable(utilities-test ${TEST_SOURCES} ${SOURCES})
install(TARGETS utilities-test DESTINATION ${CMAKE_SOURCE_DIR}/bin)