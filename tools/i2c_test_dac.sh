#!/bin/bash

# This script tries to read a register on the DAC

# see linked datasheet:
# page 79, figure 7-4 "Read Command – Single Memory Address" and 
# page 50, table 4-1 "MCP47CXBXX MEMORY MAP"
# https://ww1.microchip.com/downloads/en/DeviceDoc/MCP47CXBXX-Data-Sheet-DS20006089B.pdf

csr () {
	wishbone-tool --csr-csv csr.csv "$@" 2>/dev/null | grep -Fv "Exited MemoryAccess thread"
}

csr i2c_core_reset 1  # reset i2c block

# clock prescaler formula is apparently (clk_htz / (5*i2c_htz)) - 1
# e.g. (12e6 / (5*100e3)) - 1 == 23
csr i2c_prescale 23
csr i2c_control  0x80 # enable i2c block

addr=0x62

csr i2c_txr      $[($addr<<1)+0] # address + write
csr i2c_command  0x90 # START and send address command
csr i2c_status        # print status register (has a bit for ACK or NACK)
csr i2c_txr      0xd6 # first data byte to send
csr i2c_command  0x10 # send data byte command
csr i2c_txr      $[($addr<<1)+1] # address + read bit
csr i2c_command  0x90 # RESTART and send address command
csr i2c_command  0x20 # read with ACK
csr i2c_rxr           # print first byte read
csr i2c_command  0x68 # read with NACK and STOP
csr i2c_rxr           # print second byte read
