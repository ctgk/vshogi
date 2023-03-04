cmake_minimum_required(VERSION 3.16.3)

project(cshogi CXX)

file(GLOB_RECURSE cshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(cshogi ${cshogi_src})
target_include_directories(cshogi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
