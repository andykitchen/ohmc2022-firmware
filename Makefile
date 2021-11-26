BIOS_DIR  := build/rockling/software/bios

all: bitstream

bitstream: build/rockling/gateware/rockling.bin

bitstream-load: build/rockling/gateware/rockling.bin
	dfu-util -D build/rockling/gateware/rockling.bin

build/rockling/gateware/rockling.bin: rockling.py rockling_evt.py lxbuildenv.py custom-bios/* | venv
	venv/bin/python rockling.py

venv: setup-venv.sh requirements.txt
	git submodule update --init --recursive
	./setup-venv.sh

# NOTE: building the bitstream also builds the bios
bios:
	make -C ${BIOS_DIR} -f $(realpath custom-bios/Makefile)

bios-clean:
	rm -rf ${BIOS_DIR}/*.[od] ${BIOS_DIR}/bios.bin ${BIOS_DIR}/bios.elf

compile_commands.json: custom-bios/Makefile build/rockling/software/include/generated/variables.mak
	make bios-clean
	bear make bios

clean:
	rm -rf build

nuke:
	rm -rf build venv

.PHONY: all bitstream bitstream-load bios bios-clean clean nuke
