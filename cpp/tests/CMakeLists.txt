cmake_minimum_required(VERSION 3.16.3)

file(GLOB_RECURSE test_cshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(test_cshogi ${test_cshogi_src})
target_link_libraries(test_cshogi PRIVATE cshogi CppUTest)
