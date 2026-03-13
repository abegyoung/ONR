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
#include <time.h>
#include "serial.h"

#define SRV_IP "127.0.0.1"

#define BUFLEN 256
#define NPACK 20000
#define PORT 59000

#define TRUE 1
#define FALSE 0

volatile sig_atomic_t running = TRUE;
extern int serial_open(), serial_close();

char buf1[512];
char buf2[512];

float gpstime;
float prevtime;
int   gpsweek;

typedef struct 
{
  float X;
  float Y;
  float Z;
  float longitude;
  float latitude;
  float altitude;
} posBuffer_t;

void diep(char *s)
{
  perror(s);
  exit(1);
}

void stop(int signum)
{
  running = FALSE;
}

int main(void)
{

  int i;
  int timeset = 0;
  char *token;
  struct sockaddr_in si_other;
  int s, slen=sizeof(si_other);
  int optval;
  posBuffer_t posBuffer;

  /* Zero everything in the position buffer */
  gpstime = 0.0;
  prevtime = 0.0;
  gpsweek = 0;
  posBuffer.X = 0.0;
  posBuffer.Y = 0.0;
  posBuffer.Z = 0.0;
  posBuffer.latitude  = 0.0;
  posBuffer.longitude = 0.0;
  posBuffer.altitude  = 0.0;


  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  optval = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &si_other.sin_addr)==0){
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  serial_open();

  while(running){

    signal(SIGINT, stop);

    read(serial_fd, buf1, 512);
    token = strtok(buf1, "VNINS");
    for(i=0;i<12;i++){
      //if(token != NULL){
        token = strtok(NULL, ",");
        if(token!=NULL){
          if(i==1) gpstime = strtof(token, NULL);
          if(i==2) gpsweek = strtof(token, NULL);
          if(i==4) posBuffer.X = strtof(token, NULL);
          if(i==5) posBuffer.Y = strtof(token, NULL);
          if(i==6) posBuffer.Z = strtof(token, NULL);
          if(i==7) posBuffer.latitude  = strtof(token, NULL);
          if(i==8) posBuffer.longitude = strtof(token, NULL);
          if(i==9) posBuffer.altitude  = strtof(token, NULL);
        }
        token = NULL;
      //}
    }
    //posBuffer.altitude = 45229.; //for testing

    if( (!timeset) && (gpsweek>0) ){
      time_t now = gpstime + gpsweek*7*24*60*60 + 315964800;
      stime(&now);
      timeset = 1;

      FILE *fp;
      fp = fopen("/home/ubuntu/ONR/IMUServer/status.txt", "a");
      fprintf(fp,"time set at %.0f sec after GPS start\n", prevtime);
      fclose(fp);
    }

    prevtime = gpstime;

    if(posBuffer.X < 0.)
       posBuffer.X += 360.0;

    if(sendto(s, (struct posBuffer_t*) &posBuffer, BUFLEN, 0, (struct sockaddr *) &si_other, slen)==-1)
      printf("die\n");

  }

  //Quit
  close(serial_fd);
  close(s);
  printf("exiting!\n");
  return 0;
}
