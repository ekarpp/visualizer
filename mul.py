#!/usr/bin/env python3

import json
import sys

with open(sys.argv[1]) as f:
	dd = json.load(f)


for mul in dd:
	a = mul[0]
	b = mul[1]
	c = mul[2]
	ng = 0
	if a < 0:
		ng += 1
	if b < 0:
		ng += 1
	ccc = abs(a) * abs(b)
	ccc >>= 31
	if ng == 1:
		ccc = -ccc
	sin = b / 0x7FFFFFFF
#	if b < 0:
#		print(f"{b}")
	cc = int(a * sin)
	if c > cc + 1 or c < cc - 1:
		print(f"{a} * {sin} | {b} = {a*sin} = {c} = {ccc}")
