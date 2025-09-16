# Template CMakeLists.txt configuration for STM32 MCUS
cmake_minimum_required(VERSION 3.12)
# ##############################################################################
# Edit your MCU information up to the next border
get_filename_component(SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${SOURCE_DIR_NAME})
set(USER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/user)
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../resources)

set(MCU_FAMILY STM32U5xx)
set(MCU_MODEL STM32U575xx)
set(CPU_PARAMETERS
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/startup_stm32u575xx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/CubeMX/STM32U575xx_FLASH.ld)
# ##############################################################################
set(EXECUTABLE ${SOURCE_DIR_NAME})
enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Headers
set(CUBEMX_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${RESOURCES_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc
    ${RESOURCES_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Inc/Legacy
    ${RESOURCES_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${RESOURCES_DIR}/Drivers/CMSIS/Include
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/CMSIS/RTOS2/Include
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/include
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure)
set(PROJECT_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${USER_DIR}/inc
    ${RESOURCES_DIR}/Drivers/SG_Drivers/CAN/Include
    ${RESOURCES_DIR}/Drivers/SG_Drivers/eeprom/inc)
# Sources
file(GLOB STM32CUBEMX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c
    ${RESOURCES_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver/Src/*.c
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/*.c
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${RESOURCES_DIR}/Middlewares_v2/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/*.c)
file(GLOB_RECURSE PROJECT_SOURCES FOLLOW_SYMLINKS
    ${USER_DIR}/src/*.cpp
    ${USER_DIR}/src/*.c
    ${RESOURCES_DIR}/Drivers/SG_Drivers/CAN/Src/*.cpp
    ${RESOURCES_DIR}/Drivers/SG_Drivers/eeprom/src/*.cpp
    ${RESOURCES_DIR}/Drivers/SG_Drivers/Src/*.cpp)


# Executable files
add_executable(${EXECUTABLE}
    ${STM32CUBEMX_SOURCES} 
    ${PROJECT_SOURCES}
    ${STARTUP_SCRIPT})

# Strict warnings only for user sources
set_source_files_properties(${PROJECT_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wsuggest-override;-Wno-unused-parameter;-Wno-pedantic"
)

# Relaxed / suppressed for Cube/HAL
set_source_files_properties(${STM32CUBEMX_SOURCES} PROPERTIES
    COMPILE_OPTIONS "-w"
)

# Embedded macros(defines)
target_compile_definitions(${EXECUTABLE} PRIVATE
    ${MCU_MODEL}
    USE_HAL_DRIVER)

# Add header directories (AFTER add_executable !!)
target_include_directories(${EXECUTABLE} SYSTEM PRIVATE # Supresses warnings from the hal and cmsis libs
    ${CUBEMX_INCLUDE_DIRECTORIES}
    ${PROJECT_INCLUDE_DIRECTORIES})

# Compiler and linker options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-O2 -g0>)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -Wl,--end-group
    -Wl,--print-memory-usage)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}>
    ${EXECUTABLE}.bin)