#ifndef ROCKLING_DAC_H_
#define ROCKLING_DAC_H_

#define DAC_I2C_ADDR  0x60

#define DAC_ID_REG      0x1A
#define DAC_STATUS_REG  0x0A

#define DAC_ID_CMD      ((DAC_ID_REG<<3)|0x06)

void dac_init(void);

int dac_read_txn(int reg);
int dac_write_txn(int reg, int data);

int dac_set_val(int v0, int v1);

#endif
