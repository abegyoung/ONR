// $LastChangedDate: 2016-08-26 00:43:49 +0000 (Fri, 26 Aug 2016) $
// $Rev: 166 $      $Author: obs $

/* A place to put the udpMonitor() thread which gets the current
*  pointing from the MAX3 udp stream.
*/  

#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <sys/sysctl.h> 
#include <sys/ioctl.h>
#include "txServer.h"
#include "streamer.h"
#include "udp.h"

void *udpMonitor(void *arg) {
  struct sockaddr_in sa;
  struct ip_mreq mreq;
  int sock = -1, count=0, oldest = 0;
  long ret;
  socklen_t len;
  //  int curPosition;
    
  while(1) {
    if (sock == -1) {
      /* No socket open yet */
      if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        {
          perror("Error: could not create UDP client");
          sleep(3);             /* Wait? */
          continue;
        }
      // Set non-blocking 
      if( (ret = fcntl(sock, F_GETFL, NULL)) < 0) {
        perror("F_GETFL failed");
        close(sock);
        sock = -1;
        sleep(3);
        continue;
      }
      ret |= O_NONBLOCK;
      if( fcntl(sock, F_SETFL, ret) < 0) {
        perror("F_SETFL failed");
        close(sock);
        sock = -1;
        sleep(3);
        continue;
      }

      setsockopt(sock, IPPROTO_UDP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq));

      sa.sin_family = AF_INET;
      sa.sin_port = htons(STREAM_PORT);
      sa.sin_addr.s_addr = INADDR_ANY;

      if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind");
        close(sock);
        sock = -1;
        sleep(3);
        continue;
      }
    }                       /* End: if (sock == -1) */

    len = sizeof(sa);
    while (count < 100)
      {
        ret = recvfrom(sock, (struct posBuffer_t *) &posBuffer , sizeof(posBuffer),
                       0, (struct sockaddr *)&sa, &len);
        if(ret<=0)
          {
            usleep(UDP_PAUSE);  /* No packet ready yet */
            count++;
          }
        else
          {
            //curPosition = oldest++;
            if (oldest >= 3) oldest = 0;
            count = 0;
            usleep(UDP_PAUSE);  /* Wait for the next packet */
            continue;
          }
      }
    if  (count>=100) {
      /* UDP stream stopped?  Try closing the socket */
      close(sock);
      sock = -1;
      count = 0;
      /*if (curPosition >= 0 && time(NULL) - UDP_STALE >
                             posBuffer[curPosition].here.now.tv_sec) {
        // Last UDP packet is too stale 
        curPosition = 1;
      }*/
    }
  }
}

