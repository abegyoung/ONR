# $LastChangedDate: 2015-12-13 17:34:07 -0700 (Sun, 13 Dec 2015) $
# $Rev: 107 $      $Author: obs $


CC=gcc
CFLAGS=-Wall -O -g -Wno-unused-result
LIBFLAGS=
OBJS= spi.o adc.o i2c.o serial.o Commands.o txServer.o

all: txServer

txServer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o txServer $(LIBFLAGS)

clean :
	rm -f *.o *~ txServer
