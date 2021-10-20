#!/bin/bash

csr () {
	wishbone-tool --csr-csv build/csr.csv "$@"
}

csr rgb_ctrl 0x7
csr rgb_addr 0x8
csr rgb_dat 0xc8
csr rgb_addr 0x9
csr rgb_dat 0xba
csr rgb_addr 0xa
csr rgb_dat 0x1
csr rgb_addr 0xb
csr rgb_dat 0x0
csr rgb_addr 0x5
csr rgb_dat 0x82
csr rgb_addr 0x6
csr rgb_dat 0x82
csr rgb_addr 0x1
csr rgb_dat 0x0
csr rgb_addr 0x2
csr rgb_dat 0x14
csr rgb_addr 0x3
csr rgb_dat 0xff
