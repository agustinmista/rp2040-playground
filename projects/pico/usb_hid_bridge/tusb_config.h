#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------
// COMMON CONFIGURATION
//-------------------------------------

// Set TinyUSB OS to pico-sdk
#define CFG_TUSB_OS OPT_OS_PICO

// Memory alignment macros
#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN __attribute__((aligned(4)))

//-------------------------------------
// DEVICE CONFIGURATION
//-------------------------------------

// Enable device stack
#define CFG_TUD_ENABLED 1

// Set device roothub port
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT 0
#endif

// Set endpoint size
#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#endif

// Device classes
#define CFG_TUD_HID 1

//-------------------------------------
// Class-specific configuration

// HID buffer size should be sufficient to hold ID (if any) + data
#define CFG_TUD_HID_EP_BUFSIZE 16

//-------------------------------------
// HOST CONFIGURATION
//-------------------------------------

// Enable host stack with pio-usb
#define CFG_TUH_ENABLED 1

// Set device roothub port
#ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT 1
#endif

// Use Pico-PIO-USB
#define CFG_TUH_RPI_PIO_USB 1

// Use pins 2 and 3 instead of 0 and 1 (used by default UART)
#define PIO_USB_DP_PIN_DEFAULT 2

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 256

// Enable USB Hub mode
#define CFG_TUH_HUB 1
#define CFG_TUH_DEVICE_MAX (CFG_TUH_HUB ? 4 : 1) // Hubs typically have 4 ports

//-------------------------------------
// Class-specific configuration

// Max number of HID interfaces
#define CFG_TUH_HID (3 * CFG_TUH_DEVICE_MAX)

// Buffer sizes of HID in and out endpoints
#define CFG_TUH_HID_EPIN_BUFSIZE 64
#define CFG_TUH_HID_EPOUT_BUFSIZE 64

//-------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
