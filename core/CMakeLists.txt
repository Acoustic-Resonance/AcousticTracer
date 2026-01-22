file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/*.c)

add_library(acoustic STATIC ${CORE_SOURCES})

target_include_directories(acoustic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(acoustic PUBLIC m)
