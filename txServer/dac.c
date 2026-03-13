#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

int file;
int a, msb,lsb;
unsigned char values[11];

void main(int argc, char **argv){

a  = atoi(argv[1]);
file = open("/dev/i2c-0", O_RDWR);

ioctl(file, I2C_SLAVE, 0x60);

msb = a>>8;
lsb = a&0xFF;

values[0]  = msb;
values[1]  = lsb;
values[2]  = 0x42;
values[3]  = msb;
values[4]  = lsb;
values[5]  = 0x44;
values[6]  = msb;
values[7]  = lsb;
values[8]  = 0x46;
values[9]  = msb;
values[10] = lsb;

i2c_smbus_write_i2c_block_data(file, 0x40, 11, values);

close(file);

}

