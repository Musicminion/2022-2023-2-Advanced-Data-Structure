cmake_minimum_required(VERSION 3.9)
project (treap_app)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(executable test.cc)

option(TEST_NUM "Test number" OFF)
if(TEST_NUM)
    add_definitions(-DTEST_NUM=${TEST_NUM})
endif()
