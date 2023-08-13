cmake_minimum_required(VERSION 3.13)

include(tools/pico-sdk/pico_sdk_init.cmake)

project(my_projects)

pico_sdk_init()

add_subdirectory(projects/pico/sigrok_pico)
add_subdirectory(projects/adafruit_qtpy_rp2040/hello_world)
add_subdirectory(projects/adafruit_qtpy_rp2040/internal_temp)
add_subdirectory(projects/adafruit_qtpy_rp2040/neopixel_pio)
add_subdirectory(projects/cytron_maker_pi_rp2040/hello_neopixel)