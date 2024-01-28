cmake_minimum_required(VERSION 3.16.3)

option(BUILD_FOR_PROFILE "Build C++ test program for code profile" OFF)

include(../cmake/VShogiAddCompileOptions.cmake)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_package(CppUTest)
endif()

file(GLOB_RECURSE test_vshogi_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(test_vshogi ${test_vshogi_src})
target_include_directories(test_vshogi
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_vshogi PRIVATE vshogi CppUTest)

if(BUILD_FOR_PROFILE)
    target_compile_definitions(test_vshogi
        PRIVATE
            CPPUTEST_MEM_LEAK_DETECTION_DISABLED)
endif(BUILD_FOR_PROFILE)
vshogi_add_compile_options(test_vshogi)
