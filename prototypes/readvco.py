#!/usr/bin/python
# Reads 16 registers from Hittite VCO.

import sys
import time
import struct
import spidev
import smbus

bus = smbus.SMBus(0)

spi= spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz=3200000
spi.mode=0b00
spi.cshigh=True

s=bus.read_i2c_block_data(0x20,0x01,1)
s[0] &= ~(1 << 4);	#clear bit 4 for SPI bus #1
s[0] &= ~(1 << 5);	#clear bit 5 for SPI bus #1
bus.write_i2c_block_data(0x20,0x01,s)

for reg in range(0,16):
  addr = (reg<<1 ^ 0x80)
  data = spi.xfer2([addr,0x00,0x00,0x00])
  print hex(reg), hex((data[1]<<16)+(data[2]<<8)+data[3])

s[0] |=   1 << 4;	#set bit 4 for SPI bus #2
s[0] &= ~(1 << 5);	#clear bit 5 for SPI bus #1
bus.write_i2c_block_data(0x20,0x01,s)

bus.close()
spi.close()
