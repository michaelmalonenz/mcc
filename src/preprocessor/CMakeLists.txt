cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)

include_directories("${PROJECT_SOURCE_DIR}/.." "${PROJECT_SOURCE_DIR}/../eral")
set(SOURCE 
  ICE.c
  ICE.h
  macro_hash.c
  macro_private.h
  macro.c
  macro.h
  preprocessor.c
)

add_library(preproc STATIC ${SOURCE})
target_link_libraries(preproc eral)
add_subdirectory(test)
