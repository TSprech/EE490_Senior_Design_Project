cmake_minimum_required(VERSION 3.25)

set(LIB_FILES ModuloScheduler.hpp)
add_library(ModuloScheduler INTERFACE ${LIB_FILES})
target_include_directories(ModuloScheduler INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(ModuloScheduler INTERFACE units)

IF (${UNIT_TESTING})
    add_executable(ModuloScheduler_Test ModuloScheduler_Test.cpp)
    target_link_libraries(ModuloScheduler_Test PUBLIC ModuloScheduler)
    target_link_libraries(ModuloScheduler_Test PUBLIC TypedUnits)

    enable_testing()
    target_link_libraries(ModuloScheduler_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(ModuloScheduler_Test)
ENDIF()
