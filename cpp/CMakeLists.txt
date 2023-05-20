cmake_minimum_required(VERSION 3.16.3)

project(vshogi CXX)

include(./cmake/VShogiAddCompileOptions.cmake)

file(GLOB_RECURSE vshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(vshogi ${vshogi_src})
target_include_directories(vshogi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
vshogi_add_compile_options(vshogi)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
