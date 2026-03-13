#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

int a;
int file;
int addr = 0x20;

void main(int argc, char **argv){

a=atoi(argv[1]);
file = open("/dev/i2c-0", O_RDWR);

ioctl(file, I2C_SLAVE, addr);

//i2c_smbus_write_byte_data(file,0x03,0x00);
usleep(10000);
i2c_smbus_write_byte_data(file,0x01,a);

close(file);

}

