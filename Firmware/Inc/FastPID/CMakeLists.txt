cmake_minimum_required(VERSION 3.25)

set(LIB_FILES FastPID.hpp FastPID.ipp FastPID.cpp)
add_library(FastPID ${LIB_FILES})
target_include_directories(FastPID PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

IF (${UNIT_TESTING})
    add_executable(FastPID_Test FastPID_Test.cpp)
    target_link_libraries(FastPID_Test PUBLIC FastPID)

    enable_testing()
    target_link_libraries(FastPID_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(FastPID_Test)
ENDIF()
