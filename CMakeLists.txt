cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)
enable_testing()

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb3 -fprofile-arcs -ftest-coverage -DMCC_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DMCC_DEBUG=0")
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -flto -Wno-unused-command-line-argument")

if (NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo
  MinSizeRel."
  FORCE )
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld")
endif()

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fprofile-arcs -ftest-coverage")
add_subdirectory(src)

find_program( MEMORYCHECK_COMMAND valgrind )
set(MEMORYCHECK_COMMAND_OPTIONS "--track-origins=yes --leak-check=full --errors-for-leak-kinds=all --error-exitcode=1 --log-fd=2")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test)
endif()
