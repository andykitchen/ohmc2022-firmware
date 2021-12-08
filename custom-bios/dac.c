#include <generated/csr.h>

#include "dac.h"

#include "util.h"
#include "i2c.h"


/* initialise the DAC subsystem
 */
void dac_init(void) {
    /* DAC latch pin should be held high */
    dac_latch_out_write(1);
}

/* read DAC i2c register
 * @tx address of register to read
 *
 * @returns value read or negative i2c error status code
 */
int dac_read_txn(int tx) {
    return i2c_read_txn(DAC_I2C_ADDR, tx, -1, NULL);
}

/* write DAC i2c register
 * @tx   address of register to read
 * @data 16-bit value to write to register
 *
 * @returns value read or negative i2c error status code
 */
int dac_write_txn(int tx, int data) {
    int d0, d1;

    d0 = HI(data);
    d1 = LO(data);

    return i2c_write_txn(DAC_I2C_ADDR, tx, -1, d0, d1, NULL);
}

/* set both values of DAC simultanously
 * @v0 value for DAC channel 0 (only set if greater than 0)
 * @v2 value for DAC channel 1 (only set if greater than 0)
 *
 * @returns i2c error status code
 */
int dac_set_val(int v0, int v1) {
    /*
    Writes i2c register address 0x00 and 0x01 then
    pulses the DAC latch pin low to cause the DAC
    voltages to change.
    */
    int ret;

    if (v0 >= 0) {
        ret = dac_write_txn(0x00, v0);
        if (ret < 0) return ret;
    }

    if (v1 >= 0) {
        ret = dac_write_txn(0x01, v1);
        if (ret < 0) return ret;
    }

    /* pulse DAC latch pin low to actually change voltages */
    dac_latch_out_write(0);
    dac_latch_out_write(1);

    return 0;
}
