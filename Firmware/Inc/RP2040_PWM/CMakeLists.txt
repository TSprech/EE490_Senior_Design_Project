cmake_minimum_required(VERSION 3.25)

add_library(RP2040_PWM INTERFACE RP2040_PWM.hpp RP2040_PWM.ipp)
target_include_directories(RP2040_PWM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")