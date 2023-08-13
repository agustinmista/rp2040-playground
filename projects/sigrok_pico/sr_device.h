#include "stdarg.h"
#include <string.h>

// ------------------------------------
// Pin usage:
// * GP0-GP1 are reserved for debug UART
// * GP2-GP22 are digital inputs
// * GP26-28 are analog inputs
// ------------------------------------

// Number of digital channels
#define NUM_DIGITAL_CHANNELS 21

// Number of analog channels
#define NUM_ANALOG_CHANNELS 3

// Mask of bits 22:2 to use as inputs
#define GPIO_DIGITAL_MASK 0x7FFFFC

// Storage size of the DMA buffer. The buffer is split into two halves so that
// when the first buffer fills we can send the trace data serially while the
// other buffer is DMA'd into.
#define DMA_BUFFER_SIZE 220000

// The size of the buffer sent to the CDC serial. The TUD CDC buffer is only
// 256B so it doesn't help to have more than this.
#define TX_BUFFER_SIZE 260

// This sets the point which we will send data from the txbuf to the USB CDC.
// For the 5-21 channel RLE, it must leave a spare ~83 entries to cover the case
// where a new long steady input comes after deciding to not send a sample.
// (Assuming 128KB samples per half, a max RLE value of 1568 we can get
// 256*1024/2/1568=83 max length RLEs on a steady input). Other than that, the
// value is not very specific because the usb tub code implements a 256 entry
// fifo that queues things up and sends max length 64B transactions. The value
// 20 is arbitrarly picked to ensure that if we have even a little we send it so
// that at least something goes across the link.
#define TX_BUFFER_THRESHOLD 20

// The baud rate used to communicate with the host.
#define UART_BAUD 921600

// Base value of sys_clk in KHz. Must be less than 125Mhz per RP2040 spec and a
// multiple of 24Mhz to support integer divisors of the PIO clock and ADC clock.
#define SYS_CLK_BASE 120000

// Boosted sys_clk in KHz. Runs the RP2040 above its specifed frequency limit to
// support faster processing of digital run length encoding, which in some cases
// may allow for faster streaming of digital only data. Frequency must be a
// 24Mhz multiple and less than 300Mhz to avoid known issues The authors PICO
// failed at 288Mhz, but testing with 240Mhz seemed reliable.

// Use this at your own risk!
// #define SYS_CLK_BOOST_ENABLE 1
// #define SYS_CLK_BOOST_FREQ 240000

// ------------------------------------
// Helpers

#define DEBUG_PRINTF_BUFFER_SIZE 256

// Debug printf to UART
int debug_printf(const char *fmt, ...) {

  va_list argptr;
  int len = 1;
  char out_str[256];

  memset(&out_str, 0x00, sizeof(out_str));
  va_start(argptr, fmt);
  len = vsprintf(out_str, fmt, argptr);
  va_end(argptr);

  if ((len > 0) && (len < DEBUG_PRINTF_BUFFER_SIZE - 1)) {
    uart_puts(uart0, out_str);
    uart_tx_wait_blocking(uart0);
  } else {
    uart_puts(uart0, "UART OVERFLOW");
    uart_tx_wait_blocking(uart0);
  }
  return len;
}

// ------------------------------------
// Sigrok device

typedef struct sigrok_device {
  uint32_t a_mask;           // ???
  uint32_t a_size;           // Size of the analog data buffer
  uint32_t d_mask;           // ???
  uint32_t d_size;           // Size of the digital data buffer
  uint32_t num_samples;      // Number of samples to measure
  uint32_t sample_rate;      // Sample rate of the device in Hz
  uint32_t samples_per_half; // Number of samples for one of the 4 dma target arrays
  uint32_t sent_cnt;         // Number of samples sent
  uint8_t a_chan_cnt;        // Count of enabled analog channels
  uint8_t d_chan_cnt;        // Count of enabled digital channels
  uint8_t d_nps;             // Digital nibbles per slice from a PIO/DMA perspective
  uint8_t d_tx_bps;          // Digital transmit bytes per slice
  uint8_t pin_count;         // Pins sampled by the PIO (4,8,16 or 32)

  uint32_t dbuf0_start; // Starting memory pointers of buffers
  uint32_t dbuf1_start; //
  uint32_t abuf0_start; //
  uint32_t abuf1_start; //

  char cmdstr[20]; // Used for parsing commands input
  char cmdstrptr;  // Index within the input command buffer

  char rspstr[20]; // Used for storing commands output

  volatile bool started;    // Started flag
  volatile bool sending;    // Sending flag
  volatile bool aborted;    // Aborted flag
  volatile bool continuous; // Continuous mode flag
} sigrok_device_t;

// Reset as part of init, or on a completed send
void reset(sigrok_device_t *d) {
  d->started = false;
  d->sending = 0;
  d->aborted = false;
  d->continuous = 0;
  d->sent_cnt = 0;
};

// Initial post reset state
void init(sigrok_device_t *d) {
  reset(d);
  d->a_mask = 0;
  d->d_mask = 0;
  d->sample_rate = 5000;
  d->num_samples = 10;
  d->a_chan_cnt = 0;
  d->d_nps = 0;
  d->cmdstrptr = 0;
}

// Initialize the the transmission
void tx_init(sigrok_device_t *d) {

  // A reset should have already been called to restart the device. An
  // additional one here would clear trigger and other state that had been
  // updated.
  d->a_chan_cnt = 0;
  for (int i = 0; i < NUM_ANALOG_CHANNELS; i++) {
    if (((d->a_mask) >> i) & 1) {
      d->a_chan_cnt++;
    }
  }

  // Nibbles per slice controls how PIO digital data is stored. Only supports
  // 0,1,2,4 or 8, which use 0,4,8,16 or 32 bits of PIO fifo data per sample
  // clock.
  d->d_nps = (d->d_mask & 0xF) ? 1 : 0;
  d->d_nps = (d->d_mask & 0xF0) ? (d->d_nps) + 1 : d->d_nps;
  d->d_nps = (d->d_mask & 0xFF00) ? (d->d_nps) + 2 : d->d_nps;
  d->d_nps = (d->d_mask & 0xFFFF0000) ? (d->d_nps) + 4 : d->d_nps;

  // Dealing with samples on a per nibble (rather than per byte) basis in non D4
  // mode creates a bunch of annoying special cases, so forcing non D4 mode to
  // always store a minimum of 8 bits.
  if ((d->d_nps == 1) && (d->a_chan_cnt > 0)) {
    d->d_nps = 2;
  }

  // Digital channels must enable from D0 and go up, but that is checked by the
  // host.
  d->d_chan_cnt = 0;
  for (int i = 0; i < NUM_DIGITAL_CHANNELS; i++) {
    if (((d->d_mask) >> i) & 1) {
      d->d_chan_cnt++;
    }
  }

  // Set the device baud rate.
  d->d_tx_bps = (d->d_chan_cnt + 6) / 7;

  // Enable sending mode.
  d->sending = true;
}

// Process an incomming command. Returns 1 if the device rspstr has a response
// to send to host. Be sure that rspstr does not have '\n' or '\r'.
int process_cmd(sigrok_device_t *d, char c) {
  int tmpint, tmpint2, ret;

  d->cmdstr[d->cmdstrptr] = 0;
  switch (d->cmdstr[0]) {

  case 'i':
    sprintf(d->rspstr, "SRPICO,A%02d1D%02d,02", NUM_ANALOG_CHANNELS, NUM_DIGITAL_CHANNELS);
    debug_printf("ID rsp %s\n\r", d->rspstr);
    ret = 1;
    break;

  case 'R':
    tmpint = atol(&(d->cmdstr[1]));
    if ((tmpint >= 5000) && (tmpint <= 120000016)) { // Add 16 to support cfg_bits
      d->sample_rate = tmpint;
      ret = 1;
    } else {
      debug_printf("unsupported smp rate %s\n\r", d->cmdstr);
      ret = 0;
    }
    break;

  // sample limit
  case 'L':
    tmpint = atol(&(d->cmdstr[1]));
    if (tmpint > 0) {
      d->num_samples = tmpint;
      ret = 1;
    } else {
      debug_printf("bad num samples %s\n\r", d->cmdstr);
      ret = 0;
    }
    break;

  case 'a':
    tmpint = atoi(&(d->cmdstr[1])); // extract channel number
    if (tmpint >= 0) {
      // scale and offset are both in integer uVolts
      // separated by x
      sprintf(d->rspstr, "25700x0"); // 3.3/(2^7) and 0V offset
      ret = 1;
    } else {
      debug_printf("bad ascale %s\n\r", d->cmdstr);
      ret = 1; // this will return a '*' causing the host to fail
    }
    break;

  case 'F': // fixed set of samples
    debug_printf("STRT_FIX\n\r");
    tx_init(d);
    d->continuous = 0;
    ret = 0;
    break;

  case 'C': // continous mode
    tx_init(d);
    d->continuous = 1;
    debug_printf("STRT_CONT\n\r");
    ret = 0;
    break;

  case 't': // trigger -format tvxx where v is value and xx is two digit channel
    ret = 1;
    break;

  case 'p': // pretrigger count
    tmpint = atoi(&(d->cmdstr[1]));
    debug_printf("Pre-trigger samples %d cmd %s\n\r", tmpint, d->cmdstr);
    ret = 1;
    break;

  // format is Axyy where x is 0 for disabled, 1 for enabled and yy is channel #
  case 'A':                          /// enable analog channel always a set
    tmpint = d->cmdstr[1] - '0';     // extract enable value
    tmpint2 = atoi(&(d->cmdstr[2])); // extract channel number
    if ((tmpint >= 0) && (tmpint <= 1) && (tmpint2 >= 0) && (tmpint2 <= 31)) {
      d->a_mask = d->a_mask & ~(1 << tmpint2);
      d->a_mask = d->a_mask | (tmpint << tmpint2);
      // debug_printf("A%d EN %d Msk 0x%X\n\r",tmpint2,tmpint,d->a_mask);
      ret = 1;
    } else {
      ret = 0;
    }
    break;

    // format is Dxyy where x is 0 for disabled, 1 for enabled and yy is channel #
  case 'D':                          /// enable digital channel always a set
    tmpint = d->cmdstr[1] - '0';     // extract enable value
    tmpint2 = atoi(&(d->cmdstr[2])); // extract channel number
    if ((tmpint >= 0) && (tmpint <= 1) && (tmpint2 >= 0) && (tmpint2 <= 31)) {
      d->d_mask = d->d_mask & ~(1 << tmpint2);
      d->d_mask = d->d_mask | (tmpint << tmpint2);
      // debug_printf("D%d EN %d Msk 0x%X\n\r",tmpint2,tmpint,d->d_mask);
      ret = 1;
    } else {
      ret = 0;
    }
    break;

  default:
    debug_printf("bad command %s\n\r", d->cmdstr);
    ret = 0;
  }

  d->cmdstrptr = 0;
  return ret;
}

// Process incoming character stream. Returns 1 if the device rspstr has a
// response to send to host. Be sure that rspstr does not have '\n' or '\r'.
int process_char(sigrok_device_t *d, char c) {
  int ret;

  // Set default rspstr for all commands that have a dataless ack
  d->rspstr[0] = '*';
  d->rspstr[1] = 0;

  // The reset character works by itself
  if (c == '*') {
    reset(d);
    debug_printf("RST* %d\n\r", d->sending);
    ret = 0;
  } else if ((c == '\r') || (c == '\n')) {
    ret = process_cmd(d, c);
  } else { // no CR/LF
    if (d->cmdstrptr >= 19) {
      d->cmdstr[18] = 0;
      debug_printf("Command overflow %s\n\r", d->cmdstr);
      d->cmdstrptr = 0;
    }
    d->cmdstr[d->cmdstrptr++] = c;
    ret = 0;
  }

  // Returning 0 means to not send any kind of response
  return ret;
}
