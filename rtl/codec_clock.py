from migen import Module
from migen.fhdl.structure import ClockSignal

class CodecClock(Module):
    """Provice a master clock signal for the SGTL5000 audio codec"""
    def __init__(self, pads):
        clk = ClockSignal()
        self.comb += [
            pads.mclk.eq(clk)
        ]