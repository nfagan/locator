cmake_minimum_required(VERSION 2.8)
project(locator)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB SOURCES "src/*.c" "src/*.cpp" "src/*.hpp" "src/*.h")

add_library(locator STATIC ${SOURCES})

add_executable(bit_array-test "test/bit_array.cpp")
add_executable(dynamic_array-test "test/dynamic_array.cpp")
add_executable(utilities-test "test/utilities.cpp")
add_executable(locator-test "test/locator.cpp")

target_link_libraries(bit_array-test locator)
target_link_libraries(dynamic_array-test locator)
target_link_libraries(utilities-test locator)
target_link_libraries(locator-test locator)

install(TARGETS locator DESTINATION ${CMAKE_SOURCE_DIR}/lib)