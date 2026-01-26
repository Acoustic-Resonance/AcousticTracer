cmake_minimum_required(VERSION 3.16)
project(AcousticTracer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------
# CLI argument: choose which file in core/dev/ to compile
# ---------------------------------------------------------
set(DEV_FILE "" CACHE STRING "Name of the C file inside core/dev/ to compile")

if(NOT DEV_FILE)
    message(FATAL_ERROR "You must pass -DDEV_FILE=<file.c> (must exist in core/dev/)")
endif()

set(DEV_SOURCE "${CMAKE_SOURCE_DIR}/core/dev/${DEV_FILE}")
if(NOT EXISTS "${DEV_SOURCE}")
    message(FATAL_ERROR "The file core/dev/${DEV_FILE} does not exist")
endif()

# ---------------------------------------------------------
# Gather all core sources from core/src/
# ---------------------------------------------------------
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    core/src/*.c
)

# ---------------------------------------------------------
# Build the executable
# ---------------------------------------------------------
add_executable(at
    ${CORE_SOURCES}
    ${DEV_SOURCE}
)

# ---------------------------------------------------------
# Include directories
# ---------------------------------------------------------
target_include_directories(at PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/core/external
)

# ---------------------------------------------------------
# Math library
# ---------------------------------------------------------
target_link_libraries(at PRIVATE m)

# ---------------------------------------------------------
# Optional raylib inclusion
# ---------------------------------------------------------
option(USE_DEBUG_RAYLIB "Enable raylib when ON" OFF)

if(USE_DEBUG_RAYLIB)
    message(STATUS "Debug flag enabled — using raylib")

    set(RAYLIB_VERSION 5.5)
    find_package(raylib ${RAYLIB_VERSION} QUIET)

    if(NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        FetchContent_GetProperties(raylib)
        if(NOT raylib_POPULATED)
            set(FETCHCONTENT_QUIET NO)
            FetchContent_MakeAvailable(raylib)
        endif()
    endif()

    target_include_directories(at PRIVATE
        ${raylib_SOURCE_DIR}/src/external
    )
    target_link_libraries(at PRIVATE raylib)
    target_compile_definitions(at PRIVATE USE_RAYLIB)
endif()

# ---------------------------------------------------------
# Platform-specific linking (macOS)
# ---------------------------------------------------------
if(APPLE)
    target_link_libraries(at PRIVATE
        "-framework Cocoa"
        "-framework Metal"
        "-framework QuartzCore"
        "-framework IOKit"
    )
endif()

# ---------------------------------------------------------
# Build type configuration
# ---------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(at PRIVATE -Wall -Wextra -g)
else()
    target_compile_options(at PRIVATE -O3)
endif()

# Build instructions
#
# 1. mkdir -p build
# 2. cd build
# 3. cmake .. -DDEV_FILE=main.c
# 4. (optional) cmake .. -DUSE_DEBUG_RAYLIB=ON -DDEV_FILE=main.c
# 5. make
# 6. ./at
