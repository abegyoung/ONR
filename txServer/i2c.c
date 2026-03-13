#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

void i2c_write(int addr, int reg, int a){

   int file = open("/dev/i2c-0", O_RDWR);

   ioctl(file, I2C_SLAVE, addr);

   i2c_smbus_write_byte_data(file,reg,a);

   close(file);

}

int i2c_read(int addr, int reg){

  int file = open("/dev/i2c-0", O_RDWR);
  __s32 value;

  ioctl(file, I2C_SLAVE, addr);

  //Read one byte of DIO pin state (powerstate)
  value = i2c_smbus_read_byte_data(file, reg);

  close(file);
  return value;
}

void i2c_write_word(int addr, int reg, int a){

   int file = open("/dev/i2c-0", O_RDWR);

   ioctl(file, I2C_SLAVE, addr);

   i2c_smbus_write_word_data(file,reg,a);

   close(file);

}

void i2c_write_block(int addr, int reg, int len, const uint8_t *a){

   int file = open("/dev/i2c-0", O_RDWR);

   ioctl(file, I2C_SLAVE, addr);

   i2c_smbus_write_i2c_block_data(file, reg, len, a);

   close(file);

}
