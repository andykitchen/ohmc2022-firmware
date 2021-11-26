BIOS_DIR  := build/rockling/software/bios

all: bitstream

bitstream: build/rockling/gateware/rockling.bin

bitstream-load: build/rockling/gateware/rockling.bin
	dfu-util -D build/rockling/gateware/rockling.bin

build/rockling/gateware/rockling.bin: rockling.py rockling_evt.py lxbuildenv.py custom-bios/* | venv
	venv/bin/python rockling.py

venv:
	git submodule update --init --recursive
	bash setup-venv.sh

# NOTE: building the bitstream also builds the bios
bios:
	make -C ${BIOS_DIR} -f $(realpath custom-bios/Makefile)

bios-clean:
	rm -rf ${BIOS_DIR}/*.[od] ${BIOS_DIR}/bios.bin ${BIOS_DIR}/bios.elf

venv/bin/intercept-build: | venv
	pip install scan-build

compile_commands.json: venv/bin/intercept-build custom-bios/Makefile build/rockling/software/include/generated/variables.mak
	make bios-clean
	venv/bin/intercept-build make bios

clean:
	rm -rf build

nuke:
	rm -rf build venv

.PHONY: all bitstream bitstream-load bios bios-clean clean nuke
