add_executable(sigrok_pico)

# add some source code files
target_sources(sigrok_pico PRIVATE
  sigrok_pico.c
)

# pull in common dependencies
target_link_libraries(sigrok_pico
  pico_stdlib
  hardware_adc
  hardware_dma
  hardware_pio
  hardware_sync
  pico_multicore
  pico_stdio_usb
)

# enable usb output, disable uart output
pico_enable_stdio_usb(sigrok_pico 1)
pico_enable_stdio_uart(sigrok_pico 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(sigrok_pico)