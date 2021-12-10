from migen import Module, Signal, Instance, ClockDomain, If
from migen.genlib.cdc import AsyncResetSynchronizer

from litex.soc.integration.doc import AutoDoc

class FomuClockResourceGenerator(Module, AutoDoc):
    """Fomu Clock Resource Generator

    Fomu is a USB device, which means it must have a 12 MHz clock.  Valentyusb
    oversamples the clock by 4x, which drives the requirement for a 48 MHz clock.
    The ICE40UP5k is a relatively low speed grade of FPGA that is incapable of
    running the entire design at 48 MHz, so the majority of the logic is placed
    in the 12 MHz domain while only critical USB logic is placed in the fast
    48 MHz domain.

    Fomu has a 48 MHz crystal on it, which provides the raw clock input.  This
    signal is fed through the ICE40 PLL in order to divide it down into a 12 MHz
    signal and keep the clocks within 1ns of phase.  Earlier designs used a simple
    flop, however this proved unreliable when the FPGA became very full.

    The following clock domains are available on this design:

    +---------+------------+---------------------------------+
    | Name    | Frequency  | Description                     |
    +=========+============+=================================+
    | usb_48  | 48 MHz     | Raw USB signals and pulse logic |
    +---------+------------+---------------------------------+
    | usb_12  | 12 MHz     | USB control logic               |
    +---------+------------+---------------------------------+
    | sys     | 12 MHz     | System CPU and wishbone bus     |
    +---------+------------+---------------------------------+
    """
    def __init__(self, platform):
        clk48_raw = platform.request("clk48")
        clk12 = Signal()

        reset_delay = Signal(12, reset=4095)
        self.clock_domains.cd_por = ClockDomain()
        self.reset = Signal()

        # Certain internal parts of LiteX expect reset to be called `rst'
        self.rst = self.reset

        self.clock_domains.cd_sys = ClockDomain()
        self.clock_domains.cd_usb_12 = ClockDomain()
        self.clock_domains.cd_usb_48 = ClockDomain()

        platform.add_period_constraint(self.cd_usb_48.clk, 1e9/48e6)
        platform.add_period_constraint(self.cd_sys.clk, 1e9/12e6)
        platform.add_period_constraint(self.cd_usb_12.clk, 1e9/12e6)
        platform.add_period_constraint(clk48_raw, 1e9/48e6)

        # POR reset logic- POR generated from sys clk, POR logic feeds sys clk
        # reset.
        self.comb += [
            self.cd_por.clk.eq(self.cd_sys.clk),
            self.cd_sys.rst.eq(reset_delay != 0),
            self.cd_usb_12.rst.eq(reset_delay != 0),
        ]

        # POR reset logic- POR generated from sys clk, POR logic feeds sys clk
        # reset.
        self.comb += [
            self.cd_usb_48.rst.eq(reset_delay != 0),
        ]

        self.comb += self.cd_usb_48.clk.eq(clk48_raw)

        self.specials += Instance(
            "SB_PLL40_CORE",
            # Parameters
            p_DIVR = 0,
            p_DIVF = 15,
            p_DIVQ = 5,
            p_FILTER_RANGE = 1,
            p_FEEDBACK_PATH = "SIMPLE",
            p_DELAY_ADJUSTMENT_MODE_FEEDBACK = "FIXED",
            p_FDA_FEEDBACK = 15,
            p_DELAY_ADJUSTMENT_MODE_RELATIVE = "FIXED",
            p_FDA_RELATIVE = 0,
            p_SHIFTREG_DIV_MODE = 1,
            p_PLLOUT_SELECT = "GENCLK_HALF",
            p_ENABLE_ICEGATE = 0,
            # IO
            i_REFERENCECLK = clk48_raw,
            o_PLLOUTCORE = clk12,
            # o_PLLOUTGLOBAL = clk12,
            #i_EXTFEEDBACK,
            #i_DYNAMICDELAY,
            #o_LOCK,
            i_BYPASS = 0,
            i_RESETB = 1,
            #i_LATCHINPUTVALUE,
            #o_SDO,
            #i_SDI,
        )

        self.comb += self.cd_sys.clk.eq(clk12)
        self.comb += self.cd_usb_12.clk.eq(clk12)

        self.sync.por += \
            If(reset_delay != 0,
                reset_delay.eq(reset_delay - 1)
            )
        self.specials += AsyncResetSynchronizer(self.cd_por, self.reset)
