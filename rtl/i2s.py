from migen import Module, Instance, ClockSignal
from migen.fhdl.structure import ClockDomain, ResetSignal, Signal, If, Constant
from migen.genlib.cdc import BusSynchronizer, MultiReg
from litex.soc.integration.doc import AutoDoc, ModuleDoc
from litex.soc.interconnect.csr import AutoCSR, CSRStatus, CSRStorage, CSRField


class I2S(Module, AutoCSR, AutoDoc):
    """Minimal I2S Block"""
    def __init__(self, pads, use_global_buffer=True):
        self.sample = CSRStorage(32, description="""sample data""", reset=0xCAFEF00D)
        self.status = CSRStatus(32, description="""count of sclk ticks""")

        self.intro = ModuleDoc("""
        Minimal I2S Block
        """)

        # Provide sys clock output to mclk
        self.comb += [
            pads.mclk.eq(ClockSignal())
        ]

        # Globally buffer sclk
        sclk = Signal()
        if use_global_buffer:
            self.specials += Instance("SB_GB",
                i_USER_SIGNAL_TO_GLOBAL_BUFFER = pads.sclk,
                o_GLOBAL_BUFFER_OUTPUT = sclk
            )
        else:
            sclk = pads.sclk

        # Create a new audio clock domian 
        self.clock_domains.cd_i2s_sclk = ClockDomain()
        self.cd_i2s_sclk.clk = sclk
        self.cd_i2s_sclk.rst = ResetSignal()

        counter = Signal(32)
        self.submodules.bus_sync = BusSynchronizer(32, idomain="i2s_sclk", odomain="sys")

        self.sync.i2s_sclk += [
            counter.eq(counter + 1),
            self.bus_sync.i.eq(counter)
        ]

        self.sync.sys += [
            self.status.status.eq(self.bus_sync.o)
        ]
