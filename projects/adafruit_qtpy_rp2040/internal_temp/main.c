#include <stdio.h>

#include "boards/adafruit_qtpy_rp2040.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

/* References for this implementation:
 * raspberry-pi-pico-c-sdk.pdf, Section '4.1.1. hardware_adc'
 * pico-examples/adc/adc_console/adc_console.c
 */
float read_onboard_temperature() {
  /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
  const float conversionFactor = 3.3f / (1 << 12);
  float adc = (float)adc_read() * conversionFactor;
  float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
  return tempC;
}

int main() {
  stdio_init_all();

  adc_init();

  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  while (true) {
    float temp = read_onboard_temperature();
    printf("Onboard temperature = %.02f C\n", temp);
    sleep_ms(1000);
  }
}