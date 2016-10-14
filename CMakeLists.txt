#  Copyright (c) 2016 Joon Ahn (green@weahns.net).
#  All rights reserved.

cmake_minimum_required(VERSION 2.6)
project(mesos-test)

find_path(CPPUTEST_INCLUDE_DIR CppUTest/CommandLineTestRunner.h)
find_library(CPPUTEST_LIB CppUTest)

# for extensive warning from compilers.
add_definitions(-Wall -g -ggdb)
add_definitions(-DJADEBUG=1)

set(MYLIB libmesos_test)

#include subdirectories
include_directories(${CPPUTEST_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
#include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
