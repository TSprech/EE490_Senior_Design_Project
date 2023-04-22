cmake_minimum_required(VERSION 3.25)
include(ARMGCCToolchain.cmake) # Defines our cross compiling toolchain (ARM GCC)
include(Lib/pico-sdk/pico_sdk_init.cmake) # Gives access to all the pico-sdk CMake functions and libraries

project(EE490_Firmware CXX C ASM) # CMake project that will be using C++, C, and Assembly
set(CMAKE_C_STANDARD 23) # C++23
set(CMAKE_CXX_STANDARD 23) # C23

pico_sdk_init() # Initialize the SDK

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

if (TARGET tinyusb_device) # This is required for the RP2040 to show up as a serial device, build directives should be inside this if
    set(SOURCES Src/main.cpp Inc/RP2040_PWM/RP2040_PWM.ipp) # Put all source files to build here
    add_executable(EE490_Firmware ${SOURCES}) # Add all sources files to build (this line should be close to the top of the CMake file)

    # These must come after add_executable as the target needs to be created first
    pico_enable_stdio_usb(EE490_Firmware 1) # Enable USB output from stdio
    pico_enable_stdio_uart(EE490_Firmware 0) # Disable UART output from stdio

    set(RP2040_HW_LIBS pico_stdlib hardware_gpio hardware_pwm) # These are the libs from the Pico SDK which will be used
    target_link_libraries(EE490_Firmware PUBLIC ${RP2040_HW_LIBS}) # Add the Pico SDK hardware libs to the build

    pico_add_extra_outputs(EE490_Firmware) # Create map/bin/hex file etc.

    add_link_options(--gc-sections) # Throw out unused functions so they aren't taking up extra flash
    add_link_options(--print-memory-usage) # At the end of compilation, print out the flash, memory, etc. usage

    add_subdirectory(Inc/RP2040_PWM)
    target_link_libraries(EE490_Firmware PRIVATE RP2040_PWM)
elseif (PICO_ON_DEVICE)
    message(WARNING "TinyUSB submodule is not initialized in the SDK, project will not build as USB serial will not work")
endif ()