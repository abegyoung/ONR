#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

#define SRV_IP "127.0.0.1"

#define BUFLEN 1048
#define NPACK 20000
#define PORT 59000

FILE *fp;

void diep(char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];

  fp = fopen("../Data/heatdata_0214.bin", "r");

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SRV_IP, &si_other.sin_addr)==0){
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  for(i=0; i<NPACK; i++){
    fread(buf, 4, 256, fp);
    printf("Sending packet %d\n", i);
    usleep(5000);
    if(sendto(s, buf, BUFLEN, 0, &si_other, slen)==-1)
      diep("sendto()");
  }

  close(s);
  return 0;
}
