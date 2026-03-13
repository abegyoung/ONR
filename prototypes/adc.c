#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

int msb,lsb;
int file;
int addr = 0x68;
__s32 res;

void main(int argc, char **argv){

file = open("/dev/i2c-0", O_RDWR);

ioctl(file, I2C_SLAVE, addr);

i2c_smbus_write_byte(file,0x88);
usleep(100000);
res = i2c_smbus_read_word_data(file,0x00);
msb = res & 0xFF;
lsb = (res & 0xFF00)>>8;
float temp = 200.*2.048*((msb<<8) + lsb)/32768.-273.;

printf("%f\n", temp);

close(file);

}

