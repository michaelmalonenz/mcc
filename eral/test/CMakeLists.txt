cmake_minimum_required (VERSION 3.0)
project (Mcc)
enable_language(C)

include(CTest)
include_directories("${PROJECT_SOURCE_DIR}/..")
include_directories("${PROJECT_SOURCE_DIR}/../..")

add_executable(test_FileBuffer test_FileBuffer.c)
target_link_libraries(test_FileBuffer mccstatic eral)
add_test(test_FileBuffer test_FileBuffer)

add_executable(test_List test_List.c)
target_link_libraries(test_List mccstatic eral)
add_test(test_List test_List)

add_executable(test_Stack test_Stack.c)
target_link_libraries(test_Stack mccstatic eral)
add_test(test_Stack test_Stack)

add_executable(test_StringBuffer test_StringBuffer.c)
target_link_libraries(test_StringBuffer mccstatic eral)
add_test(test_StringBuffer test_StringBuffer)
