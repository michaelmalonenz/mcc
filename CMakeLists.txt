cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -ggdb3 -Wall -Wextra -Werror --coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} --coverage")
add_subdirectory(src)

add_subdirectory(test)
