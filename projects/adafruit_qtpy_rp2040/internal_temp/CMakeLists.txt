add_executable(internal_temp)

# pull in common dependencies
target_link_libraries(internal_temp
  pico_stdlib
  hardware_adc
)

# add some source code files
target_sources(internal_temp PRIVATE
  internal_temp.c
)

# enable USB output, disable UART output
pico_enable_stdio_usb(internal_temp 1)
pico_enable_stdio_uart(internal_temp 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(internal_temp)