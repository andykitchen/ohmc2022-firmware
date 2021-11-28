#!/bin/bash

# Hot reloading requires the "ROM" to be read-writable
# to build a compatible bitstream use the following command:
# $ make BITSTREAM_FLAGS="--rw-rom"

BIOS_BIN=build/rockling/software/bios/bios.bin

# Hold CPU in reset
wishbone-tool --csr-csv csr.csv ctrl_reset 0x02

# Reload binary into "ROM"
# NOTE: this produces big red error messages, but seems to work none the less.
wishbone-tool --server=load-file --load-name $BIOS_BIN --load-address 0x00000000

# Release CPU reset line
wishbone-tool --csr-csv csr.csv ctrl_reset 0x00
