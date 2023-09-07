cmake_minimum_required(VERSION 3.13)

#--------------------------------------
# Main project configuration
#--------------------------------------

# Bring the pico-sdk into scope
include(tools/pico-sdk/pico_sdk_init.cmake)

# Start a project
project(my_projects)

# Set pico-sdk flags
set(CMAKE_BUILD_TYPE Debug)   # Set to "Release" after debugging
set(PICO_DEOPTIMIZED_DEBUG 1) # Set to 0 after debugging

# Enable all compiler warnings
add_compile_options(-Wall -Wextra)

# Initialize the pico-sdk
pico_sdk_init()

# Add external libraries
add_subdirectory("tools/Pico-PIO-USB" pico_pio_usb)

#--------------------------------------
# Individual projects
#--------------------------------------

# Projects built for Pico
add_subdirectory(projects/pico/sigrok_pico)
add_subdirectory(projects/pico/usb_hid_bridge)

# Projects built for Adafruit QT Py RP2040
add_subdirectory(projects/adafruit_qtpy_rp2040/hello_world)
add_subdirectory(projects/adafruit_qtpy_rp2040/internal_temp)
add_subdirectory(projects/adafruit_qtpy_rp2040/neopixel_pio)

# Projects built for Cytron Maker Pi RP2040
add_subdirectory(projects/cytron_maker_pi_rp2040/hello_neopixel)