cmake_minimum_required(VERSION  3.15)

project(shm_test)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-Wall -g")

include_directories(${PROJECT_SOURCE_DIR})

set(SRC_LIST shm.cpp)

add_executable(write_test ${SRC_LIST} write.cpp)
add_executable(read_test ${SRC_LIST} read.cpp)

target_link_libraries(
  write_test
  PRIVATE
  rt pthread
)

target_link_libraries(
  read_test
  PRIVATE
  rt pthread
)



