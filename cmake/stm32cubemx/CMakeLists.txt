cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	USE_HAL_DRIVER 
	STM32F401xC
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../src/Core/Inc
    ../../src/Drivers/STM32F4xx_HAL_Driver/Inc
    ../../src/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ../../src/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../../src/Drivers/CMSIS/Include
)

target_sources(stm32cubemx INTERFACE
    ../../src/Core/Src/main.c
    ../../src/Core/Src/stm32f4xx_it.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ../../src/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ../../src/Core/Src/system_stm32f4xx.c
    ../../src/Core/Src/sysmem.c
    ../../src/Core/Src/syscalls.c
    ../../src/startup_stm32f401xc.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


