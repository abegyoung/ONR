#!/usr/bin/python
import sys
import smbus

bus=smbus.SMBus(0)
ch=int(sys.argv[1])

#read from DIO pins
if(ch==0 and len(sys.argv)==2):
 bus.write_i2c_block_data( 0x21,0x03,[0x00])	#set pins as outputs
 s=bus.read_i2c_block_data(0x21,0x01,1)	        #read output register
 print "{0:b}".format(s[0])
 bus.close()
 exit()

#write to DIO pins
if(ch==0 and len(sys.argv)==3):
 a=int(sys.argv[2])
 bus.write_i2c_block_data(0x21,0x03,[0x00])	#set all pins as outputs
 bus.write_i2c_block_data(0x21,0x01,[a])
 bus.close()
 exit()

#bit 0 = A/D MUX A0
#bit 1 = A/D MUX A1
#bit 2 = A/D MUX A2
#bit 3 = A/D MUX EN
#bit 4 = SSA0
#bit 5 = SSA1
#bit 6 = Reset SR

#write to PCF9534 controlling A/D mux, SPI bus mux, and SR reset
if(ch==1 and len(sys.argv)==3):
 a=int(sys.argv[2])
 bus.write_i2c_block_data(0x20,0x03,[0x00])	#set all pins as outputs
 bus.write_i2c_block_data(0x20,0x01,[a])
 bus.close()
 exit()

