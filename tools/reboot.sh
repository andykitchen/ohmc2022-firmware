#!/bin/bash

csr () {
	wishbone-tool --csr-csv csr.csv "$@"
}

csr reboot_ctrl 0xac
