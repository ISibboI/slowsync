cmake_minimum_required(VERSION 3.9)
project(slowsync)

find_package(Boost 1.62.0 REQUIRED COMPONENTS filesystem)

file(GLOB_RECURSE LIB_SRC *.cpp)

add_library(ftree ${LIB_SRC})
target_link_libraries(ftree boost_filesystem boost_system network)
include_directories(..)