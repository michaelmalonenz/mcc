cmake_minimum_required (VERSION 3.0)

include(CTest)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../.."
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../../test")

add_executable(test_Conditionals test_Conditionals.c)
target_link_libraries(test_Conditionals mccstatic testutils eral preproc)
add_test(test_Conditionals test_Conditionals)

add_executable(test_ICE test_ICE.c)
target_link_libraries(test_ICE mccstatic testutils eral preproc)
add_test(test_ICE test_ICE)

add_executable(test_Preprocessor test_Preprocessor.c)
target_link_libraries(test_Preprocessor mccstatic testutils eral preproc)
add_test(test_Preprocessor test_Preprocessor)

add_executable(test_Macros test_Macros.c)
target_link_libraries(test_Macros mccstatic testutils eral preproc)
add_test(test_Macros test_Macros)

add_executable(test_MacroReplacement test_MacroReplacement.c)
target_link_libraries(test_MacroReplacement mccstatic testutils eral preproc)
add_test(test_MacroReplacement test_MacroReplacement)