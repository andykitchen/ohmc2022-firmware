#!/bin/bash
#
# Write to PCF8574 I2C 8-bit GPIO bidriectional port

csr () {
  wishbone-tool --csr-csv csr.csv "$@" 2>/dev/null | grep -Fv "Exited MemoryAccess thread"
}

# I2C_BUS=i2c0  # Rockling prototype
I2C_BUS=i2c1    # Rockling production

csr ${I2C_BUS}_core_reset 1  # reset I2C block

# clock prescaler formula is apparently (clk_htz / (5 * ${I2C_BUS}_htz)) - 1
# e.g. (12e6 / (5*100e3)) - 1 == 23

csr ${I2C_BUS}_prescale 23
csr ${I2C_BUS}_control  0x80 # enable I2C block

PCF8574_ADDRESS=0x27

pcf8574_write() {
  DATA=$1

  csr ${I2C_BUS}_txr      $[($PCF8574_ADDRESS<<1)+0] # address + write
  csr ${I2C_BUS}_command  0x90  # START and send address
  csr ${I2C_BUS}_status         # print status register
                                # (tells us if response was ACK or NACK)
  csr ${I2C_BUS}_txr      $DATA # data byte 0
  csr ${I2C_BUS}_command  0x10  # send byte
}

while true; do
  pcf8574_write 0x00
  sleep 1
  pcf8574_write 0x01
  sleep 1
done
