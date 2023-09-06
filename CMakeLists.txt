cmake_minimum_required(VERSION 3.13)

include(tools/pico-sdk/pico_sdk_init.cmake)

project(my_projects)

pico_sdk_init()

# Enable warnings
add_compile_options(-Wall -Wextra)

# Projects built for Pico
add_subdirectory(projects/pico/sigrok_pico)

# Projects built for Adafruit QT Py RP2040
add_subdirectory(projects/adafruit_qtpy_rp2040/hello_world)
add_subdirectory(projects/adafruit_qtpy_rp2040/internal_temp)
add_subdirectory(projects/adafruit_qtpy_rp2040/neopixel_pio)

# Projects built for Cytron Maker Pi RP2040
add_subdirectory(projects/cytron_maker_pi_rp2040/hello_neopixel)