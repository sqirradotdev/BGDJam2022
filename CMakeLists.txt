cmake_minimum_required(VERSION 3.24)

project(BGDJam2022 VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11) # Requires C11 standard
if (MINGW)
    add_compile_options(-Wall -mwindows)
endif()

set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()
add_subdirectory(extern/cLDtk)
add_subdirectory(extern/Physac)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/build/debug/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/build/release/")

add_subdirectory(src)
