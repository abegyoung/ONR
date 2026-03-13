// $LastChangedDate: 2016-08-26 00:43:49 +0000 (Fri, 26 Aug 2016) $
// $Rev: 166 $      $Author: obs $

/* A place to put the udpMonitor() thread which gets the current
*  pointing from the MAX3 udp stream.
*/  

#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h> 
#include "txServer.h"
#include "streamer.h"
#include "pointing.h"
#include <sqlite3.h>
#include <stdlib.h>	//Adding to switch between antennas by issue ./mux.py 0 ###

// ==== Early text LUT implentation ==== //
int * getDAC(int az, int el)
{

  int i;
  static int r[3];
  FILE *fp = fopen("/home/ubuntu/ONR/txServer/pointing.txt", "r");

  int DAC[3][1024];

  for(i=0; i<360; i++){
    fscanf(fp, "%d %d %d\n", &DAC[0][i], &DAC[1][i], &DAC[2][i]);
  }
  fclose(fp);

  r[0] = DAC[0][az];
  r[1] = DAC[1][az];
  r[2] = DAC[2][az];

  return r;
  
}

// ==== Sets the DAC values ==== //
int setDAC(int DAC1, int DAC2, int DAC3)
{
  unsigned char values[8];

  values[0] = (DAC1>>8)&0xF;
  values[1] = DAC1&0xFF;
  values[2] = 0x42;
  values[3] = (DAC2>>8)&0xF;
  values[4] = DAC2&0xFF;
  values[5] = 0x44;
  values[6] = (DAC3>>8)&0xF;
  values[7] = DAC3&0xFF;

  i2c_write_block(DAC_ADDR, 0x40, 8, values);

  return 0;

}

// ==== SQL Database LUT implentation ==== //
static int callback(void *NotUsed, int argc, char **argv, char **azColName){

  //SELECT call returns a single row.  columns 2,3,4 are DAC1,2,3
  //printf("\t%d\t%d\t%d\n", atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

  //Set the DAC with those
  setDAC(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

  return 0;

}

// ============================ Main Thread ===========================//
void *pointing(void *arg) {

  int i, max, ant;
  int DAC1=0, DAC2=0, DAC3=0;
  int DAC_tfa[3][4800];
  int DAC_bfo[3][4800];
  int DAC_tfi[3][4800];

  sqlite3 *db;
  char *zErrMsg = 0;
  char stmt[256];
  char *antennaName;
  int rc;

// ========== Open SQL Database ========//
  rc = sqlite3_open("/home/ubuntu/ONR/txServer/LookUpTable.db", &db);

// ========= Read lighthouse files =======//
  FILE *fp_tfa = fopen("/home/ubuntu/ONR/txServer/lighthouse_tfa.txt", "r");
//FILE *fp_bfo = fopen("/home/ubuntu/ONR/txServer/lighthouse_bfo.txt", "r");
  FILE *fp_tfi = fopen("/home/ubuntu/ONR/txServer/lighthouse_tfi.txt", "r");

  i=0;
  while((fscanf(fp_tfa, "%d %d %d\n", &DAC_tfa[0][i], &DAC_tfa[1][i], &DAC_tfa[2][i]) != EOF) || (i>4799))
    {
      i++;
    }
  max=i;
  fclose(fp_tfa);

//  i=0;
//  while((fscanf(fp_bfo, "%d %d %d\n", &DAC_bfo[0][i], &DAC_bfo[1][i], &DAC_bfo[2][i]) != EOF) || (i>4799))
//    {
//      i++;
//    }
//  max=i;
//  fclose(fp_bfo);

  i=0;
  while((fscanf(fp_tfi, "%d %d %d\n", &DAC_tfi[0][i], &DAC_tfi[1][i], &DAC_tfi[2][i]) != EOF) || (i>4799))
    {
      i++;
    }
  max=i;
  fclose(fp_tfi);

// ==================== And Here is the thread! ============== //
  for(;;){

    POINTING_t result;
    // check antenna switch.  Only switch antenna after done with LUT
    ant = (((server.powerstate>>4)&1)<<1) + ((server.powerstate>>5)&1) + 1;

    if(server.pointmode){
      result = vincenty(server.pointmode);
      //printf("az %f\tel %f", result.az, result.el);

      if(ant==1) antennaName="TopFedAcrylic";
      if(ant==2) antennaName="BottomFedOmni";
      if(ant==3) antennaName="TopFedInflatable";
      if(ant==4) antennaName="TopFedInflatable";

      sprintf(stmt, "SELECT * FROM %s WHERE AZ==%d ORDER BY ABS(%.1f-EL) LIMIT 1;", antennaName, (int) (1.+result.az), 90-fabs(result.el)); 
      rc = sqlite3_exec(db, stmt, callback, 0, &zErrMsg);

      // Comment out text LUT implementation
      //int *p;
      //p = getDAC( (int) result.az, (int) result.el);
      //DAC1 = *(p+0);
      //DAC2 = *(p+1);
      //DAC3 = *(p+2);

      // Comment out - SQL database callback func now sets DAC
      //setDAC(DAC1, DAC2, DAC3);

      usleep(PAUSE); //20 msec (INS Solution is 40 Hz)
    }

    else{  // mode = 0, assume lighthouse
      // loop over the values in the file, line by line
      system("/home/ubuntu/ONR/txServer/swapAnt1.sh");
      ant=1;
      for(i=0; i<max; i++){

        if(ant==1){ //Top Fed Acrylic
          setDAC(DAC_tfa[0][i], DAC_tfa[1][i], DAC_tfa[2][i]);
        }
//        if(ant==2){ //Bottom Fed Omni
//          setDAC(DAC_bfo[0][i], DAC_bfo[1][i], DAC_bfo[2][i]);
//        }
//        if(ant==3||ant==4){ //Top Fed Inflatable
//          setDAC(DAC_tfi[0][i], DAC_tfi[1][i], DAC_tfi[2][i]);
//        }

        usleep(10000); //lighthouse speed (3.6 sec period)
      }
      system("/home/ubuntu/ONR/txServer/swapAnt4.sh");
      ant=2;
      for(i=0; i<max; i++){
        if(ant==3||ant==4){ //Top Fed Inflatable
          setDAC(DAC_tfi[0][i], DAC_tfi[1][i], DAC_tfi[2][i]);
	}
        usleep(10000); //lighthouse speed (3.6 sec period)

      }
    }
  }
}
