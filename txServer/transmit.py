#!/usr/bin/python
import sys
import time
import serial
ser=serial.Serial(
    port='/dev/ttyTHS0',
    baudrate=230400
)

i=0;
while (1):
  cmd="{0:0>12}".format(i)+"\n"
  ser.write(cmd);
  time.sleep(.003);
  i=i+1;
  if(i>9000000):
    i=0;
