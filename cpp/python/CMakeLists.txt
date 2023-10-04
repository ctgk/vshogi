# Set the minimum CMake version and policies for highest tested version
cmake_minimum_required(VERSION 3.15...3.27)

# Set up the project and ensure there is a working C++ compiler
project(vshogi_extension LANGUAGES CXX)

# Warn if the user invokes CMake directly
if (NOT SKBUILD)
    message(WARNING "\
    This CMake file is meant to be executed using 'scikit-build-core'.
    Running it directly will almost certainly not produce the desired
    result. If you are a user trying to install this package, use the
    command below, which will install all necessary build dependencies,
    compile the package in an isolated environment, and then install it.
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
    the package is imported. Otherwise, you need to rerun the above
    after editing C++ files.")
endif()

# Try to import all Python components potentially needed by nanobind
find_package(Python 3.8
    REQUIRED COMPONENTS Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule)

# Import nanobind through CMake's find_package mechanism
find_package(nanobind CONFIG REQUIRED)

# We are now ready to compile the actual extension module
nanobind_add_module(
    # Name of the extension
    _vshogi_extension

    # Target the stable ABI for Python 3.12+, which reduces
    # the number of binary wheels that must be built. This
    # does nothing on older Python versions
    STABLE_ABI

    # Source code goes here
    ${CMAKE_SOURCE_DIR}/vshogi_extension.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/animal_shogi/board.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/animal_shogi/stand.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/minishogi/bitboard.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/minishogi/board.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/judkins_shogi/board.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/shogi/bitboard.cpp
    ${CMAKE_SOURCE_DIR}/../src/vshogi/shogi/board.cpp
)

target_include_directories(_vshogi_extension
    PUBLIC ${CMAKE_SOURCE_DIR}/../include)

# Install directive for scikit-build-core
install(TARGETS _vshogi_extension LIBRARY DESTINATION vshogi)
