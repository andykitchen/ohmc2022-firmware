#include "dac.h"

#include "util.h"
#include "i2c.h"

int dac_read_txn(int tx) {
    return i2c_read_txn(DAC_I2C_ADDR, tx, -1, NULL);
}

int dac_write_txn(int tx, int data) {
    int d0, d1;

    d0 = HI(data);
    d1 = LO(data);

    return i2c_write_txn(DAC_I2C_ADDR, tx, -1, d0, d1, NULL);
}
