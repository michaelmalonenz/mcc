cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -ggdb3 -Wall -Wextra -Werror")

include(CTest)
add_custom_target(test_valgrind
    COMMAND ${CMAKE_CTEST_COMMAND} 
        --force-new-ctest-process --test-action memcheck)

include_directories("${PROJECT_SOURCE_DIR}/..")
add_library(testutils TestUtils.c TestUtils.h)

add_executable(test_FileBuffer test_FileBuffer.c)
target_link_libraries(test_FileBuffer mccstatic eral)
add_test(test_FileBuffer test_FileBuffer)

add_executable(test_ICE test_ICE.c)
target_link_libraries(test_ICE mccstatic testutils eral)
add_test(test_ICE test_ICE)

add_executable(test_List test_List.c)
target_link_libraries(test_List mccstatic eral)
add_test(test_List test_List)

add_executable(test_MacroReplacement test_MacroReplacement.c)
target_link_libraries(test_MacroReplacement mccstatic testutils eral)
add_test(test_MacroReplacement test_MacroReplacement)

add_executable(test_Macros test_Macros.c)
target_link_libraries(test_Macros mccstatic testutils eral)
add_test(test_Macros test_Macros)

add_executable(test_Options test_Options.c)
target_link_libraries(test_Options mccstatic eral)
add_test(test_Options test_Options)

add_executable(test_Preprocessor test_Preprocessor.c)
target_link_libraries(test_Preprocessor mccstatic testutils eral)
add_test(test_Preprocessor test_Preprocessor)

add_executable(test_Stack test_Stack.c)
target_link_libraries(test_Stack mccstatic eral)
add_test(test_Stack test_Stack)

add_executable(test_StringBuffer test_StringBuffer.c)
target_link_libraries(test_StringBuffer mccstatic eral)
add_test(test_StringBuffer test_StringBuffer)

add_executable(test_Tokeniser test_Tokeniser.c)
target_link_libraries(test_Tokeniser mccstatic testutils eral)
add_test(test_Tokeniser test_Tokeniser)


