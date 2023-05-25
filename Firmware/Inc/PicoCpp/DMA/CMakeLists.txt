cmake_minimum_required(VERSION 3.25)

set(LIB_FILES DMA.hpp)
add_library(DMA INTERFACE ${LIB_FILES})
target_include_directories(DMA INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(DMA INTERFACE units)

IF (${UNIT_TESTING})
    add_executable(DMA_Test DMA_Test.cpp)
    target_link_libraries(DMA_Test PUBLIC DMA)
    target_link_libraries(DMA_Test PUBLIC TypedUnits)

    enable_testing()
    target_link_libraries(DMA_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(DMA_Test)
ENDIF()
