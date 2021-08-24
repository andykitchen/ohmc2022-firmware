Rockling Firmware
=================

Build:

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
$ wishbone-tool 0x00000800 0xac
```

NOTE: CSR memory address may change please refer to `build/csr.csv`
and look for a line like:

```
csr_register,reboot_ctrl,0x00000800,1,rw
```

This CSR register is created in `rtl/sbwarmboot.py`

See Also:

-   [ValentyUSB](https://github.com/im-tomu/valentyusb)
-   [wishbone-utils](https://github.com/litex-hub/wishbone-utils)
