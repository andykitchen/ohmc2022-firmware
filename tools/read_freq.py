#!/usr/bin/env python3

from litex import RemoteClient

wb = RemoteClient()
wb.open()

def get_freq(reg_elapse, reg_cycles):
	elapse = reg_elapse.read()
	cycles = reg_cycles.read()
	elapse_seconds = elapse / 12e6
	freq = cycles / elapse_seconds
	return freq

f0 = get_freq(wb.regs.freq_cnt0_elapse, wb.regs.freq_cnt0_cycles)
f1 = get_freq(wb.regs.freq_cnt1_elapse, wb.regs.freq_cnt1_cycles)

print("osc0: {:.2f} Htz".format(f0))
print("osc1: {:.2f} Htz".format(f1))

wb.close()