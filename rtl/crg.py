from migen import Module, Signal, Instance, ClockDomain, If
from migen.genlib.cdc import AsyncResetSynchronizer, MultiReg

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
        self.clock_domains.cd_sys    = ClockDomain()
        self.clock_domains.cd_por    = ClockDomain()
        self.clock_domains.cd_usb_12 = ClockDomain()
        self.clock_domains.cd_usb_48 = ClockDomain()

        clk48 = Signal() # Output by global buffer
        clk12 = Signal() # Output by PLL see below

        clk48_raw = platform.request("clk48")

        # Manually place 48MHz clock signal into a global buffer
        self.specials += Instance(
            "SB_GB_IO",
            i_PACKAGE_PIN = clk48_raw,
            o_GLOBAL_BUFFER_OUTPUT = clk48
        )

        platform.add_period_constraint(self.cd_sys.clk,    1e9/12e6)
        platform.add_period_constraint(self.cd_usb_12.clk, 1e9/12e6)
        platform.add_period_constraint(self.cd_usb_48.clk, 1e9/48e6)
        platform.add_period_constraint(clk48,              1e9/48e6)

        # Power on Reset (POR) delay logic
        # Hold entire system in reset during countdown or when PLL is not yet locked
        # The countdown is there because apparently early on the lock signal can be unreliable,
        # so we ignore the PLL lock signal until it has had a chance to stabilise
        reset_delay_bits = 12
        reset_delay = Signal(reset_delay_bits, reset=2**reset_delay_bits-1)
        reset_delay_zero = Signal()
        pll_lock = Signal()
        por_reset = Signal()
        por_reset_raw = (~reset_delay_zero | ~pll_lock)
        por_reset_sync = Signal()

        # NB the PLL lock signal may be async, but we want our global
        # reset signal to be synchronous so we use a synchroniser here,
        # this also has ths side effect of slightly delaying internally triggered
        # whole system resets by two clock cycles
        self.specials += MultiReg(i=por_reset_raw, o=por_reset_sync, n=2, odomain='por')

        # Manually place POR reset into global buffer
        self.specials += Instance(
            "SB_GB",
            i_USER_SIGNAL_TO_GLOBAL_BUFFER = por_reset_sync,
            o_GLOBAL_BUFFER_OUTPUT = por_reset
        )

        self.comb += [
            reset_delay_zero.eq(reset_delay == 0),
        ]

        self.sync.por += [
            If(~reset_delay_zero,
                reset_delay.eq(reset_delay - 1)),
        ]

        # Allow Power on Reset system to be internally triggered, this is the whole system reset line
        # This seems to ruin the USB connection though, perhaps some bad interaction with resetting and USB devices.
        self.reset = Signal()
        self.specials += AsyncResetSynchronizer(self.cd_por, self.reset)

        self.rst = self.reset   # Certain internal parts of LiteX expect reset to be called `rst'

        # Wire up all clock domains
        self.comb += [
            self.cd_por.clk.eq(clk12),

            self.cd_sys.clk.eq(clk12),
            self.cd_usb_12.clk.eq(clk12),
            self.cd_usb_48.clk.eq(clk48),

            self.cd_sys.rst.eq(por_reset),
            self.cd_usb_12.rst.eq(por_reset),
            self.cd_usb_48.rst.eq(por_reset),
        ]

        # Setup up iCE40UP5k internal PLL
        self.specials += Instance(
            "SB_PLL40_CORE",
            # Parameters for 12MHz
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
            i_REFERENCECLK = clk48,
            #o_PLLOUTCORE = clk12,
            o_PLLOUTGLOBAL = clk12,
            #i_EXTFEEDBACK,
            #i_DYNAMICDELAY,
            o_LOCK = pll_lock,
            i_BYPASS = 0,
            i_RESETB = 1,
            #i_LATCHINPUTVALUE,
            #o_SDO,
            #i_SDI,
        )
