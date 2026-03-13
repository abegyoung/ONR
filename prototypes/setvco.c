#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "spi.h"

#define MUX_ADDR 0x20
uint32_t i2c_read(int addr, int reg);
void i2c_write(int addr, int reg, int a);

int main(void){

  int i, a, b, c, x;
  int s;
  char str[24];
  uint32_t bufInt;
  char     buffer[4];
  FILE *fp;

  s = i2c_read(MUX_ADDR, 0x01);
  s &= ~(1 << 4);
  s &= ~(1 << 5);
  i2c_write(MUX_ADDR, 0x01, s); //Set SPI MUX to Synth

  int spi = spi_init("/dev/spidev0.0");
  fp = fopen("VCO_REG.txt", "r");
  for(i=0;i<14;i++){
    fscanf(fp, "%02d 0x%2x%2x%2x\t%s", &x, &a, &b, &c, str);
    bufInt  = x<<25;
    bufInt += a<<17;
    bufInt += b<<9;
    bufInt += c<<1;
    buffer[0] = (bufInt)>>24;
    buffer[1] = (bufInt>>16)&0xFF;
    buffer[2] = (bufInt>>8 )&0xFF;
    buffer[3] = (bufInt&0xFF);
    spi_generic(4, buffer, spi);
  }

  s |=   1 << 4;
  s &= ~(1 << 5);
  i2c_write(MUX_ADDR, 0x01, s); //Set SPI MUX back to CIP

  close(spi);
  fclose(fp);
  return(0);
}

//make 0x50894C into 0xA1, 0x12, 0x98
//0x50<<1 0x89<<1 0x4C<<1
