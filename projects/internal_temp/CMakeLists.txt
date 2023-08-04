add_executable(internal_temp
  internal_temp.c
)

# pull in common dependencies
target_link_libraries(internal_temp
  pico_stdlib
  hardware_adc
)

# enable usb output, disable uart output
pico_enable_stdio_usb(internal_temp 1)
pico_enable_stdio_uart(internal_temp 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(internal_temp)