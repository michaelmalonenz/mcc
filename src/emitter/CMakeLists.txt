cmake_minimum_required (VERSION 3.0)

set(SOURCES
    emitter.c
)

add_library(emitter STATIC "${SOURCES}")
target_link_libraries(emitter eral)
target_include_directories(emitter
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/.."
      "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test)
endif()
