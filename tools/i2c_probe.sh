#!/bin/bash

# This script tries to sequentially probe every i2c device on the bus.

csr () {
	wishbone-tool --csr-csv csr.csv "$@" 2>/dev/null | grep -Fv "Exited MemoryAccess thread"
}

csr i2c1_core_reset 1  # reset i2c block

# clock prescaler formula is apparently (clk_htz / (5*i2c_htz)) - 1
# e.g. (12e6 / (5*100e3)) - 1 == 23
csr i2c1_prescale 23
csr i2c1_control  0x80 # enable i2c block

for i in `seq 0 127`; do
	printf "Address: 0x%x\n" $i
	csr i2c1_txr      $[($i<<1)+0] # address + write
	csr i2c1_command  0x90 # START and send address command
	csr i2c1_status        # print status register (tells us if response was ACK or NACK)
	csr i2c1_txr      0x00 # load one byte of zeros into send register
	csr i2c1_command  0x50 # send byte and STOP
done
