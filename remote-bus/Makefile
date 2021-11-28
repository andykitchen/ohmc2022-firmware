all: bios

bios: main.c
	gcc -isystem soc -I../build/rockling/software/include etherbone.c main.c -o bios

.PHONY: all
