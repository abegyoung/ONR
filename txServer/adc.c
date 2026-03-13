#include "txServer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

uint32_t read_adc(int ch){

  __s32 res;
  __s32 value;
  int s,msb,lsb;

  //Configure and set Mux
  int file = open("/dev/i2c-0", O_RDWR);
  ioctl(file, I2C_SLAVE, MUX_ADDR);

  //read current MUX bits and set ADC channel bits
  s  = i2c_smbus_read_byte_data(file, 0x01);
  s &= ~(1 << 0);  //clear adc mux bit
  s &= ~(1 << 1);  //clear adc mux bit
  s &= ~(1 << 2);  //clear adc mux bit
  s |= (ch-1);     //set adc mux bit
  s |= 1 << 3;	   //set MUX enable
  i2c_smbus_write_byte_data(file, 0x03, 0x0);
  usleep(1000);
  i2c_smbus_write_byte_data(file, 0x01, s);
  close(file);

  //Read two bytes of ADC
  file = open("/dev/i2c-0", O_RDWR);
  ioctl(file, I2C_SLAVE, ADC_ADDR);
  i2c_smbus_write_byte(file, 0x88);
  usleep(100000);
  res = i2c_smbus_read_word_data(file, 0x00);
  msb = res & 0xFF;
  lsb = (res & 0xFF00)>>8;
  value = (msb<<8) + lsb;
  close(file);

  //Return value
  return value;
}
