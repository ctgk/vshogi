# Rust Integration for vshogi

# Configure Rust to build inside the CMake build directory
set(RUST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../rust)
if (NOT EXISTS ${RUST_SOURCE_DIR}/Cargo.toml)
    # Fallback for when current source dir is different (e.g. in cpp/ instead of cpp/python/)
    set(RUST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../rust)
endif()

set(RUST_LIB_NAME vshogi_rust)
set(RUST_TARGET_DIR ${CMAKE_BINARY_DIR}/rust_target)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RUST_LIB_PATH ${RUST_TARGET_DIR}/debug/lib${RUST_LIB_NAME}.a)
else()
    set(RUST_LIB_PATH ${RUST_TARGET_DIR}/release/lib${RUST_LIB_NAME}.a)
    set(CARGO_RELEASE_FLAG --release)
endif()

add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${RUST_TARGET_DIR} cargo build ${CARGO_RELEASE_FLAG}
    WORKING_DIRECTORY ${RUST_SOURCE_DIR}
    COMMENT "Building Rust library in ${RUST_TARGET_DIR}"
)

add_custom_target(vshogi_rust_target ALL DEPENDS ${RUST_LIB_PATH})

add_library(vshogi_rust_lib STATIC IMPORTED GLOBAL)
add_dependencies(vshogi_rust_lib vshogi_rust_target)
set_target_properties(vshogi_rust_lib PROPERTIES
    IMPORTED_LOCATION ${RUST_LIB_PATH}
)
