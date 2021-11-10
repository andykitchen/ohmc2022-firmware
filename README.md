Rockling Firmware
=================

### Prerequisites

You will need:

-   [Icestorm iCE40 synthesis toolchain](http://www.clifford.at/icestorm/)
-   RISCV compiler toolchain
    -   [Binary releases from SciFive](https://www.sifive.com/software)
    -   [Binary releases from riscv-collab on GitHub](https://github.com/riscv-collab/riscv-gnu-toolchain/releases)
-   [LiteX SoC Builder](https://github.com/enjoy-digital/litex)
    -   install inside a python virtual env using the included `setup-venv.sh` script
    -   _optional:_ [Install using globally using `litex_setup.py`](https://github.com/enjoy-digital/litex#quick-start-guide)
-   _For debugging and testing:_ [wishbone-utils](https://github.com/litex-hub/wishbone-utils)
    -   [wishbone-tool precompiled binaries](https://github.com/litex-hub/wishbone-utils/releases)

### Build

Setup python environment:

```shell
$ setup-venv.sh             # only needed on first run
$ source venv/bin/activate  # only needed once per terminal session
```

Build gateware:

```
$ python rockling.py
```

If you get a timing failure, it may be transient, try a different seed:

```
$ python rockling.py --seed <N>
```

Upload firmware using `dfu-util`:

```
$ dfu-util -D build/rockling/gateware/rockling.bin
```

When running, the device should enumerate on the USB bus
as a debug device. You can peek and poke memory
on the wishbone bus using `wishbone-util` from the fomu
toolchain.

One can WARMSTART reset back into foboot by writing the value '0xac'
into the a certain CSR address:

```
$ wishbone-tool --csr-csv csr.csv reboot_ctrl 0xac
```

You can also peek and poke raw memory addresses:

```
$ wishbone-tool 0x00000800 0xac
```

NOTE: CSR memory address may change please refer to `csr.csv`
and look for a line like:

```
csr_register,reboot_ctrl,0x00000800,1,rw
```

This particular CSR register is created in `rtl/sbwarmboot.py`

## I2C Test Scripts

There are i2c test scripts in the `tools` directory, they should be run
from the top-level e.g.

```
$ ./tools/i2c_probe.sh
$ ./tools/i2c_test_dac.sh
$ ./tools/i2c_test_codec.sh
```

The 8th bit of the `i2c_status` register is the `RxACK` bit,
it is 0 if an `ACK`, 1 if `NACK`.

## Embedded Analyzer

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

## See Also:

-   [LiteX CSRs: A Developer's Overview](https://github.com/enjoy-digital/litex/wiki/CSR-Bus)
-   [ValentyUSB](https://github.com/im-tomu/valentyusb)
