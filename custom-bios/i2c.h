#ifndef ROCKLING_I2C_H_
#define ROCKLING_I2C_H_

void i2c_init(void);
int i2c_read_char(int addr, int tx, int *rx);

#endif
