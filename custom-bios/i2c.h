#ifndef ROCKLING_I2C_H_
#define ROCKLING_I2C_H_

#define ENACK -1
#define EARB  -2
#define EUNK  -16

void i2c_init(void);
int i2c_read_txn(int addr, int tx, int tx2, int *status);
int i2c_general_call_reset(void);

#endif
