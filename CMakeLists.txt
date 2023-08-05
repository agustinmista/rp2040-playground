cmake_minimum_required(VERSION 3.13)

# Initialize pico-sdk from submodule
include(tools/pico-sdk/pico_sdk_init.cmake)

# Declare my project
project(my_projects)

# Set the board variable
set(PICO_BOARD adafruit_qtpy_rp2040)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Rest of the project
add_subdirectory(projects/hello_world)
add_subdirectory(projects/internal_temp)