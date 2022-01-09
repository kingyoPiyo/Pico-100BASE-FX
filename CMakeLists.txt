cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
project(pico-100base-fx)

pico_sdk_init()

set(target_name pico-100base-fx)
add_executable(${target_name})

pico_generate_pio_header(${target_name} ${CMAKE_CURRENT_LIST_DIR}/tx.pio)

target_sources(${target_name} PRIVATE main.c udp.c)

target_link_libraries(${target_name} PRIVATE pico_stdlib hardware_pio)

pico_add_extra_outputs(${target_name})
