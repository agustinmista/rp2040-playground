add_executable(hello_neopixel)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(hello_neopixel
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

# add the ./include directory
target_include_directories(hello_neopixel PRIVATE
  include
)

# add some source code files
target_sources(hello_neopixel PRIVATE
  hello_neopixel.c
)

# pull in common dependencies
target_link_libraries(hello_neopixel
  pico_stdlib
  hardware_pio
)

# enable usb output, disable uart output
pico_enable_stdio_usb(hello_neopixel 1)
pico_enable_stdio_uart(hello_neopixel 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_neopixel)