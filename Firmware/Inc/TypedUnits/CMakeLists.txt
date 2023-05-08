cmake_minimum_required(VERSION 3.25)

set(LIB_FILES TypedUnits.hpp)
add_library(TypedUnits INTERFACE ${LIB_FILES})
target_include_directories(TypedUnits INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(TypedUnits INTERFACE units)

#IF (${UNIT_TESTING})
#    add_executable(TypedUnits_Test TypedUnits_Test.cpp)
#    target_link_libraries(TypedUnits_Test INTERFACE TypedUnits)
#
#    enable_testing()
#    target_link_libraries(TypedUnits_Test INTERFACE GTest::gtest_main)
#    include(GoogleTest)
#    gtest_discover_tests(TypedUnits_Test)
#ENDIF()
