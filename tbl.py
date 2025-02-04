#!/usr/bin/env python3

import json
import sys
from math import cos, sin, pi

with open(sys.argv[1]) as f:
	dd = json.load(f)

b32 = 0x7FFFFFFF

SAMPLES = 256

for tb in dd:
	idx = tb[0]
	re = tb[1]
	im = tb[2]
	s = im / b32
	c = re / b32

	ss = sin(-2 * pi * idx / SAMPLES)
	cc = cos(-2 * pi * idx / SAMPLES)

	#if abs(c) > abs(cc) + 1 or abs(s) > abs(ss) + 1:
	print(f"{c} = {cc}")
	print(f"{s} = {ss}")
	print()
