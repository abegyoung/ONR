# $LastChangedDate: 2015-12-13 17:34:07 -0700 (Sun, 13 Dec 2015) $
# $Rev: 107 $      $Author: obs $


CC=gcc
CFLAGS=-Wall -O -g -Wno-unused-result -Wno-return-type
LIBFLAGS=
OBJS= serial.o imuServer.o

all: imuServer udprec

imuServer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o imuServer $(LIBFLAGS)

udprec: udprec.c
	gcc -o udprec udprec.c

clean :
	rm -f *.o *~ imuServer
