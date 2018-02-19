cmake_minimum_required(VERSION 2.8)
project(bit_array)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)
include_directories(${PARENT_DIR}/dynamic_array/include)

file(GLOB SOURCES "src/*.hpp" "src/*.cpp")
file(GLOB TEST_SOURCES "test/main.cpp")

add_library(bit_array STATIC ${SOURCES})
add_executable(bit_array-test ${TEST_SOURCES})

target_link_libraries(bit_array-test bit_array)

install(TARGETS bit_array DESTINATION ${CMAKE_SOURCE_DIR}/lib)
install(TARGETS bit_array-test DESTINATION ${CMAKE_SOURCE_DIR}/bin)