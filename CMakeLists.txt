cmake_minimum_required(VERSION 2.8.4)
project(IESparser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp IES.cpp IES.h database.h)
add_executable(IESparser ${SOURCE_FILES})