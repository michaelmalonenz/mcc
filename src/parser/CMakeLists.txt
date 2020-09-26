cmake_minimum_required (VERSION 3.0)

set(SOURCES
    parser.c
)

add_library(parser STATIC "${SOURCES}")
target_include_directories(parser
    PUBLIC CMAKE_CURRENT_SOURCE_DIR
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test)
endif()
