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
#include "cipComm.h"

#define PORT 9001

extern int CIP;
extern char *one_argument (char *, char *);

int TranslateCIP(int command)
{
  int mode = 0;
  int toggle = 0;
  float freq;
  int temp_var;
  char server_command[MAXARGSIZE];

  //Lower Byte == Power/RF Switch
  if( command & 0xFF ){
    //find current power state before you set one
    CIP = SendCommand("setpower\n", 4); //position (4) is current state

    if     (  (CIP & command) == 0)
      toggle = 1;  //turn ON
    else if( (CIP & command) == 1)
      toggle = 0;  //turn OFF

    switch(command){
      case 0x01 :
        sprintf(server_command, "setpower vco55 %d\n", toggle);
        break;
      case 0x02 :
        sprintf(server_command, "setpower vco15 %d\n", toggle);
        break;
      case 0x04 :
        sprintf(server_command, "setpower pa1 %d\n", toggle);
        break;
      case 0x08 :
        sprintf(server_command, "setpower pa2 %d\n", toggle);
        break;
      case 0x10 :
        sprintf(server_command, "setpower msb %d\n", toggle);
        break;
      case 0x20 :
        sprintf(server_command, "setpower lsb %d\n", toggle);
        break;
      case 0x40 :
        sprintf(server_command, "setpower usevco %d\n", toggle);
        break;
      case 0x80 :
        sprintf(server_command, "setpower pa3 %d\n", toggle);
        break;
    }

    CIP = CIP ^ command;
    int ret = SendCommand(server_command, 12); //pos'n 12 is New state

    if(CIP != ret)
      printf("something went wrong!\n");

    return 0 ;

  }

  //Upper Byte == Do something else than power switches
  else if( command & 0xFF00 ){

    switch(command){
      case 0x100 :  //reboot computer
        system("echo receved 0x100 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
	system("/sbin/reboot");
        break;

      case 0x200 :  //restart cipComm
        system("echo receved 0x200 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
	system("nohup pkill cipComm && nohup /home/ubuntu/ONR/cipComm/cipComm > /dev/null 2>&1 &");
        break;

      case 0x400 :  //kill & restart txServer
        system("echo receved 0x400 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
	system("nohup pkill txServer && nohup /home/ubuntu/ONR/txServer/txServer > /dev/null 2>&1 &");
        break;

      case 0x800 :  //change antenna select mode (Cycle, Ant1, Ant2, Ant3)
        //  HOW DO WE DO THIS?? - get MSB & LSB setpower state for RF switches, cycle to next state
        system("echo receved 0x800 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
        CIP = SendCommand("setpower\n", 4); //position (4) is current state
        
        int currentState = ((CIP >> 4) & 0x01) << 1 | ((CIP >> 5) & 0x01);
        printf("current antenna is %d\n", currentState);
        printf("changing antenna to  %d\n", currentState+1);

        int newState = (currentState + 1) & 0x03;
        int newMSB = (newState>>1) & 0x1;
        int newLSB = newState & 0x1;

        sprintf(server_command, "setpower msb %d\n", newMSB);
        SendCommand(server_command, 12); //pos'n 12 is New state
        sprintf(server_command, "setpower lsb %d\n", newLSB);
        SendCommand(server_command, 12); //pos'n 12 is New state

        break;

      case 0x1000 :  //toggle radio
        system("echo receved 0x1000 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
        toggle = SendCommand("radio\n", 4);					//get current radio state
        toggle ^= 1;
        sprintf(server_command, "radio %d\n", toggle);
        SendCommand(server_command, 4);
        break;

      case 0x2000 :  //tune VCO down
        system("echo receved 0x2000 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
        freq = SendCommand_F("setfreq\n", 4);					//get current freuqneyc
        sprintf(server_command, "setfreq %f\n", freq - .005);
        SendCommand(server_command, 4);
        break;

      case 0x4000 :  //tune VCO up
        system("echo receved 0x4000 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
        freq = SendCommand_F("setfreq\n", 4);					//get current frequency
        sprintf(server_command, "setfreq %f\n", freq + .005);
        SendCommand(server_command, 4);
        break;

      case 0x8000 :  //change pointing mode (Lighthouse, GS1, GS2)
        system("echo receved 0x8000 >> /home/ubuntu/ONR/cipComm/Comm.log");	//log
        mode = SendCommand("point\n", 4);					//get current pointing mode
        if(mode>=4) mode=-1;
        sprintf(server_command, "point %d\n", mode + 1);
        SendCommand(server_command, 4);
        break;

    }

    return 0;  

  }
}

// recvall reads a socket a byte at a time, exiting either
// when no bytes are read, or when a newline is read
// function fills pointer to calling functions buffer
// and returns number of bytesReceived
int recvall( int socket, char *buffer)
{
  int chunk = 1;
  int bytesRcvd = 0;
  
  while(chunk>0)
  {
    chunk = recv(socket, buffer+bytesRcvd, 1, 0);

    bytesRcvd += chunk;

    if (chunk ==0 || chunk == -1)
      break;

    buffer[bytesRcvd+1] = 0;
    if(strstr(buffer, "\n"))
      break;
    
  }
  buffer[bytesRcvd] = '\0';
  return bytesRcvd;
}

// Opens TCP Socket to server and sends *server_command and receives server_response
// Returns caller the int representation of the indx'th position Hex code in server_response
int SendCommand(char * server_command, int indx)
{
   
  //Variables for TCP comm
  int i,s; 
  struct sockaddr_in serv_addr;
  struct hostent *host;
  char *SRV_IP = "127.0.0.1";
  char *server_response = malloc(MAXARGSIZE);


char *token;

  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_family = AF_INET;
  host = gethostbyname(SRV_IP);
  bcopy(host->h_addr, &serv_addr.sin_addr, host->h_length);

  //Make conection to txServer and send command
  s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  connect(s, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
  send(s, server_command, strlen(server_command), 0);
  recvall(s, server_response);

  //info from server response
  token = strtok(server_response, " \t");
  for(i=0;i<(indx-1);i++)
    token = strtok(NULL, " \t");

  free(server_response);
  close(s);

  //return server response int
  return (int)strtol(token, NULL, 0);

}

// Opens TCP Socket to server and sends *server_command and receives server_response
// Returns caller the int representation of the indx'th position Hex code in server_response
float SendCommand_F(char * server_command, int indx)
{
   
  //Variables for TCP comm
  int i,s; 
  struct sockaddr_in serv_addr;
  struct hostent *host;
  char *SRV_IP = "127.0.0.1";
  char *server_response = malloc(MAXARGSIZE);


char *token;

  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_family = AF_INET;
  host = gethostbyname(SRV_IP);
  bcopy(host->h_addr, &serv_addr.sin_addr, host->h_length);

  //Make conection to txServer and send command
  s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  connect(s, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
  send(s, server_command, strlen(server_command), 0);
  recvall(s, server_response);

  //info from server response
  token = strtok(server_response, " \t");
  for(i=0;i<(indx-1);i++)
    token = strtok(NULL, " \t");

  free(server_response);
  close(s);

  //return server response int
  return (float)strtof(token, NULL);

}
