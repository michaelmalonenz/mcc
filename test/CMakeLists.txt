cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)

include(CTest)

add_library(testutils TestUtils.c TestUtils.h)
target_link_libraries(testutils mccstatic)
target_include_directories(testutils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test_Options test_Options.c)
target_link_libraries(test_Options mccstatic eral)
add_test(test_Options test_Options)

add_executable(test_Tokeniser test_Tokeniser.c)
target_link_libraries(test_Tokeniser mccstatic testutils eral)
add_test(test_Tokeniser test_Tokeniser)

add_executable(test_TokenList test_TokenList.c)
target_link_libraries(test_TokenList mccstatic testutils eral)
add_test(test_TokenList test_TokenList)
