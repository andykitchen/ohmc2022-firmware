#!/bin/bash
#
# Scan I2C devices on the bus
#
# This will only work on the ...
# - Rockling prototype  using I2C bus 0
# - Rockling production using I2C bus 1

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

# DEVICES="`seq 1 127`"      # All potential I2C device addresses
DEVICES="10 39 96 97 98 99"  # Known Rockling I2C device addresses

for i in $DEVICES; do
  printf "Address: 0x%x\n" $i
  csr ${I2C_BUS}_txr      $[($i<<1)+0] # address + write
  csr ${I2C_BUS}_command  0x90 # START and send address command
  csr ${I2C_BUS}_status        # print status register
                               # (tells us if response was ACK or NACK)
  csr ${I2C_BUS}_txr      0x00 # load one byte of zeros into send register
  csr ${I2C_BUS}_command  0x50 # send byte and STOP
done
