from migen import Module
from migen.fhdl.structure import Signal, If, Constant
from migen.genlib.cdc import MultiReg
from litex.soc.integration.doc import AutoDoc, ModuleDoc
from litex.soc.interconnect.csr import AutoCSR, CSRStatus, CSRStorage, CSRField

class FrequencyCounter(Module, AutoCSR, AutoDoc):
    """Frequency counter"""
    def __init__(self, pads):
        self.elapse = CSRStatus(32, name="elapse", description="""number of clock ticks elapsed""")
        self.cycles = CSRStatus(16, name="cycles", description="""number of signal cycles counted""")
        self.reset  = CSRStorage(1, name="reset",  description="""reset this module""")

        self.intro = ModuleDoc("""
        Simple frequency counter
        """)

        elapse = self.elapse.status
        cycles = self.cycles.status
        reset  = self.reset.storage

        elapse_next = Signal(len(elapse))
        cycles_next = Signal(len(cycles))

        sig = Signal(len(pads))
        sig_prev = Signal(len(sig))

        self.specials += MultiReg(pads, sig)

        self.comb += [
            elapse_next.eq(elapse + 1),
            cycles_next.eq(cycles + 1),
        ]

        self.sync += [
            If(reset,
                elapse.eq(0),
                cycles.eq(0),
            ).Else(
                sig_prev.eq(sig),
                elapse.eq(elapse_next),
                If(elapse_next == Constant(0, len(elapse)), cycles.eq(0))
                  .Else(If(sig & ~sig_prev, cycles.eq(cycles_next))),
            ),
        ]