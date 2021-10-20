#!/bin/bash

csr () {
	wishbone-tool --csr-csv build/csr.csv "$@"
}

csr reboot_ctrl 0xac
