# 'Google_test' is the subproject name
project(google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
add_executable(google_tests_run simple_tests.cpp cache_created_tests.cpp basic_tests.cpp cache_additional_functions_tests.cpp pseudoLRU_tests.cpp cache_code_request_tests.cpp cache_code_response_tests.cpp cache_data_request_tests.cpp cache_data_response_tests.cpp general_cache_tests.cpp)
target_link_libraries(google_tests_run gtest gtest_main)