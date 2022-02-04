#ifndef ROCKLING_CODEC_H_
#define ROCKLING_CODEC_H_

#define CODEC_I2C_ADDR    0x0A
#define CODEC_PARTID      0xA0

int codec_init(void);

int codec_read_txn(int tx);
int codec_write_txn(int tx, int data);

#endif
