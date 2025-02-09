function(vshogi_add_compile_options target)
    target_compile_features(${target} PUBLIC cxx_std_17)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -pedantic -Wall -Wextra -Weffc++ -Wcast-align -Wcast-qual
            -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self
            -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs
            -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls
            -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
            -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion
            -Wno-array-bounds)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wshadow)
    endif()
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-O0 -g3>)
    target_compile_definitions(${target} PRIVATE
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
endfunction(vshogi_add_compile_options)
