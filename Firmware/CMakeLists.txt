cmake_minimum_required(VERSION 3.25)
include(FetchContent)

project(EE490_Firmware CXX C ASM) # CMake project that will be using C++, C, and Assembly
set(CMAKE_C_STANDARD 23) # C++23
set(CMAKE_CXX_STANDARD 23) # C23

IF ("${PLATFORM}" STREQUAL "arm")
    include(Lib/pico-sdk/pico_sdk_init.cmake) # Gives access to all the pico-sdk CMake functions and libraries
    #set(PICO_BOARD adafruit_feather_rp2040)
    pico_sdk_init() # Initialize the SDK

    if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
        message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
    endif ()

    if (TARGET tinyusb_device) # This is required for the RP2040 to show up as a serial device, build directives should be inside this if
        #    add_compile_options(-mfloat-abi=soft)
        add_compile_options(-mcpu=cortex-m0 -mthumb -mthumb-interwork) # These lines are required for proper functionality when using CMakePresets (it sets it to THUMB instructions instead of ARM)

        set(SOURCES Src/main.cpp) # Put all source files to build here
        add_executable(EE490_Firmware ${SOURCES}) # Add all sources files to build (this line should be close to the top of the CMake file)

        # These must come after add_executable as the target needs to be created first
        pico_enable_stdio_usb(EE490_Firmware 1) # Enable USB output from stdio
        pico_enable_stdio_uart(EE490_Firmware 0) # Disable UART output from stdio

        set(RP2040_HW_LIBS pico_stdlib hardware_gpio hardware_pwm hardware_adc) # These are the libs from the Pico SDK which will be used
        target_link_libraries(EE490_Firmware PUBLIC ${RP2040_HW_LIBS}) # Add the Pico SDK hardware libs to the build

        pico_add_extra_outputs(EE490_Firmware) # Create map/bin/hex file etc.

        add_link_options(--gc-sections) # Throw out unused functions so they aren't taking up extra flash
        add_link_options(--print-memory-usage) # At the end of compilation, print out the flash, memory, etc. usage

        add_compile_options(-Wall -Werror -Wpedantic)

        FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
        FetchContent_MakeAvailable(json)

        #    set(JSON_SystemInclude ON)
        set(JSON_MultipleHeaders ON)
        target_link_libraries(EE490_Firmware PRIVATE nlohmann_json::nlohmann_json)

        add_subdirectory(Inc/RP2040_PWM)
        target_link_libraries(EE490_Firmware PRIVATE RP2040_PWM)
    elseif (PICO_ON_DEVICE)
        message(WARNING "TinyUSB submodule is not initialized in the SDK, project will not build as USB serial will not work")
    endif ()
ELSE ()
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/a3580180d16923d6d5f488e20b3814608a892f17.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)
    add_executable(EE490_Firmware Src/test_main.cpp)

ENDIF ()

set(DISABLE_IOSTREAM ON)
set(DISABLE_PREDEFINED_UNITS ON)
set(ENABLE_PREDEFINED_CONSTANTS_UNITS ON)
add_subdirectory(Lib/units)
target_link_libraries(EE490_Firmware PUBLIC units)

add_subdirectory(Inc/TypedUnits)
target_link_libraries(EE490_Firmware PUBLIC TypedUnits)

add_subdirectory(Inc/FastPID)
target_link_libraries(EE490_Firmware PUBLIC FastPID)

add_subdirectory(Inc/ModuloScheduler)
target_link_libraries(EE490_Firmware PUBLIC ModuloScheduler)
