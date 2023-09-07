set(target_name usb_hid_bridge)

# add a new executable target
add_executable(${target_name})

# add some source code files
target_sources(${target_name} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/main.c
  ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
  # we can use 'tinyusb_pico_pio_usb' library later when pico-sdk is updated
  ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
  ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
)

# pull in common dependencies
target_link_libraries(${target_name}
  pico_unique_id
  pico_stdlib
  pico_multicore
  pico_pio_usb
  tinyusb_device
  tinyusb_host
)

# use tinyusb implementation
target_compile_definitions(${target_name} PRIVATE
  PIO_USB_USE_TINYUSB
)

# needed so tinyusb can find tusb_config.h
target_include_directories(${target_name} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# disable USB output, enable UART output
pico_enable_stdio_usb(${target_name} 0)
pico_enable_stdio_uart(${target_name} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${target_name})