import sys

"""
A very quick and dirty script to strip down a VCD file to a subset of the signals,
it's very very brittle because it assumes certain tokens appear on the same line.
"""

keep_sym = set()
inside_dumpvars = False


def keep(signal_name):
    return signal_name.startswith('pads_')


for l in sys.stdin.readlines():
    l = l.strip()
    tok = l.split()
    if tok[0] == '$var':
        if keep(tok[4]):
            keep_sym.add(tok[3])
        else:
            continue
    elif tok[0] == '$dumpvars':
        inside_dumpvars = True
    if inside_dumpvars:
        if l[0].isdecimal() or l[0] == 'b':
            if l[-1] not in keep_sym:
                continue
    print(l)
