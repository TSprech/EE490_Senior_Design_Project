cmake_minimum_required(VERSION 3.25)

set(LIB_FILES MPPT.hpp)
add_library(MPPT INTERFACE ${LIB_FILES})
target_include_directories(MPPT INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(MPPT INTERFACE units)

IF (${UNIT_TESTING})
#    add_executable(MPPT_Test MPPT_Test.cpp)
#    target_link_libraries(MPPT_Test PUBLIC MPPT)
#    target_link_libraries(MPPT_Test PUBLIC TypedUnits)
#
#    enable_testing()
#    target_link_libraries(MPPT_Test PUBLIC GTest::gtest_main)
#    include(GoogleTest)
#    gtest_discover_tests(MPPT_Test)
ENDIF()
