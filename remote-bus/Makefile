BIOS_DIR ?= ../custom-bios

all: bios

bios: main.c etherbone.[ch]
	gcc -Og -g -isystem soc -I../build/rockling/software/include -I$(BIOS_DIR) etherbone.c main.c $(BIOS_DIR)/rgb.c $(BIOS_DIR)/i2c.c -o bios

clean:
	rm -f bios

.PHONY: all clean
