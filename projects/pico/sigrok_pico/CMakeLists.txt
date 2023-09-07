set(target_name sigrok_pico)

# add a new executable target
add_executable(${target_name})

# add some source code files
target_sources(${target_name} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.c
)

# pull in common dependencies
target_link_libraries(${target_name}
  pico_stdlib
  hardware_adc
  hardware_dma
  hardware_pio
  hardware_sync
  pico_multicore
  pico_stdio_usb
)

# disable warnings in this mess
target_compile_options(${target_name} PRIVATE
  -w
)

# enable USB output, disable UART output
pico_enable_stdio_usb(${target_name} 1)
pico_enable_stdio_uart(${target_name} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${target_name})