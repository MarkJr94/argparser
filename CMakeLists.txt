cmake_minimum_required(VERSION 2.6)
project(ARGPARSER)

set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

add_executable(OptParser_test OptParser_test.cpp OptParser.cpp)

add_library(optparser SHARED OptParser.cpp)