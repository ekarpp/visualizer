#!/usr/bin/env python3
import serial
import sys

ser = serial.Serial('/dev/ttyACM1', 9600)
while True:
	msg = input(">")
	msg = msg.replace("\\r", "\r\n")
	msg = msg.replace("\\z", chr(26))
	msg = msg.replace("\\n", "\n")
	msg += "\n"
	if msg == "q\n":
		break
	ser.write(msg.encode("ascii"))
ser.close()
