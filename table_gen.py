#!/usr/bin/env python3
from math import sin, pi, sqrt, log
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

sine = [sin(2*pi*x/N) for x in range(N//2)]
sine = [conv_32b(x) for x in sine]

#https://en.wikipedia.org/wiki/A-weighting
def r_a(f):
    return 12194**2*f**4/((f**2+20.6**2)*sqrt((f**2+107.7**2)*(f**2+737.9**2))*(f**2+12194**2))

def a(r):
    return 20*log(r)/log(10)+2

f = 8_000
BINS = 128 // 2 - 1
step = f / BINS
w = [step / 2]
for i in range(BINS - 1):
    w.append(w[i] + step)

w = [int(a(r_a(x))) for x in w]

template="""\
#ifndef TABLES_H
#define TABLES_H

const int32_t sin_table[SAMPLES/2] = {
%s
};

const int8_t weights[BINS] = {
%s
};
#endif
"""

with open("src/tables.h", "w") as f:
    f.write(template % (
        mk_string(sine),
        mk_string(w)
    ))
