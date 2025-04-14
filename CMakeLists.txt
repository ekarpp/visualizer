cmake_minimum_required(VERSION 3.20.0)
set(BOARD stm32_bp)
set(SAMPLES_LG 8)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(visualizer)

add_compile_definitions(SAMPLES_LG=${SAMPLES_LG})

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/tables.h"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/table_gen.py" ${SAMPLES_LG}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/table_gen.py"
)

add_custom_target(
    tables
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/tables.h"
)

add_dependencies(app tables)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/adc.c)
target_sources(app PRIVATE src/ssd1306.c)
target_sources(app PRIVATE src/fft.c)
