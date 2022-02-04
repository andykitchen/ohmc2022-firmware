#include <generated/csr.h>

#include "dac.h"

#include "system.h"
#include "util.h"
#include "i2c.h"


/* initialise the DAC subsystem
 */
void dac_init(void) {
	/* Just hold DAC latch low because we don't need
	   high precision coordination between the channels */
	dac_latch_out_write(0);
}

/* read DAC i2c register
 * @reg address of register to read
 *
 * @returns value read or negative i2c error status code
 */
int dac_read_txn(int reg) {
	int tx;

	/* tx data byte format for read is `aaaaa11x`
	   where a is address bits and x is unused
	   see Figure 7-1 page 75 in the MCP47CXBXX datasheet */
	tx = (reg << 3) | 0x06;

	return i2c_read_txn(DAC_I2C_ADDR, tx, -1, NULL);
}

/* write DAC i2c register
 * @reg  address of register to read
 * @data 16-bit value to write to register
 *
 * @returns zero or negative i2c error status code
 */
int dac_write_txn(int reg, int data) {
	int tx, d0, d1;

	d0 = HI(data);
	d1 = LO(data);

	/* tx data byte format for write `aaaaa00x`
	   where a is address bits and x is unused
	   see Figure 7-1 page 75 in the MCP47CXBXX datasheet */
	tx = (reg << 3);

	return i2c_write_txn(DAC_I2C_ADDR, tx, -1, d0, d1, NULL);
}

/* set DAC output voltage levels
 * @v0 value for DAC channel 0 (only set if greater than 0)
 * @v1 value for DAC channel 1 (only set if greater than 0)
 *
 * @returns zero or negative i2c error status code
 */
int dac_set_val(int v0, int v1) {
	/* Writes i2c register address 0x00 and 0x01 */
	int ret;

	if (v0 >= 0) {
		ret = dac_write_txn(0x00, v0);
		if (ret < 0) return ret;
	}

	if (v1 >= 0) {
		ret = dac_write_txn(0x01, v1);
		if (ret < 0) return ret;
	}

	return 0;
}
