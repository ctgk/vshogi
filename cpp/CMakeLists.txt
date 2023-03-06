cmake_minimum_required(VERSION 3.16.3)

project(vshogi CXX)
include(../cmake/VShogiAddCompileOptions.cmake)

file(GLOB_RECURSE vshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(vshogi ${vshogi_src})
target_include_directories(vshogi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
vshogi_add_compile_options(vshogi)
