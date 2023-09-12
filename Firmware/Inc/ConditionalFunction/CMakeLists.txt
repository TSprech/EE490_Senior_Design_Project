cmake_minimum_required(VERSION 3.25)

set(LIB_FILES ConditionalFunction.hpp)
add_library(ConditionalFunction INTERFACE ${LIB_FILES})
target_include_directories(ConditionalFunction INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(ConditionalFunction INTERFACE units)

IF (${UNIT_TESTING})
    add_executable(ConditionalFunction_Test ConditionalFunction_Test.cpp)
    target_link_libraries(ConditionalFunction_Test PUBLIC ConditionalFunction)
    target_link_libraries(ConditionalFunction_Test PUBLIC TypedUnits)

    enable_testing()
    target_link_libraries(ConditionalFunction_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(ConditionalFunction_Test)
ENDIF()
