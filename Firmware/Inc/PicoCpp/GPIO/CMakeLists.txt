cmake_minimum_required(VERSION 3.25)

set(LIB_FILES GPIO.hpp)
add_library(GPIO INTERFACE ${LIB_FILES})
target_include_directories(GPIO INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(GPIO INTERFACE units)

IF (${UNIT_TESTING})
    add_executable(GPIO_Test GPIO_Test.cpp)
    target_link_libraries(GPIO_Test PUBLIC GPIO)
    target_link_libraries(GPIO_Test PUBLIC TypedUnits)

    enable_testing()
    target_link_libraries(GPIO_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(GPIO_Test)
ENDIF()
