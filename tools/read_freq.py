#!/usr/bin/env python3

import time
from litex import RemoteClient

wb = RemoteClient()
wb.open()

# reset frequency counters
wb.regs.freq_cnt0_ctrl.write(0x1)
wb.regs.freq_cnt1_ctrl.write(0x1)

# collect some cycles
time.sleep(1.0)

# pause frequency counters
wb.regs.freq_cnt0_ctrl.write(0x2)
wb.regs.freq_cnt1_ctrl.write(0x2)

def get_freq(reg_elapse, reg_cycles):
	elapse = reg_elapse.read()
	cycles = reg_cycles.read()
	elapse_seconds = elapse / 12e6
	freq = cycles / elapse_seconds
	return freq, cycles, elapse_seconds

f0, n0, t0 = get_freq(wb.regs.freq_cnt0_elapse, wb.regs.freq_cnt0_cycles)
f1, n1, t1 = get_freq(wb.regs.freq_cnt1_elapse, wb.regs.freq_cnt1_cycles)

print("osc0: {:8.2f} Htz  ({:4.0f} / {:4.0f}ms)".format(f0, n0, 1000*t0))
print("osc1: {:8.2f} Htz  ({:4.0f} / {:4.0f}ms)".format(f1, n1, 1000*t1))

# reset frequency counters again
wb.regs.freq_cnt0_ctrl.write(0x1)
wb.regs.freq_cnt1_ctrl.write(0x1)

wb.close()