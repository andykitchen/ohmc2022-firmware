include ../include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

CFLAGS += -Wno-parentheses -O2

OBJECTS = \
	isr.o             \
	rgb.o             \
	i2c.o             \
	loop.o            \
	main.o

all: bios.bin
	$(PYTHON) -m litex.soc.software.memusage bios.elf $(CURDIR)/../include/generated/regions.ld $(TRIPLE)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	chmod -x $@

bios.elf: $(BIOS_DIRECTORY)/linker.ld $(OBJECTS)

vpath %.a $(PACKAGES:%=../%)

%.elf: crt0.o $(LIBS:%=%.a)
	$(CC) $(LDFLAGS) -T $(BIOS_DIRECTORY)/linker.ld -N -o $@ \
		crt0.o \
		$(OBJECTS) \
		$(PACKAGES:%=-L../%) \
		-Wl,--whole-archive \
		-Wl,--gc-sections \
		$(LIBS:lib%=-l%) \


# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

# main.o: CFLAGS += -O2

%.o: $(BIOS_DIRECTORY)/%.c
	$(compile)

%.o: $(BIOS_DIRECTORY)/%.S
	$(assemble)

crt0.o: $(CPU_DIRECTORY)/crt0.S
	$(assemble)

clean:
	$(RM) $(OBJECTS) bios.elf bios.bin

.PHONY: all clean
