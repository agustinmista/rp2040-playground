#include <stdio.h>

#include "cytron_maker_pi_rp2040.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"

// Set some NeoPixel pins and stuff
#define NEOPIXEL_DATA_PIN PICO_DEFAULT_WS2812_PIN
#define NEOPIXEL_NUM_PIXELS PICO_DEFAULT_WS2812_NUM_PIXELS
#define NEOPIXEL_IS_RGBW true

// The PIO state machine we will use to communicate with the NeoPixel
uint sm = -1;

// Choose which PIO instance to use (there are two instances)
PIO pio = pio0;

// Flip the bits around from RGB to WBGR with W=0
static inline uint32_t rgb_to_wbgr(uint8_t r, uint8_t g, uint8_t b) {
  return (((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b)) << 8u;
}

// Send an RGB color to the onboard NeoPixel
static inline void onboard_neopixel_put_rgb(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t data = rgb_to_wbgr(r, g, b);
  printf("Sending (%d, %d, %d)", r, g, b);
  for (int i = 0; i < NEOPIXEL_NUM_PIXELS; i++) {
    pio_sm_put_blocking(pio, sm, data);
  }
}

// Inialize the single onboard NeoPixel
void init_onboard_neopixel() {
  // Our assembled program needs to be loaded into this PIO's instruction
  // memory. This SDK function will find a location (offset) in the instruction
  // memory where there is enough space for our program. We need to remember
  // this location!
  uint offset = pio_add_program(pio, &ws2812_program);

  // Find a free state machine on our chosen PIO (erroring if there are none).
  // Configure it to run our program, and start it, using the helper function we
  // included in our .pio file.
  sm = pio_claim_unused_sm(pio, true);

  // Call the PIO state machine program initializer
  ws2812_program_init(pio, sm, offset, NEOPIXEL_DATA_PIN, 800000, NEOPIXEL_IS_RGBW);
}

int main() {
  stdio_init_all();
  init_onboard_neopixel();

  while (true) {
    onboard_neopixel_put_rgb(127, 0, 0);
    sleep_ms(500);

    onboard_neopixel_put_rgb(0, 127, 0);
    sleep_ms(500);

    onboard_neopixel_put_rgb(0, 0, 127);
    sleep_ms(500);
  }
}