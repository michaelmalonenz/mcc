cmake_minimum_required (VERSION 3.0)
project (liberal)
enable_language(C)

include_directories("${PROJECT_SOURCE_DIR}/..")
set(SOURCE 
  fileBuffer.c
  fileBuffer.h
  list.c
  list.h
  stack.c
  stack.h
  stringBuffer.c
  stringBuffer.h
)

add_library(eral STATIC ${SOURCE})