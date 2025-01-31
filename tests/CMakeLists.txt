cmake_minimum_required (VERSION 3.29)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(gtest_disable_pthreads on)


set(TestsFiles
	InvertedIndex_test.cpp
	ConfigLoader_test.cpp
)

add_executable(GtestTests ${TestsFiles} )
target_link_libraries(GtestTests nlohmann_json::nlohmann_json GTest::gtest_main)
include(GoogleTest)
target_include_directories(GtestTests PUBLIC ${MY_INCLUDE_DIR})
gtest_discover_tests(GtestTests)
