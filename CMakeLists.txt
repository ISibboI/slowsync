cmake_minimum_required(VERSION 3.9)
project(slowsync)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/network)
add_subdirectory(lib/ftree)

include_directories(PUBLIC lib/)

add_executable(ssh_pingpong app/ssh_pingpong.cpp)
target_link_libraries(ssh_pingpong network)

add_executable(filesystem_test app/filesystem_test.cpp)
target_link_libraries(filesystem_test ftree)