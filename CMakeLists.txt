cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -ggdb -Wall -Wextra -Werror")
add_subdirectory(eral)
include_directories("eral" ${PROJECT_SOURCE_DIR})

set(SOURCE_FILES 
  config.h
  fileOpener.c
  ICE.c
  ICE.h
  macro_hash.c
  macro_private.h
  macro.c
  macro.h
  mcc.c
  mcc.h
  options.c
  options.h
  preprocessor.c
  tokeniser.c
  tokenList.c
  tokenList.h
  tokens.c
  tokens.h
  toolChainCommands.h
)
add_executable(mcc ${SOURCE_FILES})
target_link_libraries(mcc m eral)

add_library(mccstatic STATIC ${SOURCE_FILES})
target_link_libraries(mccstatic m)

add_subdirectory(test)

