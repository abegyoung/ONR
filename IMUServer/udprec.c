#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 2048
#define NPACK 100
#define PORT 59000
#define TRUE 1

typedef struct
{
  float X;
  float Y;
  float Z;
  float latitude;
  float longitude;
  float altitude;
} posBuffer_t;

void diep(char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  int s, i; 
  struct sockaddr_in si_me, si_other;
  struct ip_mreq mreq;
  int slen=sizeof(si_other);
  char buf[BUFLEN];
  char *token;

  posBuffer_t posBuffer;

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  setsockopt(s, IPPROTO_UDP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq));

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me))==-1)
    diep("bind");

  while(TRUE){

    //recvfrom(s, buf, BUFLEN, 0, &si_other, &slen);
    recvfrom(s, (struct posBuffer_t *) &posBuffer, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
    slen=sizeof(si_other);

    printf("%.3f ", posBuffer.X);
    printf("%.3f ", posBuffer.Y);
    printf("%.3f ", posBuffer.Z);
    printf("%.8f ", posBuffer.latitude);
    printf("%.8f ", posBuffer.longitude);
    printf("%.3f\n", posBuffer.altitude);
    
  }

  close(s);
  return 0;
}
