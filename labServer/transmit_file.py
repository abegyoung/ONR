#!/usr/bin/python
import sys
import time
import serial
ser=serial.Serial(
    port='/dev/ttyTHS0',
    baudrate=230400
)

file = open("file.txt", 'r')

byte=0
while byte!="":
  byte = file.read(30)
  ser.write(byte + ' EOL\n')
  time.sleep(.005)
ser.write('EOF EOL\n')
