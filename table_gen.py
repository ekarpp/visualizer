#!/usr/bin/env python3
from math import sin, pi
from sys import argv

def conv_32b(x):
    b32 = 0x7FFFFFFF
    if x >= 0:
        return "0x%08X" % int(x * b32 + 0.5)
    else:
        msk = (b32 << 1) | 1
        v = abs(x)
        v = int(v * b32 + 0.5)
        v ^= msk
        v += 1
        return "0x%08X" % (v & msk)

def mk_string(a):
    string = " "*3
    for i in range(len(a) - 1):
        string += " %3s," % a[i]
        if i%4 == 3:
            string += "\n   "
    return "%s %3s" % (string, a[i+1])

N = (1 << int(argv[1]))

sine = [sin(2*pi*x/N) for x in range(3*N//4)]
sine = [conv_32b(x) for x in sine]

template="""\
#ifndef TABLES_H
#define TABLES_H

const int32_t sin_table[3*SAMPLES/4] = {
%s
};
#endif
"""

with open("src/tables.h", "w") as f:
    f.write(template % mk_string(sine))
