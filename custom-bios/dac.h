#ifndef ROCKLING_DAC_H_
#define ROCKLING_DAC_H_

#define DAC_I2C_ADDR  0x60
#define DAC_ID_CMD    0xd6

void dac_init(void);

int dac_read_txn(int tx);
int dac_write_txn(int tx, int data);

int dac_set_val(int v0, int v1);

#endif
