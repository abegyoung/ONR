/* $LastChangedDate: 2015-08-10 08:53:54 -0700 (Mon, 10 Aug 2015) $ */
/* $Rev: 58 $      $Author: peters $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define SERV_HOST "localhost"
#define SERV_PORT 9001

#define MAXLINE 1024
#define DELAY_S 0
#define DELAY_U 100000
#define READ 1
#define NOREAD 0

#ifndef _ERROR
#  define _NO_ERROR  0
#  define _ERROR    -1
#endif

#ifndef __BOOLEAN__
#  define __BOOLEAN__
   enum BOOL {FALSE, TRUE};
   typedef enum BOOL boolean;
#endif

#define MAXARGSIZE 128
#define MAXCOMMSTR 256
char logstr [MAXCOMMSTR];
char outstr [MAXCOMMSTR];

#define TELLUSER(fmt, args...) { snprintf (outstr, MAXCOMMSTR, fmt, ## args); write (server_curFD, outstr, strlen (outstr)); }
#define ADDTOLOG(fmt, args...) { snprintf (logstr, MAXCOMMSTR, fmt, ## args); appendlog (logstr); }
#define WHITESPACE(ch)         ( (ch) == ' ' || (ch) == '\t' || (ch) == '\n' || (ch) == '\r' )

// if func_call errors, report to the user //
#define TRY_FUNCTION_CHECKERROR( func_call, func_name, errorstr ) \
  errorstr [0] = 0;                                               \
  errorflag=0;                                                    \
  if (func_call == _ERROR) {                                      \
    TELLUSER ("\n\tError while attempting %s(..).", func_name);   \
    TELLUSER ("\n\tresponse: %s\n", errorstr);                    \
    ADDTOLOG ("\n\tError while attempting %s(..).", func_name);   \
    ADDTOLOG ("\n\tresponse: %s\n", errorstr);                    \
    errorflag=1;                                                  \
  }

#define SERVER_MAXCLIENTS 10

#define HIGH 1
#define LOW 0

//these are the I2C addresses
#define MUX_ADDR 0x20	//pcf9534 that sets multiplexer into ADC
#define DIO_ADDR 0x21	//pcf9534 that sets power switches
#define ADC_ADDR 0x68
#define DAC_ADDR 0x60

//these are the DCDC/ converter switches on DIO_ADDR
#define SYNTH55   0x01
#define SYNTH15   0x02
#define PA1     0x04
#define PA2     0x08
#define SW_MSB  0x10
#define SW_LSB  0x20
#define USEVCO  0x40
#define PA3     0x80

// temperature watermarks
#define HIGHTEMP 55
#define LOWTEMP 45

#define RADIO 0
#define TELEMETRY 1

struct serverstatus
{
  int powerstate;
  float temps[2];
  float press;
  int pointmode;
  int vcodac;
  int shutdown;
  int serial_open;
  char warning[64];
  float freq;
};

struct serverstatus server;

// some globals
int server_curFD, server_quit;
int pid;

void i2c_write(int addr, int reg, int a);
void i2c_write_word(int addr, int reg, int a);
void i2c_write_block(int addr, int reg, int len, const uint8_t *a);
uint32_t i2c_read(int addr, int reg);
uint32_t read_adc(int ch);
float getCPUtemp();

int spi_init(char filename[40]);
char * spi_read(int addr,int nbytes,int file);
void spi_write(int addr,int nbytes,char value[10],int file);
void spi_generic(int nbytes,char value[10],int file);

pid_t popen2(char **command, int *in_fd, int *out_fd);

//For VINCENTY
#define PI 3.14159

//Define struct type
typedef struct POINTING{
  float az;
  float el;
} POINTING_t;


//Function declaration
struct POINTING vincenty(int gs);

