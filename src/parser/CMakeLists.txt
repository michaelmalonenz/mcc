cmake_minimum_required (VERSION 3.0)

set(SOURCES
    parser.c
)

add_library(parser STATIC "${SOURCES}")
target_link_libraries(parser eral)
target_include_directories(parser
    PUBLIC CMAKE_CURRENT_SOURCE_DIR
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/.."
      "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test)
endif()
