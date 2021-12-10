from migen import Module
from migen.fhdl.structure import Signal, If, Constant
from migen.genlib.cdc import MultiReg
from litex.soc.integration.doc import AutoDoc, ModuleDoc
from litex.soc.interconnect.csr import AutoCSR, CSRStatus, CSRStorage, CSRField

class FrequencyCounter(Module, AutoCSR, AutoDoc):
    """Frequency counter"""
    def __init__(self, pads):
        self.elapse = CSRStatus(32, name="elapse", description="""number of clock ticks elapsed""")
        self.cycles = CSRStatus(32, name="cycles", description="""number of signal cycles counted""")

        self.ctrl = CSRStorage(name="ctrl", fields = [
            CSRField("reset", size=1, description="Write `1` for a synchronous reset of the counters", pulse=True),
            CSRField("disable", size=1, description="Write `1` to disable counter")
        ])

        self.intro = ModuleDoc("""
        Simple frequency counter
        """)

        elapse  = self.elapse.status
        cycles  = self.cycles.status
        reset   = self.ctrl.fields.reset
        disable = self.ctrl.fields.disable

        elapse_next = Signal(len(elapse))
        cycles_next = Signal(len(cycles))

        self.sig = sig = Signal(len(pads))
        sig_prev = Signal(len(sig))

        sig_rising_edge = Signal()

        self.specials += MultiReg(pads, sig)

        self.comb += [
            elapse_next.eq(elapse + 1),
            cycles_next.eq(cycles + 1),
            sig_rising_edge.eq(sig & ~sig_prev),
        ]

        self.sync += [
            If(~disable,
                If(reset,
                    elapse.eq(0),
                    cycles.eq(0),
                ).Else(
                    sig_prev.eq(sig),
                    elapse.eq(elapse_next),
                    If(elapse_next == Constant(0, len(elapse)), cycles.eq(0))
                      .Elif(sig_rising_edge, cycles.eq(cycles_next)),
                ))
        ]