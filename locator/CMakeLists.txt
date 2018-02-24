cmake_minimum_required(VERSION 2.8)
project(locator)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)

include_directories(${PARENT_DIR}/dynamic_array/include)
include_directories(${PARENT_DIR}/bit_array/src)
include_directories(${PARENT_DIR}/utilities/src)

file(GLOB SOURCES "src/*.hpp" "src/*.cpp")
file(GLOB TEST_SOURCES "test/main.cpp")

add_library(locator STATIC ${SOURCES})
add_executable(locator-test ${TEST_SOURCES})

target_link_libraries(locator-test locator)

install(TARGETS locator DESTINATION ${CMAKE_SOURCE_DIR}/lib)
install(TARGETS locator-test DESTINATION ${CMAKE_SOURCE_DIR}/bin)