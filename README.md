Rockling Firmware
=================

Prerequisites
-------------

You will need:

-   [Python 3.6+](https://www.python.org/)
-   [Icestorm iCE40 synthesis toolchain](http://www.clifford.at/icestorm/)
-   RISCV compiler toolchain
    -   [Binary releases from SciFive](https://www.sifive.com/software)
    -   [Binary releases from riscv-collab on GitHub](https://github.com/riscv-collab/riscv-gnu-toolchain/releases)
-   [LiteX SoC Builder](https://github.com/enjoy-digital/litex) _(automatically installed as git submodules)_
    -   _optional:_ manually install inside a python virtual env using the included `setup-venv.sh` script
    -   _optional:_ [Install globally using `litex_setup.py`](https://github.com/enjoy-digital/litex#quick-start-guide)
-   For debugging and testing:_ [wishbone-utils](https://github.com/litex-hub/wishbone-utils)
    -   [wishbone-tool precompiled binaries](https://github.com/litex-hub/wishbone-utils/releases)


Building and Running
--------------------

I've tried to automate the build process as best I can.
After installing the prerequsites just run:

```
$ make all
```

Everything _should_ automagically work, if there are
any problems you can try the step-by-step manual
build process further down to try and narrow down the problem.


### Firmware Upload

Upload firmware to the Rocking using `dfu-util`:

```
$ dfu-util -D build/rockling/gateware/rockling.bin
```

OR

```
$ make bitstream-load
```

The colour of the LED in the Rockling's eye should change:

-   BLUE is good and means power on self test passed.
-   RED  is bad and means power on self test has failed.
-   OFF  bitstream has failed to load correctly.

When running, the device should enumerate on the USB bus
as a debug device. You can peek and poke memory
on the wishbone bus using `wishbone-util` from the fomu
toolchain.

One can WARMSTART reset back into foboot by writing the value '0xac'
into a certain CSR address:

```
$ wishbone-tool --csr-csv csr.csv reboot_ctrl 0xac
```

You can also peek and poke raw memory addresses:

```
# read 4 bytes at the start of SRAM
$ wishbone-tool 0x01000000
```

You can find all the CSR memory addresses in `csr.csv`.


### I2C Test Scripts

There are i2c test scripts in the `tools` directory, they should be run
from the top-level e.g.

```
$ ./tools/i2c_probe.sh
$ ./tools/i2c_test_dac.sh
$ ./tools/i2c_test_codec.sh
```

The 8th bit of the `i2c_status` register is the `RxACK` bit,
it is 0 if an `ACK`, 1 if `NACK`.


### Embedded Analyzer

One can enable an embedded [litescope](https://github.com/enjoy-digital/litescope)
signal analyzer. First, build the firmware with the analyzer enabled:

```
$ python rockling.py --with-analyzer
```

Litescope communicates using the etherbone protocol,
you will need to use the `wishbone-tool` in the server
mode to bridge from etherbone<->USB.

```
$ wishbone-tool --server=wishbone
```

Once the `wishbone-tool` server is running you
can use the `litescope_cli` tool to download
traces from the device:

```
$ bin/litescope_cli -r freq_cnt0_sig --subsample 1200
```

Some useful documentation on how to
[use litescope to debug a SoC](https://github.com/enjoy-digital/litex/wiki/Use-LiteScope-To-Debug-A-SoC)
and how to [use the host bridge to debug](https://github.com/enjoy-digital/litex/wiki/Use-Host-Bridge-to-control-debug-a-SoC).


### Manual build

Setup python environment:

```
# On first run
$ git submodule update --init --recursive
$ setup-venv.sh

# For each terminal session
$ source venv/bin/activate
```

Build gateware:

```
$ python rockling.py
```

If you get a timing failure, it may be transient, try a different seed:

```
$ python rockling.py --seed <N>
```


Custom BIOS
-----------

Generally when LiteX is used on larger boards a small BIOS is stored directly
into a ROM embedded in the bitstream. This BIOS is used to then load an operating system
from for example UART, SPI flash, TFTP etc. The operating system will then
host the application.

The currently Rockling application is minimal enough that the entire application
can fit inside the ROM, so there is no real need for a BIOS per se.
The entire application runs on the bare metal and the whole thing is stored
in bitstream ROM.
The code in the `custom-bios` directory entirely replaces the usual LiteX BIOS.
This has the benefit that the bitstream is entirely self contained,
the drawback is unfortunately that making a small change to the application
firmware requires rebuilding the entire bitstream. Currently this process
is reasonably quick and reliable, but if that changes we'll have to find a
way to speed up development.

You can disassemble the ELF version of the bios with:

```
$ riscv64-unknown-elf-objdump -d build/rockling/software/bios/bios.elf
```

Or directly dissassemble the raw binary image of the BIOS with:

```
$ riscv64-unknown-elf-objdump -b binary -m riscv:rv32 -D build/rockling/software/bios/bios.bin
```

It's interesting to see because the code is basically just a lot of
of loads and stores to control registers, with a few branches.


### Boot sequence

The early boot procedure is pretty straight forward.
The CPU reset vector is just `0x00000000` this is the bottom of ROM,
corresponding to the first instruction in `bios.bin`.

Then, there is an immediate jump into the `reset_vector` body which:

1. Sets up the stack pointer
2. Sets up `trap_vector` using `csrw mtvec,t0` this is the location the CPU will jump on interrupts:
    * illegal instructions
    * bus errors?
    * external interrupts? _(not implemented in FemtoRV the CPU core we're currently using)_
3. Copies initial values for static variables (`.data`) from their location in ROM into SRAM starting at `0x010000000`
4. Zeros SRAM locations for zero initialised variables (`.bss`)
5. Enables some interrupts using `csrs mie,t0`
6. Jumps to `main()`
7. Goes into an infinite loop if `main()` ever returns

The `trap_vector`:

1. Tries to save all the registers on the stack
2. Jumps to the `isr()`
3. Reloads the saved registers
4. Tries to return from the handler via the `mret` instruction

### Software Debugging

Because the code runs on the bare metal on a very minimalistic RISCV core,
there is no easy way to attach a debugger or even `printf()` messages for
that matter.

Here are tricks that I've been using:

1.   Use the colour of the LED to indicate progress.
2.   Use Messible.
3.   Just read the SRAM directly.

#### Messible

The Messible system is a minimal FIFO controlled with CSR registers
for device-to-host communication, `wishbone-tool` has some support for it.
On the device:

```c
/* write to messible in C */
messible_puts("DOH!");
```

On the host:

```shell
# read messible messages on the host
$ wishbone-tool --server=messible --messible-address=0x82002800
```

Where the messible address can be looked up in `csr.csv`,
look for the `csr_base` address of `messible`:

```
csr_base,messible,0x82002800,,
```

#### Directly read from SRAM

Because there is debug bus access you can peek / poke to a known location
in SRAM and read that out on the host:

Create an arbitrary global variable and mark it volatile, so the
compiler knows that it should not optimise away writes to that location:

```c
volatile unsigned int debug_status;

/* store some state or flag in a global variable to read out of SRAM */
die() {
    debug_status = 0xDEADBEEF;
    while(1) { }
}
```

Find out the address of the global variable in the compiled binary symbol table:

```shell
$ riscv64-unknown-elf-readelf -s build/rockling/software/bios/bios.elf | grep debug_status
```

Read the value of that global variable by directly reading that memory address:

```shell
$ wishbone-tool 0x01000000
```

This is very ghetto debugging but it gets the job done. Ideally one could automate this
to various degrees.


VSCode Setup
------------

The [VSCode IDE](https://code.visualstudio.com/) works quite well on this project when configured correctly.
Both the Python and C/C++ features work at the same time, which is handy
when switching between editing the LiteX Python SoC description and the C software.
You can use `tools/dot-vscode-example` as a starting point for your own configuration.

For automagic vscode setup use the following command:

```shell
$ ./tools/vscode-setup.sh
```

You may want to install the following extensions:
-   Python
-   C/C++
-   RISC-V Support (for RISCV assembly code syntax colouring)

If you prefer using [VSCodium](https://vscodium.com/) and/or use
the [clangd](https://clangd.llvm.org/) plugin; you can generate a
`compile_commands.json` file using [bear](https://github.com/rizsotto/Bear).

```shell
$ make bios-clean && bear make bios
```

See Also
--------

-   [LiteX CSRs: A Developer's Overview](https://github.com/enjoy-digital/litex/wiki/CSR-Bus)
-   [ValentyUSB](https://github.com/im-tomu/valentyusb)
