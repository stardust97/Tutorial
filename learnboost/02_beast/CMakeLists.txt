cmake_minimum_required(VERSION 3.15)

project(breast_test )

set(CMAKE_BUILD_TYPE Debug)

set(BOOST_ROOT_PATH /home/tiancheng/third_party/boost)

include_directories(
  ${BOOST_ROOT_PATH}/include
)

add_executable(server server.cpp )
add_executable(client client.cpp )

set(links pthread)

target_link_libraries( client  ${links})
target_link_libraries( server  ${links})

