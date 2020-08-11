cmake_minimum_required (VERSION 3.0)
enable_language(C)

set(SOURCE 
  fileBuffer.c
  fileBuffer.h
  hash_table.c
  hash_table.h
  list.c
  list.h
  stack.c
  stack.h
  stringBuffer.c
  stringBuffer.h
)

add_library(eral STATIC ${SOURCE})
target_include_directories(eral
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
add_subdirectory(test)
