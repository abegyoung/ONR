# $LastChangedDate: 2015-12-13 17:34:07 -0700 (Sun, 13 Dec 2015) $
# $Rev: 107 $      $Author: obs $


CC=gcc
CFLAGS=-Wall -O -g -Wno-unused-result -Wno-return-type -pthread
LIBFLAGS= -lm -lbsd
OBJS= pointing.o housekeeping.o udp.o spi.o adc.o i2c.o serial.o vincenty.o Commands.o txServer.o
OBJS1= i2c.o spi.o setvco.o 

all: txServer setvco

txServer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o txServer $(LIBFLAGS)

setvco: $(OBJS1)
	$(CC) $(CFLAGS) $(OBJS1) -o setvco $(LIBFLAGS)

clean :
	rm -f *.o *~ txServer setvco
