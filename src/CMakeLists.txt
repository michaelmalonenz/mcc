cmake_minimum_required (VERSION 3.0)

add_compile_options(-O0 -Wextra -Werror -Wall -g -ggdb3 -Wno-unused-command-line-argument)

add_subdirectory(eral)
add_subdirectory(preprocessor)
add_subdirectory(emitter)
add_subdirectory(parser)

set(SOURCE_FILES
  fileOpener.c
  mcc.h
  options.c
  options.h
  tokeniser.c
  tokenList.c
  tokenList.h
  tokens.c
  tokens.h
  toolChainCommands.h
)
add_library(mccstatic STATIC ${SOURCE_FILES})
target_link_libraries(mccstatic m eral preproc parser)
target_include_directories(mccstatic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(mcc mcc.c)
target_link_libraries(mcc mccstatic)
