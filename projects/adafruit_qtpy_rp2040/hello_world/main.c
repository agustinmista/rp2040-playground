#include <stdio.h>

#include "boards/adafruit_qtpy_rp2040.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();

  while (true) {
    printf("Hello, world!\n");
    sleep_ms(1000);
  }
}