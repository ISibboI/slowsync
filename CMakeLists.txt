cmake_minimum_required(VERSION 3.9)
project(slowsync)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/network)
add_subdirectory(lib/ftree)

include_directories(PUBLIC lib/)

add_executable(ssh_pingpong_test app/ssh_pingpong_test.cpp)
target_link_libraries(ssh_pingpong_test network)

add_executable(filesystem_test app/filesystem_test.cpp)
target_link_libraries(filesystem_test ftree)

add_executable(server app/server.cpp)
target_link_libraries(server ftree)