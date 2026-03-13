#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "gpio.h"
#include "cipComm.h"
#include <time.h>

#define TRUE 1
#define FALSE 0

int CIP;
unsigned int gpio = 57;
volatile sig_atomic_t running = TRUE;

FILE *fp;

/*
 * Take a 'line', copy the first 'arg' into
 * a provided array, return the rest of it.
 */
char *one_argument (char *line, char *arg) {

    char *end;

    // skip over whitespace at the start //
    while (WHITESPACE (*line)) line++;

    // get the first argument off the front //
    while (!WHITESPACE (*line) && *line != '\0') {
        *arg = *line;
        line++;
        arg++;
    }
    *arg = '\0';

    // move to the next argument //
    while (WHITESPACE (*line)) line++;

    // remove any whitespace at the end //
    for (end = line + strlen (line) - 1; WHITESPACE (*end); end--);
    *(end + 1) = '\0';

    // return what's left (the 2nd+ arguments) //
    return line;
}

void stop(int signum)
{
  running = FALSE;
}

int main(void)
{
  CIP = 0;

  //Variables for SPI comm
  int i;
  int ret;
  int cmd;
  char *buffer;
  char databuf[128];

  //set up gpio
  gpio_export(gpio);
  gpio_set_dir(gpio, INPUT_PIN);

  //read GPI and clear INT
  int file = spi_init("/dev/spidev0.0");
  buffer = (char *) spi_read(2, file);
  close(file);

  //Set Latch
  ret = i2c_read(MUX_ADDR, 0x01);
  ret &= ~(1 << 6);
  i2c_write(MUX_ADDR, 0x01, ret);  //set latch bit low
  usleep(2000);
  ret |= 1 << 6;
  i2c_write(MUX_ADDR, 0x01, ret);  //set latch bit high

  while(running){

    //Wait for gpio or sigint
    unsigned int value = HIGH;
    while((value!=LOW) && running)
    {
      gpio_get_value(gpio, &value);
      signal(SIGINT, stop);
      usleep(10000);
    } 

    usleep(250000); //Sleep the rest of the 70mSec CIP pulse
    //set SPI MUX
    ret = i2c_read(MUX_ADDR, 0x01);
    ret |=   1 << 4;  //  set SSA0
    ret &= ~(1 << 5); //unset SSA1
    i2c_write(MUX_ADDR, 0x01, ret);  //set Slave select
    usleep(2000);

    //read GPI
    int file = spi_init("/dev/spidev0.0");
    buffer = (char *) spi_read(2, file);

    close(file);

    //Reset Latch
    ret = i2c_read(MUX_ADDR, 0x01);
    ret &= ~(1 << 6);
    i2c_write(MUX_ADDR, 0x01, ret);  //set latch bit low
    usleep(2000);
    ret |= 1 << 6;
    i2c_write(MUX_ADDR, 0x01, ret);  //set latch bit high

    cmd=((buffer[0]<<8) + buffer[1]);

if(cmd){
    fp = fopen("/home/ubuntu/ONR/cipComm/Comm.log", "a");
    time_t clk = time(NULL);
    snprintf((char *)databuf, 100, "TIME: %u %s", (unsigned)clk, ctime(&clk));
    for(i=7;i>-1;i--)
      fprintf(fp,"%d", (buffer[0]>>i)&0x01);
    fprintf(fp," ");
    for(i=7;i>-1;i--)
      fprintf(fp,"%d", (buffer[1]>>i)&0x01);
    fprintf(fp, " TIME: %u %s", (unsigned)clk, ctime(&clk));
    fclose(fp);
}

    if( (cmd & -cmd) == cmd )	  //if only one bit is set
      TranslateCIP(cmd);

  }

  //Quit
  gpio_unexport(gpio);
  //printf("exiting!\n");
  return 0;
}
