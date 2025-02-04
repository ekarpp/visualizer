#!/usr/bin/env python3

import numpy as np
import json
import sys

with open(sys.argv[1]) as f:
	dd = json.load(f)

for pp in dd:
	iinn = [i[0] for i in pp[0]]
	print(f"min: {min(iinn)} max: {max(iinn)}")
	fftf = np.fft.fft(iinn)
	ifftf = np.fft.irfft(fftf)
	for i in range(len(pp[1]) // 2):
		print(i)
		print("(%d%s%dj)\t\t%d" % (int(np.real(fftf[i])), '' if np.imag(fftf[i]) < 0 else '+', int(np.imag(fftf[i])), int(np.imag(fftf[i])*np.imag(fftf[i]) + np.real(fftf[i]*np.real(fftf[i])))))
		print("(%d%s%dj)\t\t%d" % (pp[1][i][0], '' if pp[1][i][1] < 0 else '+', pp[1][i][1], pp[1][i][0]*pp[1][i][0] + pp[1][i][1] * pp[1][i][1]))
		print()
