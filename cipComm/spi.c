#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

char tx_buf[10];
char rx_buf[10];

//////////
// Init SPIdev
//////////
int spi_init(char filename[40])
{
   int file;
   uint8_t mode = 0x05;
   uint32_t speed=3200000;

   if ((file = open(filename,O_RDWR)) < 0)
   {
      printf("Failed to open %s.\n", filename);
      exit(1);
   }

   if(ioctl(file, SPI_IOC_WR_MODE, &mode)<0)
   {
      printf("Failed to set mode.\n");
      exit(1);
   }

   if(ioctl(file, SPI_IOC_WR_MAX_SPEED_HZ, &speed)<0)
   {
      printf("Failed to set speed.\n");
      exit(1);
   }

   return file;
}

//////////
// Read n bytes
//////////
char *spi_read(int nbytes,int file)
{
   int ret;
   struct spi_ioc_transfer xfer[1] = {0, };

   xfer[0].rx_buf = (unsigned long) rx_buf;
   xfer[0].len = nbytes;   /* Length of  command to write*/
   ret = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
   if (ret < 0)
   {
      perror("SPI_IOC_MESSAGE");
      return;
   }

   return rx_buf;
}

//////////
// Write n bytes
//////////
void spi_write(int nbytes,char value[10],int file)
{
   int i,ret;
   unsigned char tx_buf[32];
   struct spi_ioc_transfer xfer[1] = {0, };

   for(i=0; i<nbytes; i++)
     tx_buf[i] = value[i];
   xfer[0].tx_buf = (unsigned long) tx_buf;
   xfer[0].len = nbytes; /* Length of  command to write*/
   ret = ioctl(file, SPI_IOC_MESSAGE(1), xfer);
   if (ret < 0)
   {
      perror("SPI_IOC_MESSAGE");
      return;
   }

}
//////////
// Read n bytes from the address addr
//////////
char *spi_read_reg(int addr,int nbytes,int file)
{
   int ret;
   struct spi_ioc_transfer xfer[2] = { 0, };    //Two xfers

   tx_buf[0] = addr;
   xfer[0].tx_buf = (unsigned long) tx_buf;
   xfer[0].len = 1;        /* Length of  command to write*/
   xfer[0].cs_change = 0;  /* single CS between two xfers */
   xfer[1].rx_buf = (unsigned long) rx_buf;
   xfer[1].len = nbytes+1;   /* Length of Data to read */
   ret = ioctl(file, SPI_IOC_MESSAGE(2), xfer);
   if (ret < 0)
   {
      perror("SPI_IOC_MESSAGE");
      return;
   }

   return rx_buf;
}

