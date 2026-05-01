cmake_minimum_required(VERSION 3.15...3.26)

project(vshogi LANGUAGES CXX)

if (NOT SKBUILD)
    message(WARNING "\
    This CMake file is meant to be executed using 'scikit-build-core'. Running
    it directly will almost certainly not produce the desired result. If
    you are a user trying to install this package, please use the command
    below, which will install all necessary build dependencies, compile
    the package in an isolated environment, and then install it.
    =====================================================================
    $ pip install .
    =====================================================================
    If you are a software developer, and this is your own package, then
    it is usually much more efficient to install the build dependencies
    in your environment once and use the following command that avoids
    a costly creation of a new virtual environment at every compilation:
    =====================================================================
    $ pip install nanobind scikit-build-core[pyproject]
    $ pip install --no-build-isolation -ve .
    =====================================================================
    You may optionally add -Ceditable.rebuild=true to auto-rebuild when
    the package is imported. Otherwise, you need to re-run the above
    after editing C++ files.")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Try to import all Python components potentially needed by nanobind
# Note: While we require Python 3.10+, stable ABI wheels (cp312+) will only
# be generated for Python 3.12+. Python 3.10-3.11 will get standard wheels.
find_package(Python 3.10
    REQUIRED COMPONENTS Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule)

# Import nanobind through CMake's find_package mechanism
find_package(nanobind CONFIG REQUIRED)

# --- Rust Integration ---
include(../cmake/RustIntegration.cmake)
# ------------------------

# Collect vshogi library source files
file(GLOB_RECURSE VSHOGI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)

# Include directories
set(VSHOGI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR})

# Build the main _vshogi extension module
nanobind_add_module(
    _vshogi
    STABLE_ABI
    NB_STATIC
    ${VSHOGI_SOURCES}
    vshogi_wrapper.cpp
)

target_link_libraries(_vshogi PRIVATE vshogi_rust_lib)
target_include_directories(_vshogi PRIVATE ${VSHOGI_INCLUDE_DIRS})
# Don't use vshogi_add_compile_options for nanobind modules due to strict warnings
target_compile_features(_vshogi PUBLIC cxx_std_17)

# Install directive for scikit-build-core
install(TARGETS _vshogi LIBRARY DESTINATION .)

# Build minishogi extension
nanobind_add_module(
    _minishogi
    STABLE_ABI
    NB_STATIC
    ${VSHOGI_SOURCES}
    minishogi_wrapper.cpp
)

target_link_libraries(_minishogi PRIVATE vshogi_rust_lib)
target_include_directories(_minishogi PRIVATE ${VSHOGI_INCLUDE_DIRS})
# Don't use vshogi_add_compile_options for nanobind modules due to strict warnings
target_compile_features(_minishogi PUBLIC cxx_std_17)

# Install directive for scikit-build-core
install(TARGETS _minishogi LIBRARY DESTINATION minishogi)

# Build judkins_shogi extension
nanobind_add_module(
    _judkins_shogi
    STABLE_ABI
    NB_STATIC
    ${VSHOGI_SOURCES}
    judkins_shogi_wrapper.cpp
)

target_link_libraries(_judkins_shogi PRIVATE vshogi_rust_lib)
target_include_directories(_judkins_shogi PRIVATE ${VSHOGI_INCLUDE_DIRS})
# Don't use vshogi_add_compile_options for nanobind modules due to strict warnings
target_compile_features(_judkins_shogi PUBLIC cxx_std_17)

# Install directive for scikit-build-core
install(TARGETS _judkins_shogi LIBRARY DESTINATION judkins_shogi)

# Build shogi extension
nanobind_add_module(
    _shogi
    STABLE_ABI
    NB_STATIC
    ${VSHOGI_SOURCES}
    shogi_wrapper.cpp
)

target_link_libraries(_shogi PRIVATE vshogi_rust_lib)
target_include_directories(_shogi PRIVATE ${VSHOGI_INCLUDE_DIRS})
# Don't use vshogi_add_compile_options for nanobind modules due to strict warnings
target_compile_features(_shogi PUBLIC cxx_std_17)

# Install directive for scikit-build-core
install(TARGETS _shogi LIBRARY DESTINATION shogi)

# Copy Python source files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../python/vshogi
        DESTINATION .
        FILES_MATCHING PATTERN "*.py")
