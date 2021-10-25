# This file is Copyright (c) 2019 Tom Keddie <git@bronwenandtom.com>
# This file is Copyright (c) 2019 Sean Cross <sean@xobs.io>
# This file is Copyright (c) 2021 Andy Kitchen <kitchen.andy+git@gmail.com>
# License: BSD

# Rockling Prototype Platform

from litex.build.generic_platform import *
from litex.build.lattice import LatticePlatform
from litex.build.lattice.programmer import IceStormProgrammer

# IOs -------------------------------------------------------------------------

_io = [
    ("clk48", 0, Pins("20"), IOStandard("LVCMOS33")),                  # GOOD

    ("rgb_led", 0,
        Subsignal("r", Pins("40")),                                    # GOOD
        Subsignal("g", Pins("39")),                                    # GOOD
        Subsignal("b", Pins("41")),                                    # GOOD
        IOStandard("LVCMOS33"),
    ),

    ("user_btn_n", 0, Pins("43"), IOStandard("LVCMOS33")),             # PIN_OK

#   ("serial", 0,
#       Subsignal("rx", Pins("34")),                                   # CHECK
#       Subsignal("tx", Pins("36"), Misc("PULLUP")),                   # WRONG
#       IOStandard("LVCMOS33")
#   ),

    ("usb", 0,
        Subsignal("d_p",    Pins("25")),                               # PIN_OK
        Subsignal("d_n",    Pins("23")),                               # PIN_OK
        Subsignal("pullup", Pins("26")),                               # PIN_OK
        IOStandard("LVCMOS33")
    ),

    ("spiflash", 0,
        Subsignal("cs_n", Pins("16"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("clk",  Pins("15"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("miso", Pins("17"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("mosi", Pins("14"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("wp",   Pins("18"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("hold", Pins("19"), IOStandard("LVCMOS33")),         # GOOD
    ),

    ("spiflash4x", 0,
        Subsignal("cs_n", Pins("16"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("clk",  Pins("15"), IOStandard("LVCMOS33")),         # GOOD
        Subsignal("dq",   Pins("14 17 18 19"), IOStandard("LVCMOS33")),# GOOD
    ),

    ("i2c", 0,
        Subsignal("scl", Pins("9"),  IOStandard("LVCMOS33")),          # PIN_OK
        Subsignal("sda", Pins("10"), IOStandard("LVCMOS33")),          # PIN_OK
    ),

    ("i2c", 1,
        Subsignal("scl", Pins("31"), IOStandard("LVCMOS33")),          # PIN_OK
        Subsignal("sda", Pins("32"), IOStandard("LVCMOS33")),          # PIN_OK
    ),

    ("i2s", 0,
        Subsignal("mclk",  Pins("44"), IOStandard("LVCMOS33")),
        Subsignal("dout",  Pins("45"), IOStandard("LVCMOS33")),
        Subsignal("sclk",  Pins("46"), IOStandard("LVCMOS33")),
        Subsignal("lrclk", Pins("47"), IOStandard("LVCMOS33")),
        Subsignal("din",   Pins("48"), IOStandard("LVCMOS33")),
    ),

    ("osc", 0, Pins("13"), IOStandard("LVCMOS33")),
    ("osc", 1, Pins("37"), IOStandard("LVCMOS33")),
]

# Connectors ------------------------------------------------------------------

_connectors = [
    ("touch_pins", "48 47 46 45"),  # 12 34 38 43
#   ("pmoda_n",    "28 27 26 23"),
#   ("pmodb_n",    "48 47 46 45"),
#   ("dbg",        "20 12 11 25 10 9"),
]

# Platform --------------------------------------------------------------------

class Platform(LatticePlatform):
    default_clk_name   = "clk48"
    default_clk_period = 1e9/48e6

    def __init__(self):
        LatticePlatform.__init__(self, "ice40-up5k-sg48", _io, _connectors, toolchain="icestorm")

    def create_programmer(self):
        return IceStormProgrammer()

    def do_finalize(self, fragment):
        LatticePlatform.do_finalize(self, fragment)
        self.add_period_constraint(self.lookup_request("clk48", loose=True), 1e9/48e6)
