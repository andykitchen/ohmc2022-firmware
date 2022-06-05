all: bios.bin

bios.o: bios.S
	riscv64-unknown-elf-as -march=rv32i -mabi=ilp32 -fno-pic -g -ahl bios.S -o bios.o

bios.elf: bios.o linker.ld
	riscv64-unknown-elf-ld -nostdlib -nodefaultlibs --no-dynamic-linker --build-id=none -T linker.ld bios.o -o bios.elf

bios.bin: bios.elf
	riscv64-unknown-elf-objcopy -O binary bios.elf bios.bin

clean:
	rm -rf *.o bios.elf bios.bin

.PHONY: all clean
