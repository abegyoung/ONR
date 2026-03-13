#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

int file;
int msb,lsb;
int ch,a,com;
int addr = 0x60;

void main(int argc, char **argv){

ch = atoi(argv[1]);
a  = atoi(argv[2]);
file = open("/dev/i2c-0", O_RDWR);

ioctl(file, I2C_SLAVE, addr);

if(ch==0) com=0x58;
if(ch==1) com=0x5A;
if(ch==2) com=0x5C;
if(ch==3) com=0x5E;
msb = a>>8;
lsb = a&0xFF;
i2c_smbus_write_word_data(file,com,msb+(lsb<<8));

close(file);

}

