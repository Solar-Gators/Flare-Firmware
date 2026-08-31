cmake_minimum_required(VERSION 3.15)
get_filename_component(SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${SOURCE_DIR_NAME} C ASM)

set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../resources)
set(MCU_FAMILY STM32U5xx)
set(MCU_MODEL STM32U575xx)
set(CPU_PARAMETERS -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard)
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/../Rear-VCU/CubeMX/startup_stm32u575xx.s)
set(SYSTEM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../Rear-VCU/Core/Src/system_stm32u5xx.c)
set(HAL_DIR ${RESOURCES_DIR}/Drivers/${MCU_FAMILY}_HAL_Driver)

set(BL_HAL_SOURCES
    ${HAL_DIR}/Src/stm32u5xx_hal.c
    ${HAL_DIR}/Src/stm32u5xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_flash.c
    ${HAL_DIR}/Src/stm32u5xx_hal_flash_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_fdcan.c
    ${HAL_DIR}/Src/stm32u5xx_hal_dma.c
    ${HAL_DIR}/Src/stm32u5xx_hal_exti.c
)

set(BL_PORT_COMMON
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports/stm32u5/bl_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports/stm32u5/bl_clock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports/stm32u5/bl_it.c
    ${SYSTEM_SRC}
    ${STARTUP_SCRIPT}
)
set(BL_PORT_CAN
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports/stm32u5/bl_can.c
)

set(BL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ports/stm32u5/Inc
    ${HAL_DIR}/Inc
    ${HAL_DIR}/Inc/Legacy
    ${RESOURCES_DIR}/Drivers/CMSIS/Device/ST/${MCU_FAMILY}/Include
    ${RESOURCES_DIR}/Drivers/CMSIS/Include
)

set(BL_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_slots.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_confirm.c
)

function(bl_firmware name)
    cmake_parse_arguments(A "WITH_CAN" "LINKER;BOARD" "SOURCES;DEFS" ${ARGN})
    set(_port ${BL_PORT_COMMON})
    if(A_WITH_CAN)
        list(APPEND _port ${BL_PORT_CAN})
    endif()
    add_executable(${name}
        ${A_SOURCES}
        ${BL_HAL_SOURCES}
        ${_port}
    )
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_include_directories(${name} PRIVATE ${BL_INCLUDE_DIRS})
    target_compile_definitions(${name} PRIVATE ${MCU_MODEL} USE_HAL_DRIVER ${A_DEFS})
    target_compile_options(${name} PRIVATE
        ${CPU_PARAMETERS}
        $<$<CONFIG:Debug>:-Og -g3 -ggdb>
        $<$<CONFIG:Release>:-O2 -g0>
        -Wall -Wextra -Wno-unused-parameter
    )
    target_link_options(${name} PRIVATE
        -T${A_LINKER}
        ${CPU_PARAMETERS}
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${name}.map
        --specs=nosys.specs
        -Wl,--gc-sections
        -Wl,--print-memory-usage
    )
    if(A_BOARD)
        target_sources(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/boards/${A_BOARD}.c)
    endif()
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${name}>
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${name}> ${CMAKE_CURRENT_BINARY_DIR}/${name}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${name}> ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin
        BYPRODUCTS
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.hex
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin
    )
endfunction()

set(BL_LD ${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32U575xx_BOOTLOADER.ld)
set(SLOT_A_LD ${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32U575xx_APP_SLOT_A.ld)
set(SLOT_B_LD ${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32U575xx_APP_SLOT_B.ld)

bl_firmware(bootloader-steering-wheel
    WITH_CAN
    LINKER ${BL_LD}
    BOARD steering_wheel
    SOURCES
        ${BL_CORE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_main.c
)

bl_firmware(bootloader-rear-vcu
    WITH_CAN
    LINKER ${BL_LD}
    BOARD rear_vcu
    SOURCES
        ${BL_CORE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_main.c
)

bl_firmware(bootloader-front-vcu
    WITH_CAN
    LINKER ${BL_LD}
    BOARD front_vcu
    SOURCES
        ${BL_CORE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_main.c
)

bl_firmware(bootloader-telemetry
    WITH_CAN
    LINKER ${BL_LD}
    BOARD telemetry
    SOURCES
        ${BL_CORE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_main.c
)

set(TEST_APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_app/test_app_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_slots.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_confirm.c
)

bl_firmware(bl-test-app-slot-a
    LINKER ${SLOT_A_LD}
    SOURCES ${TEST_APP_SOURCES}
    DEFS VECT_TAB_OFFSET=0x00010200 BL_TEST_CONFIRM=1
)

bl_firmware(bl-test-app-slot-b
    LINKER ${SLOT_B_LD}
    SOURCES ${TEST_APP_SOURCES}
    DEFS VECT_TAB_OFFSET=0x00108200 BL_TEST_CONFIRM=1
)

bl_firmware(bl-test-app-noconfirm-slot-b
    LINKER ${SLOT_B_LD}
    SOURCES ${TEST_APP_SOURCES}
    DEFS VECT_TAB_OFFSET=0x00108200 BL_TEST_CONFIRM=0
)

set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GEN_DIR})

function(bl_embed_bin target symbol c_file)
    add_custom_command(
        OUTPUT ${c_file}
        COMMAND ${CMAKE_COMMAND}
            -DINPUT=${CMAKE_CURRENT_BINARY_DIR}/${target}.bin
            -DOUTPUT=${c_file}
            -DSYMBOL=${symbol}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bin_to_c.cmake
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${target}.bin ${target}
        COMMENT "Embedding ${target}.bin as ${symbol}"
    )
endfunction()

bl_embed_bin(bl-test-app-slot-a bl_image_slot_a ${GEN_DIR}/bl_image_slot_a.c)
bl_embed_bin(bl-test-app-slot-b bl_image_slot_b ${GEN_DIR}/bl_image_slot_b.c)
bl_embed_bin(bl-test-app-noconfirm-slot-b bl_image_noconfirm_b ${GEN_DIR}/bl_image_noconfirm_b.c)

bl_firmware(bl-host-rear-vcu
    WITH_CAN
    LINKER ${BL_LD}
    BOARD rear_vcu
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/host_main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_crc32.c
        ${GEN_DIR}/bl_image_slot_a.c
        ${GEN_DIR}/bl_image_slot_b.c
        ${GEN_DIR}/bl_image_noconfirm_b.c
)

# Native A/B fallback tests (run on the build machine, not the MCU).
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(test_bl_slots
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bl_slots.c
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/fake_flash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_crc32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bl_slots.c
    )
    target_include_directories(test_bl_slots PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME test_bl_slots COMMAND test_bl_slots)
endif()
