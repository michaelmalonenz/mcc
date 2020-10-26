cmake_minimum_required (VERSION 3.0)

set(SOURCE
  conditional.c
  ICE.c
  ICE.h
  macro_hash.c
  macro.c
  macro.h
  preprocessor.c
  macro_expansion.c
  macro_expansion.h
)

add_library(preproc STATIC ${SOURCE})
target_link_libraries(preproc eral)
target_include_directories(preproc
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
  PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test)
endif()
