cmake_minimum_required (VERSION 3.0)

include(CTest)
add_executable(test_emitter test_emitter.c)
target_link_libraries(test_emitter emitter eral)
add_test(test_emitter test_emitter)
