set(target_name neopixel_pio)

# add a new executable target
add_executable(${target_name})

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(${target_name}
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

# add some source code files
target_sources(${target_name} PRIVATE
  ${target_name}.c
)

# pull in common dependencies
target_link_libraries(${target_name}
  pico_stdlib
  hardware_pio
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${target_name} 1)
pico_enable_stdio_uart(${target_name} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${target_name})