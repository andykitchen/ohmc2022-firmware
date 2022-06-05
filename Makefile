BIOS_DIR ?= build/rockling/software/bios
BITSTREAM_FLAGS ?=

all: bitstream

venv:
	git submodule update --init --recursive
	bash setup-venv.sh

test:
	python -m unittest discover -s test

bitstream: build/rockling/gateware/rockling.bin

bitstream-load: build/rockling/gateware/rockling.bin
	dfu-util -D build/rockling/gateware/rockling.bin

build/rockling/gateware/rockling.bin: rockling.py rockling_evt.py lxbuildenv.py custom-bios/* rtl/*.py | venv
	. venv/bin/activate && python rockling.py $(BITSTREAM_FLAGS)

# NOTE: building the bitstream also builds the bios
bios: | venv
	. venv/bin/activate && make -C ${BIOS_DIR} -f $(realpath custom-bios/Makefile)

bios-clean:
	rm -rf ${BIOS_DIR}/*.[od] ${BIOS_DIR}/bios.bin ${BIOS_DIR}/bios.elf

# NOTE: bitstream must be specially compiled to support hot reloading, see README
bios-reload: bios
	bash tools/hot_reload.sh

venv/bin/intercept-build: | venv
	pip install scan-build

compile_commands.json: venv/bin/intercept-build custom-bios/Makefile build/rockling/software/include/generated/variables.mak
	make bios-clean
	. venv/bin/activate && intercept-build make bios
# FIXME is there a better way of doing this?
	sed -i'' 's/"cc"/"riscv64-unknown-elf-gcc"/g' compile_commands.json

clean:
	rm -rf build

nuke:
	rm -rf build venv

.PHONY: all test bitstream bitstream-load bios bios-clean clean nuke
