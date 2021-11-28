#ifndef ROCKLING_CODEC_H_
#define ROCKLING_CODEC_H_

#define CODEC_I2C_ADDR 0x0A
#define CODEC_PARTID   0xA0

#define CHIP_ID           0x0000
#define CHIP_ANA_ADC_CTRL 0x0020

int codec_read_txn(int tx);
int codec_write_txn(int tx, int data);

#endif
