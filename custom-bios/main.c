#include <generated/csr.h>

#include "util.h"
#include "rgb.h"
#include "i2c.h"
#include "i2c_addr.h"

volatile int debug_status;

static const char *greeting = "Hello World!\n";

void counter_loop(void);

static void post(void);
static void all_good(void);
static void nah_mate(void);

static void messible_puts(const char *p);


int main(void) {
	debug_status = 1;

	rgb_init();
	i2c_init();

	debug_status = 2;

	messible_puts(greeting);

	debug_status = 3;

	post();

	counter_loop();

	return 0;
}

/* somple power on self test of i2c devices */
static void post(void) {
	int rx, status;
	int post_failed = 0;

	/* ask the DAC for it's own address */
	rx = i2c_read_txn(DAC_I2C_ADDR, 0xd6, -1, &status);

	debug_status = rx & 0xffff;

	if (BYTE0(rx) != DAC_I2C_ADDR) {
		nah_mate();
		post_failed = 1;
	}

	/* ask the CODEC to identify itself */
	rx = i2c_read_txn(CODEC_I2C_ADDR, 0x00, 0x00, &status);

	if (BYTE1(rx) != CODEC_PARTID) {
		nah_mate();
		post_failed = 1;
	}

	debug_status |= (rx & 0xffff) << 16;

	if (!post_failed)
		all_good();
}

static void all_good(void) {
	int r, g, b;

	r = 0x00;
	g = 0x00;
	b = 0xff;
	rgb_set(r, g, b);
}

static void nah_mate(void) {
	int r, g, b;

	r = 0xff;
	g = 0x00;
	b = 0x00;
	rgb_set(r, g, b);
}

static void NOINLINE messible_puts(const char *p) {
	char c;

	while (c = *p++)
		messible_in_write(c);
}
