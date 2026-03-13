#!/usr/bin/python
import time
import smbus
import struct

bus=smbus.SMBus(0)

bus.write_byte(0x68,0x88)
time.sleep(0.1)
s = bus.read_i2c_block_data(0x68,0x00,3)
print 200*2.048*((s[0]<<8) + s[1])/32768.-273.

bus.close()

