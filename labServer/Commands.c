#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "txServer.h"
#include "streamer.h"
#include "udp.h"

#define BUFLEN 2048
#define BUF_SIZE 1024		/* Serial buffer size */
#define PORT 59000

extern int serial_open(int, int), serial_close(int);
extern unsigned int serial_fd[2];
extern char *one_argument (char *, char *);
extern FILE *config;

char buf[32];

void diep(char *s)
{
  perror(s);
  exit(1);
}

int Status()
{
  time_t clk = time(NULL);
  TELLUSER("TIME: %u %s", (unsigned)clk, ctime(&clk));
  TELLUSER("UDP: %.3f %.3f %.3f %.6f %.6f %.3f\n", posBuffer.X, posBuffer.Y, posBuffer.Z, posBuffer.latitude, posBuffer.longitude, posBuffer.altitude);
  TELLUSER("PWR: 0x%x = %d\n", server.powerstate & 0xff, server.powerstate & 0xff);
  TELLUSER("ENV: %.1f %.1f %.2f\n", server.temps[0], server.temps[1], server.press);
  TELLUSER("MODE: %d\n", server.pointmode);
  TELLUSER("WARN: %s\n", server.warning);
  TELLUSER("VCO: %.3f\n", server.freq);
  TELLUSER("EOF\n");
  return(0);
}


int SetDAC(char *args)
{
  int DAC[4]={0x58, 0x5A, 0x5C, 0x5E}, whichDAC, value, msb, lsb;
  char arg1[BUF_SIZE], arg2[BUF_SIZE];
  args = one_argument(args, arg1);
  whichDAC = atoi(arg1);
  
  if((whichDAC > 3) || (whichDAC < 0))
    {
      TELLUSER("Invalid DAC: expecting channel 0 to 3");
      return(0);
    }
  args = one_argument(args, arg2);
  value = atoi(arg2);
  if(value < 0)
    value=0;
  else if(value > 4095)
    value=4095;
  msb = value>>8;
  lsb = value & 0xFF;
  i2c_write_word(DAC_ADDR, DAC[whichDAC], msb+(lsb<<8));
  return(0);
}

/* Report the latest UDP monitor packet */
int GetUDP()
{
  extern posBuffer_t posBuffer;

  TELLUSER("TELES_X %.6f\n", posBuffer.X);
  TELLUSER("TELES_Y %.6f\n", posBuffer.Y);
  TELLUSER("TELES_Z %.6f\n", posBuffer.Z);
  TELLUSER("GON_LAT %f\n",   posBuffer.latitude);
  TELLUSER("GON_LONG %f\n",  posBuffer.longitude);
  TELLUSER("GON_ALT %.0f\n", posBuffer.altitude);
  TELLUSER("EOF\n");

  return 0;
}

int Point(char *args)
{
  char arg1[BUF_SIZE];

  if(!args || !*args) // no arguments
    TELLUSER("Current mode = %d\n", server.pointmode)
  else
    {
    args = one_argument(args, arg1);
    server.pointmode = atoi(arg1);
    TELLUSER("Mode now = %d\n", server.pointmode)
    }

  return 0;

}

int Radio(char * args)
{
  char arg1[BUF_SIZE];

  if(!args || !*args) // no arguments
    {
      if(pid>0){
        TELLUSER("Radio is currently 1\n");
        }
      else
        {
        TELLUSER("Radio is currently 0\n");
        }
      return(0);
    }
  else
    {
      args=one_argument(args, arg1);
      if(atoi(arg1) == 1){
        char *command[] = {"/home/ubuntu/ONR/txServer/transmit.py", NULL};
        pid = popen2(command, NULL, NULL);
        TELLUSER("Radio is set 1\n");
      }
      else if(atoi(arg1) == 0){
        TELLUSER("Radio is set 0\n");
        kill(pid, SIGTERM);
        pid=0;
      }
    }
  return(0);
}

int SetPower(char * args)
{
  char arg1[MAXARGSIZE], arg2[MAXARGSIZE];
  int toSwitch=0, oldstate, value, state, i, ant;
  unsigned char sendval;
  static char *switchnames[] = {"Hittite LO 5.5V ", \
                                "Hittite LO 15V  ", \
                                "Power Amp #1    ", \
                                "Power Amp #2    ", \
                                "MSB RF Switch   ", \
                                "LSB RF Switch   ", \
                                "Use VCO for RF  ", \
                                "Power Amp #3    "};

  oldstate=server.powerstate;
  if(!args || !*args)			   //No arguments - Display Current State
    {
      value = i2c_read(DIO_ADDR, 0x01); 
      server.powerstate=(unsigned char)value;
      TELLUSER("Current power setting: 0x%x\n", (value & 0xff));
      for(i=0;i<8;i++)
        {
          TELLUSER("%s:  %d\n", switchnames[i], (value & 0x01));
          value=value>>1;
        }
      TELLUSER("EOF\n");
    }
  else
    {
      args=one_argument(args,arg1);
      args=one_argument(args,arg2);
      if(!*arg2)		       
	TELLUSER("Error: 1 arg received.  No args will read state, 2 args will set it.")
      else
        {
          sscanf(arg2, "%d", &state);
          if (!strncmp (arg1, "syn", 3))
              toSwitch=SYNTH55+SYNTH15;
          else if(!strncmp( arg1, "vco55", 5))
              toSwitch=SYNTH55;
          else if(!strncmp( arg1, "vco15", 5))
              toSwitch=SYNTH15;
          else if(!strncmp( arg1, "pa1", 3))
              toSwitch=PA1;
          else if(!strncmp( arg1, "pa2", 3))
              toSwitch=PA2;
          else if(!strncmp( arg1, "pa3", 3))
              toSwitch=PA3;
	  else if(!strncmp( arg1, "PAs", 3))
	    toSwitch=PA1+PA2+PA3;
          else if(!strncmp( arg1, "lsb", 3))
              toSwitch=SW_LSB;
          else if(!strncmp( arg1, "msb", 3))
              toSwitch=SW_MSB;
          else if(!strncmp( arg1, "ant", 3))
	    {
	      // weird special case -- select antenna output here
	      switch(state)
		{
		case 1:
		  server.powerstate = server.powerstate & ~SW_LSB;
		  server.powerstate = server.powerstate & ~SW_MSB;
		  break;
		case 2:
		  server.powerstate = server.powerstate | SW_LSB;
		  server.powerstate = server.powerstate & ~SW_MSB;
		  break;
		case 4:
		  server.powerstate = server.powerstate | SW_LSB;
		  server.powerstate = server.powerstate | SW_MSB;
		  break;
                case 5:  //special case, increment to next, 1,2,4
                  ant = (((server.powerstate>>4)&1)<<1) + ((server.powerstate>>5)&1) + 1;
		  if(ant==1) //go to ant #2
		    {
                      server.powerstate = server.powerstate | SW_LSB;
                      server.powerstate = server.powerstate & ~SW_MSB;
		    }
                  if(ant==2) //go to ant #4
		    {
                      server.powerstate = server.powerstate | SW_LSB;
                      server.powerstate = server.powerstate | SW_MSB;
		    }
                  if(ant==4) //go to ant #1
		    {
                      server.powerstate = server.powerstate & ~SW_LSB;
                      server.powerstate = server.powerstate & ~SW_MSB;
		    }
                  break;
		default:
		  TELLUSER("Error: Unknown antenna switch. OK values are 1, 2, or 4.\n");
		  return(0);
		}
	    }
	      else if(!strncmp( arg1, "usevco", 6))
		toSwitch=USEVCO;
          else
            {
              TELLUSER("Error: Unknown DC/DC switch!\n")
              return(0);
            }
         }
       if(state && toSwitch)
         server.powerstate = server.powerstate | toSwitch;
       else if(!state && toSwitch)
         server.powerstate = server.powerstate & ~toSwitch;
       sendval=(unsigned char)server.powerstate;
       TELLUSER("Setting MUX to %d = 0x%x\t", sendval, sendval);
       i2c_write(DIO_ADDR, 0x01, sendval);	//Set Hardware switches to PowerState
       TELLUSER("Old: 0x%x  Switching: 0x%x   New: 0x%x\n", (oldstate & 0xff), toSwitch, (server.powerstate & 0xff)); 
    }
  return(0);

}

int ReadADC(char *args){ 

  char arg1[MAXARGSIZE];
  int value;
  float volts,temp;

  if(!args || !*args) // no arguments
    {
      TELLUSER("Error: you didn't select a mux to read! (ch 1 thru 4)\n");
      return(0);
    }
  else
    {
      args=one_argument(args,arg1);
      value = read_adc(atoi(arg1));
      volts = 2.048*value/32768.0;
      if(atoi(arg1)==2){
        temp = -1.*(7.575*volts-12.5);
        TELLUSER("Voltage = %4.3f Pressure = %6.3f inH2O\n", volts, temp);
        return(0);
      }
      temp = 200.*2.048*(value)/32768.-273.15;
      TELLUSER("Voltage = %4.3f   Temperature = %6.3f C\n", volts, temp);
    }
  return(0);
}

int InitSynth(void)
{
  int i, a, b, c, x;
  int s;
  char str[24];
  uint32_t bufInt;
  char buffer[4];
  FILE *fp;

  fp = fopen("/home/ubuntu/ONR/txServer/VCO_REG.txt", "r");

  s = i2c_read(MUX_ADDR, 0x01); 
  s &= ~(1 << 4);  //unset SSA0
  s &= ~(1 << 5);  //unset SSA1
  i2c_write(MUX_ADDR, 0x01, s);	//Set SPI MUX to Synth

  int spi = spi_init("/dev/spidev0.0");
  for(i=0;i<14;i++)
  {
    fscanf(fp, "%d 0x%2x%2x%2x\t%s", &x, &a, &b, &c, str);
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

  s |=   1 << 4;  //  set SSA0
  s &= ~(1 << 5); //unset SSA1
  i2c_write(MUX_ADDR, 0x01, s);	//Set SPI MUX back to CIP
  
  server.freq = 8650.;
  close(spi);
  fclose(fp);
  return(0);
}


int SetFreq(char *args){

  int s;
  int fxtal = 50e6;
  char arg1[MAXARGSIZE];
  float freq;
  uint32_t Nint, Nfrac;
  uint32_t bufInt1, bufInt2;
  char bufNint[4];
  char bufNfrc[4];

  if(!args || !*args)  // no arguments
    {
      TELLUSER("Current freq = %.6f MHz\n", server.freq);
      return(0);
    }
  else
    {
      args=one_argument(args,arg1);
      server.freq = atof(arg1);
      freq = 0.5*atof(arg1)*1e6;
      Nint = floor(freq/fxtal);
      Nfrac = (pow(2,24)/fxtal)*(freq-((long long) fxtal*Nint));
      TELLUSER("Freq now = %.3f\n", server.freq);
      TELLUSER("Nint=0x%x, Nfrac=0x%x\n", Nint, Nfrac);

      bufInt1  = 3<<25;			//Set register 0x03
      bufInt1 += Nint<<1;		//with Nint
      bufNint[0] = (bufInt1)>>24;
      bufNint[1] = (bufInt1>>16)&0xFF;
      bufNint[2] = (bufInt1>>8 )&0xFF;
      bufNint[3] = (bufInt1&0xFF);

      bufInt2  = 4<<25;			//Set register 0x04
      bufInt2 += Nfrac<<1;		//with Nfrac
      bufNfrc[0] = (bufInt2)>>24;
      bufNfrc[1] = (bufInt2>>16)&0xFF;
      bufNfrc[2] = (bufInt2>>8 )&0xFF;
      bufNfrc[3] = (bufInt2&0xFF);

      int spi = spi_init("/dev/spidev0.0");

      s = i2c_read(MUX_ADDR, 0x01); 	//Set SPI MUX to Synth
      s &= ~(1 << 4);
      s &= ~(1 << 5);
      i2c_write(MUX_ADDR, 0x01, s);

      spi_generic(4, bufNint, spi); //Set Nint   1 byte reg + 3 bytes data
      spi_generic(4, bufNfrc, spi); //Set Nfrac  1 byte reg + 3 bytes data

      s |=   1 << 4;
      s &= ~(1 << 5);
      i2c_write(MUX_ADDR, 0x01, s);	//Set SPI MUX back to CIP
      close(spi);
    }
    return(0);
}


int Quit()
{
  TELLUSER ("Bye!\n");
  close (server_curFD);
  return (1);
}


