cmake_minimum_required(VERSION 3.22)

project(integral)

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)


set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(GTest REQUIRED)
enable_testing()

add_executable(integral ${CMAKE_CURRENT_SOURCE_DIR}/sort.cc)

# add_executable(integral_test ${CMAKE_CURRENT_SOURCE_DIR}/integral_test.cc)
# target_link_libraries(integral_test PRIVATE ${GTEST_LIBRARIES})


target_include_directories(integral PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
