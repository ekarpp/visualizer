cmake_minimum_required(VERSION 3.20.0)
#set(BOARD stm32_min_dev)
#set(SHIELD ssd1306_128x64)
set(BOARD stm32_bp)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zz)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/adc.c)
target_sources(app PRIVATE src/ssd1306.c)
target_sources(app PRIVATE src/fft.c)
