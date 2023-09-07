#include "pico/unique_id.h"
#include "tusb.h"

//-------------------------------------
// Device descriptor
//-------------------------------------

#define USB_VID 0xCAFE

#define PID_MASK(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID (0x4000 | PID_MASK(CDC, 0) | PID_MASK(MSC, 1) | PID_MASK(HID, 2) | PID_MASK(MIDI, 3) | PID_MASK(VENDOR, 4))

tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&desc_device;
}

//-------------------------------------
// HID report descriptor
//-------------------------------------

uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1))
};

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
  (void)itf;
  return desc_hid_report;
}

//-------------------------------------
// Configuration descriptor
//-------------------------------------

uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,                                      // Config number
        1,                                      // Interface count
        0,                                      // String index
        TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN, // Total length
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,     // Attribute
        100                                     // Power in mA
    ),

    TUD_HID_DESCRIPTOR(
        0,                       // Interface number
        0,                       // String index
        HID_ITF_PROTOCOL_NONE,   // Protocol
        sizeof(desc_hid_report), // Report descriptor length
        0x81,                    // Endpoint IN address
        CFG_TUD_HID_EP_BUFSIZE,  // Endpoint size
        5                        // Polling interval
    )
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index; // for multiple configurations
  return desc_configuration;
}

//-------------------------------------
// String descriptors
//-------------------------------------

char pico_serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

// array of pointer to string descriptors
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: Supported language is English (0x0409)
    "Raspberry Pi",             // 1: Manufacturer
    "HID Bridge",               // 2: Product
    pico_serial,                // 3: Serial number using pico's unique board id
};

static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
// NOTE: the 0xEE index string is a Microsoft OS 1.0 Descriptors
// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;

  uint8_t chr_count;

  if (index == 0) {

    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;

  } else {

    if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) {
      return NULL;
    }

    if (index == 3) {
      pico_get_unique_board_id_string(pico_serial, sizeof(pico_serial));
    }

    const char *str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if (chr_count > 31) {
      chr_count = 31;
    }

    // Convert ASCII string into UTF-16
    for (uint8_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

  return _desc_str;
}