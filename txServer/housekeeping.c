#include <bsd/string.h>
#include <time.h>
#include "txServer.h"
#include "udp.h"

extern int serial_open(int, int), serial_close(int);
extern unsigned int serial_fd[2];
extern posBuffer_t posBuffer;

void *hkMonitor(void *arg)
{
  float cpuTemp;
  int dac, value, count, msb, lsb;
  char string[32];
  char databuf[128];
  FILE *fp;
  int highTempCheckCounter=0;
  
  server.shutdown=0;
  while(1)
    {
      highTempCheckCounter=(highTempCheckCounter+1)%12;
      cpuTemp=getCPUtemp();
      count=0;
      value = read_adc(2); // should be the pressure sensor
      server.press = -1.*(7.575*(2.048*(value)/32768.)-12.5);
      value = read_adc(3);  // should be the VCO temp
      server.temps[0] = 200.*2.048*(value)/32768.-273.15;
      value = read_adc(4); // should be the PA temp
      server.temps[1] = 200.*2.048*(value)/32768.-273.15;
	
	//sanity check temp values against Tegra CPU temp
	if(abs(cpuTemp-server.temps[0])>15) //deviation too high!
		server.temps[0]=cpuTemp+0.01;
	if( (cpuTemp-server.temps[1]>10) || (server.temps[1]-cpuTemp>20) ) //deviation too high!
		server.temps[1]=cpuTemp+5.01;
	
      if(server.temps[1] > HIGHTEMP  && highTempCheckCounter==0)  
	{
	  server.powerstate = server.powerstate & ~(PA1+PA2+PA3);
	  i2c_write(DIO_ADDR, 0x01, (unsigned char)server.powerstate);	  
	  snprintf(server.warning, 63, "PAs are too hot, turned off.");
	  server.shutdown=1;
	}
      else if(server.temps[1] < LOWTEMP && server.shutdown) 
	{
	  server.powerstate = server.powerstate | (PA1+PA2+PA3);
	  i2c_write(DIO_ADDR, 0x01, (unsigned char)server.powerstate);	  
	  snprintf(server.warning, 63, "PAs are cool enough, turning back on.");
	  server.shutdown=0;
	}
      else if(server.temps[1] > LOWTEMP)
	{
	  snprintf(server.warning, 63, "PAs are hot.");
	  if(server.shutdown)
	    {
	      snprintf(string, 31, " PAs remain off.");
	      strlcat(server.warning, string, 63);
	    }
	}
      else if(server.temps[1] < 0)
	snprintf(server.warning, 63, "PAs are cold.");
      else
	server.warning[0]='\0';  // null string & append as needed below
      // now compute VCO DAC and adjust if needed
      dac = (int)((950.0-(-0.26141*server.temps[0])-533.5)/0.15982);
      if(dac != server.vcodac)
      //if(dac != server.vcodac && server.temps[0] > 0)
	{
	  msb = dac>>8;
	  lsb = dac & 0xFF;
	  i2c_write_word(DAC_ADDR, 0x5E, msb+(lsb<<8));
	  snprintf(string, 31, " VCO DAC set to %d.", dac);
	  strlcat(server.warning, string, 63);
	  server.vcodac = dac;
	}

      // now spew it over the serial port if available, try a few times if "busy"
      while(server.serial_open && (count < 10))
	usleep(500000);
      if(count<10)
	{
	  serial_open(TELEMETRY, 9600);
          fp = fopen("/home/ubuntu/ONR/txServer/HKlog.log", "a");
	  
	  time_t clk = time(NULL);
	  snprintf((char *)databuf, 100, "TIME: %u %s", (unsigned)clk, ctime(&clk));
	  fprintf(fp, "TIME: %u %s", (unsigned)clk, ctime(&clk));
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "UDP: %.3f %.3f %.3f %.6f %.6f %.3f\n", posBuffer.X, posBuffer.Y, posBuffer.Z, posBuffer.latitude, posBuffer.longitude, posBuffer.altitude);
	  fprintf(fp, "UDP: %.3f %.3f %.3f %.6f %.6f %.3f\n", posBuffer.X, posBuffer.Y, posBuffer.Z, posBuffer.latitude, posBuffer.longitude, posBuffer.altitude);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "PWR: 0x%x = %d\n", server.powerstate & 0xff, server.powerstate & 0xff);
	  fprintf(fp, "PWR: 0x%x = %d\n", server.powerstate & 0xff, server.powerstate & 0xff);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "ENV: %.2f %.2f %.2f\n", server.temps[0], server.temps[1], server.press);
	  fprintf(fp, "ENV: %.1f %.1f %.2f\n", server.temps[0], server.temps[1], server.press);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  if(pid==0)
	    snprintf((char *)databuf, 100, "MODE: %d 0\n", server.pointmode);
	  if(pid>0)
	    snprintf((char *)databuf, 100, "MODE: %d 1\n", server.pointmode);
	  fprintf(fp, "MODE: %d\n", server.pointmode);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "WARN: %s\n", server.warning);
	  fprintf(fp, "WARN: %s\n", server.warning);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "VCO: %.3f\n", server.freq);
	  fprintf(fp, "VCO: %.3f\n", server.freq);
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  snprintf((char *)databuf, 100, "EOF\n");
	  fprintf(fp, "EOF\n");
	  write(serial_fd[TELEMETRY], databuf, strlen(databuf));

	  serial_close(TELEMETRY);
          fclose(fp);
	}
      
      sleep(5);
    }
  pthread_exit(0);
}
