BIOS_DIR=build/rockling/software/bios

all: bitstream

bitstream: venv
	venv/bin/python rockling.py

bitstream-load: bitstream
	dfu-util -D build/rockling/gateware/rockling.bin

venv:
	git submodule update --init --recursive
	./setup-venv.sh

# NOTE: building the bitstream also builds the bios
bios:
	make -C ${BIOS_DIR} -f $(realpath custom-bios/Makefile)

bios-clean:
	rm -rf ${BIOS_DIR}/*.o ${BIOS_DIR}/*.d ${BIOS_DIR}/bios.bin ${BIOS_DIR}/bios.elf

clean:
	rm -rf build

.PHONY: all bitstream bitstream-load bios bios-clean clean
