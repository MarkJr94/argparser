cmake_minimum_required(VERSION 2.6)
project(ARGPARSER)

set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

#add_executable(Argument_test Argument_test.cpp Argument.cpp)

add_executable(ArgumentParser_test ArgumentParser_test.cpp ArgumentParser.cpp)

add_library(argparse SHARED ArgumentParser.cpp)