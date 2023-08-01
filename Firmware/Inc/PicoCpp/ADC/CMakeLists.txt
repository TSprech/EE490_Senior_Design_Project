cmake_minimum_required(VERSION 3.25)

set(LIB_FILES ADC.hpp)
add_library(ADC INTERFACE ${LIB_FILES})
target_include_directories(ADC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(ADC INTERFACE units)

IF (${UNIT_TESTING})
    add_executable(ADC_Test ADC_Test.cpp)
    target_link_libraries(ADC_Test PUBLIC ADC)
    target_link_libraries(ADC_Test PUBLIC TypedUnits)

    enable_testing()
    target_link_libraries(ADC_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(ADC_Test)
ENDIF()
