# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(pico_keyboard C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico_keyboard pico_keyboard.c usb_descriptors.c key.c keys_config.c key_scan.c mouse_movement.c oled.c hid.c)

pico_set_program_name(pico_keyboard "pico_keyboard")
pico_set_program_version(pico_keyboard "0.1")
#pico_set_binary_type(pico_keyboard copy_to_ram)

#pico_enable_stdio_uart(pico_keyboard 1)

target_include_directories(pico_keyboard PUBLIC ./)

pico_generate_pio_header(pico_keyboard ${CMAKE_CURRENT_LIST_DIR}/key_scan.pio)
pico_generate_pio_header(pico_keyboard ${CMAKE_CURRENT_LIST_DIR}/oled.pio)

# Add the standard library to the build
target_link_libraries(pico_keyboard pico_stdlib)

# Add any user requested libraries
target_link_libraries(pico_keyboard
        hardware_pio
        hardware_dma
        tinyusb_device
        )

add_custom_target(pico_keyboard_font DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generated/oled_font.h)
add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/oled_font.h
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/oled_font.py
        COMMAND python ${CMAKE_CURRENT_LIST_DIR}/oled_font.py > oled_font.h)
add_dependencies(pico_keyboard pico_keyboard_font)

pico_add_extra_outputs(pico_keyboard)

