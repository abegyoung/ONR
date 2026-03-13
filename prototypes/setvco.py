#!/usr/bin/python
#Sets 16 registers on Hittite VCO.

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

val=[None]*16
val[0]=0x485901		#ChipID (readonly)
val[1]=0x86		#RST
val[2]=0x1		#REFDIV
val[3]=0x56		#Nint
val[4]=0x800000		#Nfrac
val[5]=0x50894c		#Seed
val[6]=0x702f87		#SD CFG
val[7]=0x4fa		#LKD/CSP
val[8]=0x31df		#Analog EN
val[9]=0x900380		#CP register
val[10]=0x0		#CP Op Amp
val[11]=0x72		#PFD
val[12]=0x0		#VCO SPI
val[13]=0xa		#GPI_SPI_RDIV
val[14]=0x0		#
val[15]=0x1		#LD State

s=bus.read_i2c_block_data(0x20,0x01,1)
s[0] &= ~(1 << 4);      #clear bit 4 for SPI bus #1
s[0] &= ~(1 << 5);      #clear bit 5 for SPI bus #1
bus.write_i2c_block_data(0x20,0x01,s)

for reg in range(1,16):
  cmd2=(reg<<24) + ((val[reg]>>16)<<16) + (((val[reg]>>8)&0xFF)<<8) + (val[reg]&0xFF)<<1
  cmd =[(cmd2>>24)&0xFF, (cmd2>>16)&0xFF, (cmd2>>8)&0xFF, cmd2&0xFF]
  spi.xfer2(cmd)
  print (hex(reg), hex((cmd2>>1)&0xFFFFFF))

s[0] |=   1 << 4;       #set bit 4 for SPI bus #2
s[0] &= ~(1 << 5);      #clear bit 5 for SPI bus #1
bus.write_i2c_block_data(0x20,0x01,s)

bus.close()
spi.close()
