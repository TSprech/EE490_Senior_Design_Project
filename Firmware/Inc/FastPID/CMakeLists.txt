cmake_minimum_required(VERSION 3.25)

set(LIB_FILES FastPID.hpp FastPID.ipp FastPID.cpp)
add_library(FastPID ${LIB_FILES})
target_include_directories(FastPID PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

IF ("${PLATFORM}" STREQUAL "win")
    add_executable(FastPID_Test FastPID_Test.cpp)
    target_link_libraries(FastPID_Test PUBLIC FastPID)

    enable_testing()
    target_link_libraries(FastPID_Test PUBLIC GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(FastPID_Test)
ENDIF()

#set(LIB_FILES "FastPID.hpp" "FastPID.ipp" "FastPID.cpp")
##set(LIB_FILES FastPID.hpp FastPID.ipp FastPID.cpp FastPID_Fixture.hpp FastPID_Test.cpp)
#
#IF ("${PLATFORM}" STREQUAL "win")
#    enable_testing()
#    list(APPEND LIB_FILES "FastPID_Fixture.hpp" "FastPID_Test.cpp")
#ELSE()
#    add_library(FastPID ${LIB_FILES})
#    target_include_directories(FastPID PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#ENDIF()
##
##MESSAGE(STATUS "Library files: ")
##MESSAGE(STATUS ${LIB_FILES})
#
#IF ("${PLATFORM}" STREQUAL "win")
#    add_library(FastPID STATIC ${LIB_FILES})
#    add_executable(FastPID_Test FastPID_Test.cpp)
#    target_include_directories(FastPID_Test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#    target_link_libraries(FastPID_Test PUBLIC FastPID)
#
#    include(GoogleTest)
#    target_link_libraries(FastPID PUBLIC GTest::gtest_main)
#    gtest_discover_tests(FastPID)
#ENDIF()



#IF ("${PLATFORM}" STREQUAL "win")
#    set(LIB_FILES FastPID.hpp FastPID.cpp)
#    add_library(FastPID STATIC ${LIB_FILES})
#    enable_testing()
#    add_executable(FastPIDTest FastPID_Test.cpp)
#
#    target_include_directories(FastPID PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#    target_include_directories(FastPIDTest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#    target_link_libraries(FastPIDTest PUBLIC FastPID)
#    target_link_libraries(FastPIDTest PUBLIC GTest::gtest_main)
#
#    include(GoogleTest)
#    gtest_discover_tests(FastPIDTest)
#ENDIF()
##ELSE()
##    set(LIB_FILES FastPID.hpp FastPID.ipp FastPID.cpp)
##
##    add_library(FastPID ${LIB_FILES})
##    target_include_directories(FastPID PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
##ENDIF()