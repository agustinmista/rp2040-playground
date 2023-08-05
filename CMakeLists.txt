cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(tools/pico-sdk/pico_sdk_init.cmake)

# declare my project
project(my_projects)

# set the board variable
set(PICO_BOARD adafruit_qtpy_rp2040)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of the project
add_subdirectory(projects/hello_world)
add_subdirectory(projects/internal_temp)
add_subdirectory(projects/neopixel_pio)