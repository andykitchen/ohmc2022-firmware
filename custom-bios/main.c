#include <generated/csr.h>

#include "system.h"
#include "util.h"
#include "rgb.h"
#include "i2c.h"
#include "dac.h"
#include "codec.h"
#include "codec_reg.h"

volatile int debug_status;
volatile int osc0_cycles_low;
volatile int osc0_cycles_high;
volatile int osc1_cycles_low;
volatile int osc1_cycles_high;

static const char *greeting = "Hello World!\n";

void counter_loop(void);

static void post(void);
static void extended_post(void);
static void dac_test_loop(void);
static void all_good(void);
static void nah_mate(void);

static void messible_puts(const char *p);


int main(void) {
	debug_status = 1;

	rgb_init();
	dac_init();
	i2c_init();
	codec_init(); /* needs i2c */

	debug_status = 2;

	messible_puts(greeting);

	debug_status = 3;

	post();
	extended_post();
	busy_wait(2000);
	dac_test_loop();
	counter_loop();

	return 0;
}

static void dac_test_loop(void) {
	int rx;
	while (1) {
		freq_cnt0_ctrl_reset_write(1);
		freq_cnt1_ctrl_reset_write(1);

		rx = dac_set_val(0x00, 0x00);
		if (rx < 0)
			nah_mate();
		busy_wait(2000);

		osc0_cycles_low = freq_cnt0_cycles_read();
		osc1_cycles_low = freq_cnt1_cycles_read();


		freq_cnt0_ctrl_reset_write(1);
		freq_cnt1_ctrl_reset_write(1);

		rx = dac_set_val(0xff, 0xff);
		if (rx < 0)
			nah_mate();
		busy_wait(2000);

		osc0_cycles_high = freq_cnt0_cycles_read();
		osc1_cycles_high = freq_cnt1_cycles_read();
	}
}

/* simple power on self test of i2c devices */
static void post(void) {
	int rx, status;
	int post_failed = 0;

	i2c_general_call_reset();

	busy_wait(20);

	/* ask the DAC for its own address */
	rx = i2c_read_txn(DAC_I2C_ADDR, DAC_ID_CMD, -1, &status);

	debug_status = rx & 0xffff;

	if (LO(rx) != DAC_I2C_ADDR)
		post_failed = 1;

	/* ask the CODEC to identify itself */
	rx = i2c_read_txn(CODEC_I2C_ADDR, 0x00, 0x00, &status);

	if (HI(rx) != CODEC_PARTID)
		post_failed = 1;

	debug_status |= (rx & 0xffff) << 16;


	i2c_general_call_reset();

	if (!post_failed) {
		all_good();
	} else {
		nah_mate();
	}
}

/* more intensive tests where things are actually written i2c registers */
static void extended_post(void) {
	int rx;
	int post_failed = 0;

	/* Write to 0x00 and 0x01 volatile DAC registers
	   and read back to compare values */
	for (int addr = 0; addr < 0x02; addr++) {
		static const int val = 0x6e;
		dac_latch_out_write(1);
		rx = dac_write_txn(addr, val);
		if (rx < 0)
			post_failed = 1;

		/* NOTE: You do not read back the last written value until it is latched */
		dac_latch_out_write(0);
		rx = dac_read_txn(addr);
		if (rx != val)
			post_failed = 1;
	}
	dac_latch_out_write(0);

	/* Write to the CODEC volume register and
	   read back to compare values */
	static const int val = 0x0001;

	rx = codec_write_txn(CHIP_ANA_ADC_CTRL, val);
	if (rx < 0)
		post_failed = 1;

	rx = codec_read_txn(CHIP_ANA_ADC_CTRL);
	if (rx != val)
		post_failed = 1;

	rx = codec_write_txn(CHIP_ANA_ADC_CTRL, 0x0000);
	if (rx < 0)
		post_failed = 1;

	i2c_general_call_reset();

	if (!post_failed) {
		all_good();
	} else {
		nah_mate();
	}
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
