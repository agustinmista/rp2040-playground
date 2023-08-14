add_executable(hello_world)

# pull in common dependencies
target_link_libraries(hello_world
  pico_stdlib
)

# add some source code files
target_sources(hello_world PRIVATE
  hello_world.c
)

# enable USB output, disable UART output
pico_enable_stdio_usb(hello_world 1)
pico_enable_stdio_uart(hello_world 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_world)