#include "stdint.h"

#include "codec.h"
#include "codec_reg.h"

#include "util.h"
#include "i2c.h"

/* This struct represents a codec initialisation step */
struct codec_init_step {
	uint16_t reg;   /* register address to write */
	uint16_t clear; /* bits to clear (reg &= !x) */
	uint16_t set;   /* bits to set   (reg |=  x) */
};
/* N.B. Clearing happens before setting so we can write a specific value to a multibit field
   by clearing the whole field then setting the desired bit pattern */

static const struct codec_init_step init_steps[] = {
	{CHIP_ANA_POWER,     .clear = LINREG_SIMPLE_POWERUP | STARTUP_POWERUP },
	{CHIP_ANA_POWER,     .set   = VDDC_CHRGPMP_POWERUP },
	{CHIP_LINREG_CTRL,   .set   = VDDC_ASSN_OVRD | VDDC_MAN_ASSN },
	{CHIP_REF_CTRL,      .clear = 0x01F0, .set = 0x01F0 },
	{CHIP_LINE_OUT_CTRL, .clear = 0x003F, .set = 0x0022 },
	{CHIP_LINE_OUT_CTRL, .clear = 0x0F00, .set = 0x0300 },
};

/* initialise audio CODEC
 *
 * @returns zero or negative i2c error status code
 */
NOINLINE int codec_init(void) {
	int ret, val;
	struct codec_init_step init;

	/* loop through init steps clearing and setting required register bits */
	for (int i = 0; i < ARRAY_LEN(init_steps); i++) {
		init = init_steps[i];

		ret = codec_read_txn(init.reg);

		if (ret < 0)
			return val;

		val = ret;
		val &= ~init.clear;
		val |= init.set;

		ret = codec_write_txn(init.reg, val);

		if (ret < 0)
			return val;
	}

	return 0;
}

/* read CODEC i2c register
 * @tx address of register to read
 *
 * @returns value read or negative i2c error status code
 */
int codec_read_txn(int tx) {
	int tx0, tx1;

	tx0 = HI(tx);
	tx1 = LO(tx);

	return i2c_read_txn(CODEC_I2C_ADDR, tx0, tx1, NULL);
}

/* read CODEC i2c register
 * @tx   address of register to write
 * @data 16-bit value to write to register
 *
 * @returns zero or negative i2c error status code
 */
int codec_write_txn(int tx, int data) {
	int tx0, tx1, d0, d1;

	tx0 = HI(tx);
	tx1 = LO(tx);
	d0  = HI(data);
	d1  = LO(data);

	return i2c_write_txn(CODEC_I2C_ADDR, tx0, tx1, d0, d1, NULL);
}
