#!/bin/bash

# This script tries to read a register on the attached CODEC
# see linked datasheet:
# page 24, table 13 "Read Single Location" and
# page 30, table 16 "CHIP_ID 0x0000" 
# https://www.nxp.com/docs/en/data-sheet/SGTL5000.pdf

csr () {
	wishbone-tool --csr-csv csr.csv "$@" 2>/dev/null | grep -Fv "Exited MemoryAccess thread"
}

csr i2c_core_reset 1  # reset i2c block

# clock prescaler formula is apparently (clk_htz / (5*i2c_htz)) - 1
# e.g. (12e6 / (5*100e3)) - 1 == 23
csr i2c_prescale 23
csr i2c_control  0x80 # enable i2c block

addr=0x0A

csr i2c_txr      $[($addr<<1)+0] # address + write
csr i2c_command  0x90 # START and send address
csr i2c_status        # print status register (has a bit for ACK or NACK)
csr i2c_txr      0x00 # data byte 1
csr i2c_command  0x10 # send byte
csr i2c_txr      0x00 # data byte 0
csr i2c_command  0x10 # send byte
csr i2c_txr      $[($addr<<1)+1] # address + read bit
csr i2c_command  0x90 # RESTART and send address command
csr i2c_command  0x20 # read with ACK
csr i2c_rxr           # print first byte read
csr i2c_command  0x68 # read with NACK and STOP
csr i2c_rxr           # print second byte read
