import unittest

from migen import *

from rtl.i2s import I2S

TEST_VAL = 0xa5a58001

pads = Record([
    ("mclk",  1),
    ("sclk",  1),
    ("lrclk", 1),
    ("din",   1),
    ("dout",  1)
])


class I2STest(unittest.TestCase):
    def test_sample_write(self):
        def generator(dut):
            yield from dut.sample.write(TEST_VAL)
            yield
            self.assertEqual((yield from dut.sample.read()), TEST_VAL)
            yield dut.control.fields.reset.eq(0)
            yield
            for i in range(512+20):
                yield

        dut = I2S(pads)
        run_simulation(dut, generator(dut), vcd_name='i2s.vcd')


if __name__ == "__main__":
    from migen.fhdl.verilog import convert
    top = I2S(pads)
    ios = set(pads.flatten())
    ios.add(top.sample.re)
    ios.update(top.sample.fields.fields)
    ios.update(top.control.fields.fields)
    ios.update(top.status.fields.fields)
    convert(top, ios=ios).write("i2s.v")
