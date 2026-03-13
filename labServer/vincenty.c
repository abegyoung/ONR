#include <stdio.h>
#include <math.h>
#include "txServer.h"
#include "streamer.h"

//Function
POINTING_t vincenty(int gs){

   extern posBuffer_t posBuffer;

   struct POINTING result;

   /* udpMonitor thread is filling the GONDOLA position buffer */
   float phi1 = posBuffer.latitude  * (2*PI/360.);
   float   L1 = posBuffer.longitude * (2*PI/360.);
   float   h1 = posBuffer.altitude;

   float phi2;
   float   L2;
   float   h2;

   if (gs == 1)
     { 
       /* Ground Station 1 is at FSU airport */
       //phi2 =   34.490303 * (2*PI/360.);	/* Latitude */
       //L2   = -104.221943 * (2*PI/360.);	/* Longitude */
       h2   = 1229.;				/* Altitude */
       //for testing
       phi2 =   32.5      * (2*PI/360.);	/* Latitude */
       L2   = -110.5      * (2*PI/360.);	/* Longitude */
    }
   if (gs == 2)
     { 
       /* Ground Station 2 is in the Clovis Walmart Parking Lot */
       //phi2 =   34.437967 * (2*PI/360.);	/* Laitude */
       //L2   = -104.198248 * (2*PI/360.);	/* Longitude */
       h2   = 1301.;				/* Altitude */
       //for testing
       phi2 =   31.8      * (2*PI/360.);	/* Latitude */
       L2   = -110.6      * (2*PI/360.);	/* Longitude */
    }

   float a = 6378137.0;
   float f = 1/298.257223563;
   float b = (1-f)*a;

   float U1 = atan((1-f)*tan(phi1));
   float U2 = atan((1-f)*tan(phi2));
   float L=L2-L1;
   float lam=L;
   float lam0=0.0;

   float sins=0.0;
   float coss=0.0;
   float sig=0.0;
   float sina;
   float cosa=0.0;
   float cos2sm=0.0;
   float C;

   float u;
   float k1;
   float A;
   float B;

   float ds;
   float s;
   float a1;

   float b1;
   //float los;

   while( fabs(lam0-lam) > 0.0000000000001 ){

      lam0 = lam;
      sins = sqrt(pow(cos(U2)*sin(lam),2)+pow(cos(U1)*sin(U2)-sin(U1)*cos(U2)*cos(lam),2));
      coss = sin(U1)*sin(U2)+cos(U1)*cos(U2)*cos(lam);
      sig  = atan2(sins,coss);
      sina = cos(U1)*cos(U2)*sin(lam)/sins;
      cosa = sqrt(1-sina*sina);
      cos2sm = coss - (2*sin(U1)*sin(U2))/(cosa*cosa);
      C = (f/16)*cosa*cosa*(4+f*(4-3*cosa*cosa));
      lam = L + (1-C)*f*sina*(sig + C*sins*(cos2sm + C*coss*(-1+2*cos2sm*cos2sm)));

   }

   u = sqrt(cosa*cosa*(((a*a)-(b*b))/(b*b)));
   k1= (sqrt(1.+u*u)-1.)/(sqrt(1.+u*u)+1.);
   A = (1.+0.25*k1*k1)/(1.-k1);
   B = k1*(1-3./8.*k1*k1);

   ds = B*sins*(cos2sm+B/4*(coss*(-1+2*cos2sm*cos2sm)-B/6*cos2sm*(-3+4*sins*sins)*(-3+4*cos2sm*cos2sm)));
   s  = b*A*(sig-ds);
   a1 = atan2(cos(U2)*sin(lam),(cos(U1)*sin(U2)-sin(U1)*cos(U2)*cos(lam)));
//   if(a1<0.)
//     a1=a1+(2*PI);

   b1 = asin((h2-h1)/s);
   //los = sqrt(pow(a+h2,2)+pow(a+h1,2)-2*(a+h1)*(a+h2)*coss);

   //result.az = fabs(a1*(360/(2*PI)) - posBuffer.X);
   result.az = fmod( a1*(360./(2.*PI)) + (360.-posBuffer.X) , 360.);
   result.el = b1*(360./(2.*PI));

   return result;

}

