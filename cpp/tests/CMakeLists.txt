cmake_minimum_required(VERSION 3.16.3)

include(../cmake/VShogiAddCompileOptions.cmake)
file(GLOB_RECURSE test_vshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(test_vshogi ${test_vshogi_src})
target_link_libraries(test_vshogi PRIVATE vshogi CppUTest)
vshogi_add_compile_options(test_vshogi)
