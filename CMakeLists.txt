cmake_minimum_required(VERSION 3.9)
project(slowsync)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/network)

include_directories(PUBLIC lib/)

add_executable(ssh_pingpong app/ssh_pingpong_test.cpp)
target_link_libraries(ssh_pingpong network)

add_executable(server app/server.cpp)