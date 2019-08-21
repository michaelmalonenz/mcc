cmake_minimum_required (VERSION 3.0)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../eral")
set(SOURCE
  conditional.c
  ICE.c
  ICE.h
  macro_hash.c
  macro_private.h
  macro.c
  macro.h
  preprocessor.c
  preprocessor_private.h
)

add_library(preproc STATIC ${SOURCE})
target_link_libraries(preproc eral)
add_subdirectory(test)
