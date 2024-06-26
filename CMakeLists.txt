cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME stm32f401-lorawan-node)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add LoRaMac-Node
add_subdirectory(lib/LoRaMac)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    src/Board/Src/adc-board.c
    src/Board/Src/board.c
    src/Board/Src/delay-board.c
    src/Board/Src/gpio-board.c
    src/Board/Src/rtc-board.c
    src/Board/Src/eeprom-board.c
    src/Board/Src/spi-board.c
    src/Board/Src/sx1262-board.c
    src/Board/Src/lorawan.c
    src/Board/Src/dht.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    src/Board/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    lorawan-node
)

# Define output file names
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${OUTPUT_DIR})
set(OUTPUT_HEX ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.hex)
set(OUTPUT_BIN ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.bin)
set(OUTPUT_ELF ${OUTPUT_DIR}/${CMAKE_PROJECT_NAME}.elf)

# Add custom command to generate HEX file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${OUTPUT_HEX}
    COMMENT "Generating ${OUTPUT_HEX}"
)

# Add custom command to generate BIN file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${OUTPUT_BIN}
    COMMENT "Generating ${OUTPUT_BIN}"
)

# Copy ELF file to output directory
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${OUTPUT_ELF}
    COMMENT "Copying ${OUTPUT_ELF}"
)