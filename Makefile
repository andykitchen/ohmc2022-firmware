all: blinky.bin

blinky.json: blinky.ys top.v pll.v
	yosys blinky.ys

blinky.asc: blinky.json
	nextpnr-ice40 --hx1k --package tq144 --json blinky.json --pcf pins.pcf --asc blinky.asc

blinky.bin: blinky.asc
	icepack blinky.asc blinky.bin

prog:
	iceprog blinky.bin

clean:
	rm -f blinky.json blinky.asc blinky.bin

.PHONY: all clean prog
