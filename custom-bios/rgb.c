#include <generated/csr.h>

#include "rgb.h"

#include "util.h"

enum led_registers {
	LEDDCR0  = 8,
	LEDDBR   = 9,
	LEDDONR  = 10,
	LEDDOFR  = 11,
	LEDDBCRR = 5,
	LEDDBCFR = 6,
	LEDDPWRR = 1,
	LEDDPWRG = 2,
	LEDDPWRB = 3,
};

#define BREATHE_ENABLE (1 << 7)
#define BREATHE_EDGE_ON (0 << 6)
#define BREATHE_EDGE_BOTH (1 << 6)
#define BREATHE_MODE_MODULATE (1 << 5)
#define BREATHE_MODE_FIXED (0 << 5)

// Breathe rate is in 128 ms increments
#define BREATHE_RATE_MS(x) ((((x)+1) / 128 & 7) << 0)

// Blink on/off time is in 32 ms increments
#define BLINK_TIME_MS(x) (((x)) / 32)

#define LEDDEN (1 << 7)
#define FR250 (1 << 6)
#define OUTPUL (1 << 5)
#define OUTSKEW (1 << 4)
#define QUICK_STOP (1 << 3)
#define PWM_MODE_LFSR (1 << 2)
#define PWM_MODE_LINEAR (0 << 2)


static void rgb_write(uint8_t value, uint8_t addr) {
	rgb_addr_write(addr);
	rgb_dat_write(value);
}

void NOINLINE rgb_init(void) {
	// Turn on the RGB block and current enable, as well as enabling led control
	rgb_ctrl_write((1 << 0) | (1 << 1) | (1 << 2));

	// Enable the LED driver, and set 250 Hz mode.
	// Also set quick stop, which we'll use to switch patterns quickly.
	rgb_write(LEDDEN | FR250 | QUICK_STOP, LEDDCR0);

	// Set clock register to 12 MHz / 64 kHz - 1
	rgb_write((12000000/64000)-1, LEDDBR);

	rgb_write(BLINK_TIME_MS(32), LEDDONR);  // Amount of time to stay "on"
	rgb_write(BLINK_TIME_MS(0),  LEDDOFR);   // Amount of time to stay "off"

	rgb_write(BREATHE_ENABLE | BREATHE_MODE_FIXED | BREATHE_RATE_MS(256), LEDDBCRR);
	rgb_write(BREATHE_ENABLE | BREATHE_MODE_FIXED | BREATHE_RATE_MS(256), LEDDBCFR);
}

void NOINLINE rgb_set(uint8_t r, uint8_t g, uint8_t b) {
	// On the production rockling the RGB LED colours are wired funny
	rgb_write(r, LEDDPWRR);
	rgb_write(b, LEDDPWRG);
	rgb_write(g, LEDDPWRB);
}

// The amount of time to stay off or on
void rgb_on_time(uint8_t ms) {
	rgb_write(BLINK_TIME_MS(ms), LEDDONR);  // Amount of time to stay "on"
}

void rgb_off_time(uint8_t ms) {
	rgb_write(BLINK_TIME_MS(ms), LEDDOFR);   // Amount of time to stay "off"
}

// The amount of time to breathe in/out
void rgb_in_time(uint8_t ms) {
	rgb_write(BREATHE_ENABLE | BREATHE_MODE_FIXED | BREATHE_RATE_MS(ms), LEDDBCRR);
}

void rgb_out_time(uint8_t ms) {
	rgb_write(BREATHE_ENABLE | BREATHE_MODE_FIXED | BREATHE_RATE_MS(ms), LEDDBCFR);
}
