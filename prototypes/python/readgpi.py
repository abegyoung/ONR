#!/usr/bin/python
# Reads 16 registers from Hittite VCO.

import sys
import time
import struct
import spidev

spi= spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz=3200000
spi.mode=0b00
spi.cshigh=True

data = spi.readbytes(2)
msb = data[0]
lsb = data[1]
#print "{0:08b}".format(msb)
#print "{0:08b}".format(lsb)
print hex(msb)
print hex(lsb)

spi.close()
