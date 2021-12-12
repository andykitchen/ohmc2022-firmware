from migen import Module, ClockSignal, Cat
from migen.fhdl.structure import Signal, If
from litex.soc.integration.doc import AutoDoc, ModuleDoc
from litex.soc.interconnect.csr import AutoCSR, CSRStatus, CSRStorage, CSRField

SCLK_PRESCALE_BITS = 2
LR_PRESCALE_BITS   = 6
SAMPLE_BITS        = 16

class I2S(Module, AutoCSR, AutoDoc):
    """Minimal I2S Block"""
    def __init__(self, pads):
        self.sample = CSRStorage(description="""sample data""", fields=[
            CSRField("left",  size=16, description="16-bit left channel sample"),
            CSRField("right", size=16, description="16-bit right channel sample"),
        ])

        self.control = CSRStorage(fields=[
            CSRField("reset", size=1, description="reset core (NOTE: core is held in reset after system reset)", reset=1),
        ])

        self.status = CSRStatus(fields=[
            CSRField("wait", size=1, description="current sample is waiting to be processed"),
        ])

        self.intro = ModuleDoc("""
        Minimal I2S Block

        Basic usage: write a sample into the `sample' register then busy loop on the `status' register
        until the wait bit is clear, then write a new value.

        This whole core is designed for a 12MHz mclk for an Fs (sample rate) of 12MHz/256 = 46.9kHz
        """)

        core_reset = self.control.fields.reset  # reset line specific to core
        wait       = self.status.fields.wait    # is the current sample waiting to be for output?
        left       = self.sample.fields.left
        right      = self.sample.fields.right

        # Core state
        init       = Signal(reset=1)                       # is this the first cycle after a reset?
        shiftreg   = Signal(SAMPLE_BITS, reset_less=True)  # shift register for outgoing data (including padding bit)
        buffer     = Signal(SAMPLE_BITS, reset_less=True)  # buffer data other channel waiting to go out

        clk_scale  = Signal(SCLK_PRESCALE_BITS + LR_PRESCALE_BITS)
        next_sclk  = clk_scale[SCLK_PRESCALE_BITS-1]
        next_lrclk = clk_scale[SCLK_PRESCALE_BITS+LR_PRESCALE_BITS-1]
        
        sclk       = Signal()
        lrclk      = Signal()
        dout       = Signal()

        # Named internal logic signals
        sclk_falling  = Signal()
        lrclk_rising  = Signal()
        lrclk_falling = Signal()
        begin_sample  = Signal()

        self.comb += [
            sclk_falling.eq(~next_sclk & sclk),      # sclk will be low next cycle
            lrclk_rising.eq(next_lrclk & ~lrclk),    # lrclk will be high on the next cycle (right channel is starting)
            lrclk_falling.eq(~next_lrclk &  lrclk),  # lrclk will be low on the next cycle (left channel is starting)
            begin_sample.eq(init | lrclk_falling),   # a new sample is starting next cycle
        ]

        # Wire up outputs
        self.comb += [
            pads.mclk.eq(ClockSignal()),  # output sys clock on mclk (12MHz   = 256 * Fs)
            pads.sclk.eq(sclk),           # output sclk              ( 3MHz   =  64 * Fs)
            pads.lrclk.eq(lrclk),         # output lrclk             (46.9kHz =   1 * Fs)
            pads.dout.eq(dout),           # output dout
        ]

        self.sync += [
            If(core_reset,
                init.eq(init.reset),
                clk_scale.eq(clk_scale.reset),
                sclk.eq(sclk.reset),
                lrclk.eq(lrclk.reset),
                dout.eq(dout.reset)
            ).Else(
                init.eq(0),
                clk_scale.eq(clk_scale + 1),
                sclk.eq(next_sclk),
                lrclk.eq(next_lrclk),

                # update dout register on the falling edge of the sclk
                If(sclk_falling, dout.eq(shiftreg[-1])),

                # When we begin a sample put the left channel data directly into the shift register
                # and save the right channel data in a buffer
                If(begin_sample, shiftreg.eq(left), buffer.eq(right))
                    .Elif(lrclk_rising, shiftreg.eq(buffer))
                    .Elif(sclk_falling, shiftreg.eq(shiftreg << 1)),
                
                # Clear the wait bit once the sample is read into shift register, otherwise set it on sample write.
                If(begin_sample, wait.eq(0))
                    .Elif(self.sample.re, wait.eq(1)),
            )
        ]
