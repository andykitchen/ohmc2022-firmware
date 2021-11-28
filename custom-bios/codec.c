#include "codec.h"

#include "util.h"
#include "i2c.h"

int codec_read_txn(int tx) {
    int tx0, tx1;

    tx0 = HI(tx);
    tx1 = LO(tx);

    return i2c_read_txn(CODEC_I2C_ADDR, tx0, tx1, NULL);
}

int codec_write_txn(int tx, int data) {
    int tx0, tx1, d0, d1;

    tx0 = HI(tx);
    tx1 = LO(tx);
    d0  = HI(data);
    d1  = LO(data);

    return i2c_write_txn(CODEC_I2C_ADDR, tx0, tx1, d0, d1, NULL);
}