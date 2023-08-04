cmake_minimum_required(VERSION 3.13)

# Initialize pico-sdk from submodule
include(tools/pico-sdk/pico_sdk_init.cmake)

# Declare my project
project(my_projects)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Rest of the project
add_subdirectory(projects/hello_world)