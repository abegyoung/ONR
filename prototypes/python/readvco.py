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

for reg in range(0,16):
  addr = (reg<<1 ^ 0x80)
  data = spi.xfer2([addr,0x00,0x00,0x00])
  print hex(reg), hex((data[1]<<16)+(data[2]<<8)+data[3])

spi.close()
