cmake_minimum_required(VERSION 3.15)

project(breast_test )

add_executable(server server.cpp )

set(links pthread)
target_link_libraries( server  ${links})

