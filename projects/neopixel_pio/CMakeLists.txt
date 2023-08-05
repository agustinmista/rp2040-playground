add_executable(neopixel_pio)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(neopixel_pio
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

# add some source code files
target_sources(neopixel_pio PRIVATE
  neopixel_pio.c
)

# pull in common dependencies
target_link_libraries(neopixel_pio
  pico_stdlib
  hardware_pio
)

# enable usb output, disable uart output
pico_enable_stdio_usb(neopixel_pio 1)
pico_enable_stdio_uart(neopixel_pio 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(neopixel_pio)