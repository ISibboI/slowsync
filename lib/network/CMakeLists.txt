cmake_minimum_required(VERSION 3.9)
project(slowsync)

file(GLOB_RECURSE LIB_SRC *.cpp)

add_library(network ${LIB_SRC})
include_directories(..)