#!/usr/bin/env python3

import sys
import json

def inv(n, bits):
        new = 0
        for i in range(bits):
                new |= ((n >> i)&1) << (bits - i - 1)
        return new

with open(sys.argv[1]) as f:
	dd = json.load(f)

from math import log


def get_rev(g2):
        rr = [0 for _ in range(1 << g2)]
        rr[0] = 0
        rr[1] = 1
        for k in range(2,g2+1):
                r = 1 << k
                l = r >> 1
                for i in range(l):
                        rr[i] *= 2
                for i in range(l, r):
                        rr[i] = rr[i - l] + 1
        return rr


for lg2 in range(2,10):
        rev = get_rev(lg2)
        rev = dd
        lg2 = int(log(len(dd)) / log(2))
        for ii in range(len(rev)):
                i = rev[ii]
                j = rev[i]
                iv = inv(i, lg2)
                if iv != j:
                        print(lg2, ii, i, j, iv)
