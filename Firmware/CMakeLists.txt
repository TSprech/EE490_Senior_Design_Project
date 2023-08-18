cmake_minimum_required(VERSION 3.25)
include(FetchContent)

set(UNIT_TESTING 0)

IF(NOT UNIT_TESTING)
set(CMAKE_SYSTEM_NAME Generic) # No OS
set(CMAKE_SYSTEM_VERSION 1)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
# Prevent CMake from attempting to compile the default test as it does not work for for embedded toolchains
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)
ENDIF()

#include(Lib/pico-sdk/pico_sdk_init.cmake) # Gives access to all the pico-sdk CMake functions and libraries
project(EE490_Firmware CXX C ASM) # CMake project that will be using C++, C, and Assembly
set(CMAKE_C_STANDARD 23) # C++23
set(CMAKE_CXX_STANDARD 23) # C23


IF(NOT UNIT_TESTING)
pico_sdk_init() # Initialize the SDK
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

if (TARGET tinyusb_device) # This is required for the RP2040 to show up as a serial device, build directives should be inside this if
#        add_compile_options(-mfloat-abi=soft)
#        add_compile_options(-march=cortex-m0 -mthumb -mthumb-interwork) # These lines are required for proper functionality when using CMakePresets (it sets it to THUMB instructions instead of ARM)

    set(SOURCES Src/main.cpp
            Src/main1.cpp
            Src/main1.cpp
            Src/main1.hpp) # Put all source files to build here
    add_executable(EE490_Firmware ${SOURCES}) # Add all sources files to build (this line should be close to the top of the CMake file)

    # These must come after add_executable as the target needs to be created first
    pico_enable_stdio_usb(EE490_Firmware 1) # Enable USB output from stdio
    pico_enable_stdio_uart(EE490_Firmware 0) # Disable UART output from stdio

    set(RP2040_HW_LIBS pico_stdlib hardware_gpio hardware_pwm hardware_adc hardware_i2c hardware_spi hardware_dma) # These are the libs from the Pico SDK which will be used
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
    add_subdirectory(Inc/PicoCpp/GPIO)
    add_subdirectory(Inc/PicoCpp/DMA)
    add_subdirectory(Inc/PicoCpp/ADC)
    target_link_libraries(EE490_Firmware PRIVATE
            RP2040_PWM
            GPIO
            DMA
            ADC
    )
elseif (PICO_ON_DEVICE)
    message(WARNING "TinyUSB submodule is not initialized in the SDK, project will not build as USB serial will not work")
endif ()
ELSE()
    enable_testing()
    set(SOURCES Src/test_main.cpp) # Put all source files to build here
    add_executable(EE490_Firmware ${SOURCES}) # Add all sources files to build (this line should be close to the top of the CMake file)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        release-1.11.0
    )

    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    target_link_libraries(EE490_Firmware PUBLIC gtest_main)
ENDIF()

##set(DISABLE_IOSTREAM ON)
##set(DISABLE_PREDEFINED_UNITS ON)
##set(ENABLE_PREDEFINED_CONSTANTS_UNITS ON)
add_subdirectory(Lib/units)
add_subdirectory(Inc/TypedUnits)
add_subdirectory(Inc/FastPID)
add_subdirectory(Inc/ModuloScheduler)
add_subdirectory(Lib/fmt)
add_subdirectory(Inc/MPPT)
add_subdirectory(Inc/ConditionalFunction)

target_link_libraries(EE490_Firmware PUBLIC
        units
        TypedUnits
        FastPID
        ModuloScheduler
        fmt
        MPPT
        ConditionalFunction
)

FetchContent_Declare(
        tinyfsm
        GIT_REPOSITORY https://github.com/digint/tinyfsm.git
        GIT_TAG        v0.3.3
)

FetchContent_GetProperties(tinyfsm)
if(NOT tinyfsm_POPULATED)
    FetchContent_Populate(tinyfsm)
    target_include_directories(EE490_Firmware PUBLIC ${tinyfsm_SOURCE_DIR}/include)
endif()