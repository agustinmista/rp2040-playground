#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "boards/pico.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tusb.h"

//-------------------------------------
// USB device callbacks
//-------------------------------------

// Invoked when device is mounted
void tud_mount_cb(void) {
  printf("[device] device mounted\n");
  ; // TODO
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  printf("[device] device unmounted\n");
  ; // TODO
}

// Invoked when USB bus is suspended
// remote_wakeup_en: if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;

  printf("[device] USB bus suspended\n");
  ; // TODO
}

// Invoked when USB bus is resumed
void tud_resume_cb(void) {
  printf("[device] USB bus resumed\n");
  ; // TODO
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length
// Return zero will cause the stack to stall the request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  printf("[device] received GET_REPORT request\n");
  ; // TODO

  return 0;
}

// Invoked when received SET_REPORT control request or received data on OUT
// endpoint (Report ID = 0, Type = 0)
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;

  printf("[device] received SET_REPORT request or data on OUT endpoint\n");
  ; // TODO
}

//-------------------------------------
// USB host callbacks
//-------------------------------------

// Invoked when device with hid interface is mounted
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be
// skipped therefore desc_report = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  (void)dev_addr;
  (void)instance;
  (void)desc_report;
  (void)desc_len;

  printf("[host] device mounted\n");
  ; // TODO
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  (void)dev_addr;
  (void)instance;

  printf("[host] device unmounted\n");
  ; // TODO
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  (void)dev_addr;
  (void)instance;
  (void)report;
  (void)len;

  printf("[host] received report\n");
  ; // TODO
}

//-------------------------------------
// core0: handle USB device events
//-------------------------------------

void core0_main() {
  printf("[core0] started\n");

  // Init device stack on native USB port
  printf("[core0] initializing USB device stack\n");
  tud_init(BOARD_TUD_RHPORT);

  // Enter core0 loop
  printf("[core0] entering loop\n");
  while (true) {
    tud_task();
  }
}

//-------------------------------------
// core1: handle USB host events
//-------------------------------------

void core1_main() {
  printf("[core1] started\n");

  // Init host stack on simulated USB port
  printf("[core1] initializing USB host stack\n");
  tuh_init(BOARD_TUH_RHPORT);

  // Enter core1 loop
  printf("[core0] entering loop\n");
  while (true) {
    tuh_task();
  }
}

//-------------------------------------
// main
//-------------------------------------

int main(void) {
  sleep_ms(100); // Don't do anything while the debugger restarts the board

  set_sys_clock_khz(120000, true); // sys_clock must be a multiple of 12MHz
  sleep_ms(10);

  stdio_uart_init();

  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  core0_main();
  return 0;
}