# Rust Integration for vshogi

# Configure Rust to build inside the CMake build directory
# CMAKE_CURRENT_LIST_DIR is the directory containing this script (cpp/cmake)
get_filename_component(VSHOGI_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(RUST_SOURCE_DIR "${VSHOGI_ROOT_DIR}/rust")

message(STATUS "Rust source directory: ${RUST_SOURCE_DIR}")

set(RUST_LIB_NAME vshogi_rust)
set(RUST_TARGET_DIR ${CMAKE_BINARY_DIR}/rust_target)

# Handle different build types more robustly
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(RUST_CONFIG debug)
    set(CARGO_RELEASE_FLAG "")
else()
    set(RUST_CONFIG release)
    set(CARGO_RELEASE_FLAG "--release")
endif()

set(RUST_LIB_PATH "${RUST_TARGET_DIR}/${RUST_CONFIG}/lib${RUST_LIB_NAME}.a")

message(STATUS "Rust configuration: ${RUST_CONFIG}")
message(STATUS "Rust library path: ${RUST_LIB_PATH}")

add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${RUST_TARGET_DIR} cargo build ${CARGO_RELEASE_FLAG}
    WORKING_DIRECTORY ${RUST_SOURCE_DIR}
    COMMENT "Building Rust library in ${RUST_TARGET_DIR} (${RUST_CONFIG})"
    VERBATIM
)

add_custom_target(vshogi_rust_target ALL DEPENDS ${RUST_LIB_PATH})

add_library(vshogi_rust_lib STATIC IMPORTED GLOBAL)
add_dependencies(vshogi_rust_lib vshogi_rust_target)
set_target_properties(vshogi_rust_lib PROPERTIES
    IMPORTED_LOCATION ${RUST_LIB_PATH}
)
# Ensure the linker knows it's a Rust static library and might need extra libs
if(APPLE)
    # On macOS, we might need to link against SystemConfiguration or other frameworks if Rust uses them,
    # but for simple logic it should be fine.
endif()
