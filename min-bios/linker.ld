OUTPUT_FORMAT("elf32-littleriscv")
ENTRY(_start)

__DYNAMIC = 0;

MEMORY {
	rom  : ORIGIN = 0x00000000, LENGTH = 0x00002000
	sram : ORIGIN = 0x01000000, LENGTH = 0x00020000
	csr  : ORIGIN = 0x82000000, LENGTH = 0x00010000
}

SECTIONS
{
	.text : { *(.text) }  > rom
	.bss  : { *(COMMON) } > sram
}
