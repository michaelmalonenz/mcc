cmake_minimum_required (VERSION 3.0)

include(CTest)
add_executable(test_parser test_parser.c)
target_link_libraries(test_parser mccstatic parser)
add_test(test_parser test_parser)

