BIOS_DIR ?= ../custom-bios

BIOS_SRC = \
	$(BIOS_DIR)/rgb.c \
	$(BIOS_DIR)/i2c.c \
	$(BIOS_DIR)/dac.c \
	$(BIOS_DIR)/codec.c

SRC = $(BIOS_SRC) etherbone.c
HDR = $(SRC:%.c=%.h)

all: bios

bios: main.c $(SRC) $(HDR)
	gcc -Og -g -isystem soc -I../build/rockling/software/include -I$(BIOS_DIR) $(SRC) main.c -o bios

compile_commands.json:
	. ../venv/bin/activate && make clean && intercept-build make bios

clean:
	rm -f bios

.PHONY: all clean
