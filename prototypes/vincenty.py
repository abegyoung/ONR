#!/usr/bin/python2.7

import math
from math import sin, cos, tan, asin, atan, atan2, sqrt

phi1=math.radians(+35.527711)        #starting latitude
L1  =math.radians(-103.928806)       #starting longitude
h1= 30480.
phi2=math.radians(+34.490303)        #ending latitude
L2  =math.radians(-104.221943)       #ending longitude
h2= 1229.

a=6378137.0                     # Earth radius
f=1/298.257223563
b=(1-f)*a

U1= atan((1-f)*tan(phi1))
U2= atan((1-f)*tan(phi2))
L=L2-L1
lam=L
lam0=0

while (abs(lam0-lam)>1e-12):
 lam0=lam
 sins= sqrt((cos(U2)*sin(lam))**2+(cos(U1)*sin(U2)-sin(U1)*cos(U2)*cos(lam))**2)
 coss= sin(U1)*sin(U2)+cos(U1)*cos(U2)*cos(lam)
 sig = atan2(sins,coss)
 sina= cos(U1)*cos(U2)*sin(lam)/sins
 cosa= sqrt(1-sina*sina)
 cos2sm= coss - (2*sin(U1)*sin(U2))/(cosa*cosa)
 C= (f/16)*cosa*cosa*(4+f*(4-3*cosa*cosa))
 lam= L + (1-C)*f*sina*(sig + C*sins*(cos2sm + C*coss*(-1+2*cos2sm*cos2sm)))
 #print lam0-lam

u = sqrt(cosa*cosa*((a**2-b**2)/b**2))
k1= (sqrt(1+u**2)-1)/(sqrt(1+u**2)+1)
A = (1+0.25*k1*k1)/(1-k1)
B = k1*(1-3./8.*k1*k1)

ds = B*sins*(cos2sm+B/4*(coss*(-1+2*cos2sm*cos2sm)-B/6*cos2sm*(-3+4*sins*sins)*(-3+4*cos2sm*cos2sm)))
s  = b*A*(sig-ds)
a1 = atan2(cos(U2)*sin(lam),(cos(U1)*sin(U2)-sin(U1)*cos(U2)*cos(lam)))
if a1<0:
  a1=a1+2*math.pi

b1 = asin((h2-h1)/s)
los = sqrt(((a+h2)**2)+((a+h1)**2)-2*(a+h1)*(a+h2)*coss)

#print 'distance = %.3f km'   % (s/1000.) 
print 'bearing   = %.3f deg'  % math.degrees(a1)
print 'elevation = %.3f deg'  % math.degrees(b1)
#print 'arc leng = %.3f deg'  % math.degrees(sig)
print 'LOS dist  = %.3f km'   % (los/1000.)


