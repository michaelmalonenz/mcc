cmake_minimum_required (VERSION 3.0)

add_subdirectory(eral)
add_subdirectory(preprocessor)
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/eral"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/preprocessor"
)

set(SOURCE_FILES
  config.h
  fileOpener.c
  mcc.c
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
add_executable(mcc ${SOURCE_FILES})
add_compile_options(-O0)
target_link_libraries(mcc m eral preproc)

add_library(mccstatic STATIC ${SOURCE_FILES})
target_link_libraries(mccstatic m)
