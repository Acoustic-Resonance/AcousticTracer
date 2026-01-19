cmake_minimum_required(VERSION 3.16)
project(AcousticTracer C)

# ---------------------------------------------------------
# CLI argument: choose which file in core_dev/ to compile
# ---------------------------------------------------------
set(DEV_FILE "" CACHE STRING "Name of the C file inside core_dev/ to compile")

if(NOT DEV_FILE)
    message(FATAL_ERROR "You must pass -DDEV_FILE=<file.c> (must exist in core_dev/)")
endif()

set(DEV_SOURCE "${CMAKE_SOURCE_DIR}/core_dev/${DEV_FILE}")

if(NOT EXISTS "${DEV_SOURCE}")
    message(FATAL_ERROR "The file core_dev/${DEV_FILE} does not exist")
endif()

# ---------------------------------------------------------
# Gather all engine sources from src/
# ---------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

# ---------------------------------------------------------
# Build the executable
# ---------------------------------------------------------
add_executable(at
    ${ENGINE_SOURCES}
    ${DEV_SOURCE}
)

# ---------------------------------------------------------
# cgltf (header-only, always included)
# ---------------------------------------------------------
target_include_directories(at PRIVATE external_libs/cgltf)

# ---------------------------------------------------------
# Math library (always)
# ---------------------------------------------------------
target_link_libraries(at PRIVATE m)

# ---------------------------------------------------------
# Optional raylib inclusion
# ---------------------------------------------------------
option(USE_DEBUG_RAYLIB "Enable raylib when ON" OFF)

if(USE_DEBUG_RAYLIB)
    message(STATUS "Debug flag enabled — using raylib")

    # Use your local header
    target_include_directories(at PRIVATE external_libs)

    # Fetch + build raylib library
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

    # Link the built raylib library
    target_link_libraries(at PRIVATE raylib)
endif()

if(APPLE)
  target_link_libraries(at PRIVATE
      "-framework Cocoa"
      "-framework Metal"
      "-framework QuartzCore"
      "-framework IOKit"
  )
endif()

# Build instructions
#
# 1. `mkdir -p build`
# 2. `cd build`
# 3. `cmake .. -DDEV_FILE=main.c`
# to include raylib
# 4. `cmake .. -DUSE_DEBUG_RAYLIB=ON -DDEV_FILE=main.c`
# 5. `make`
# 6. `./at`
