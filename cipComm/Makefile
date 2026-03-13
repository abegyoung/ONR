# $LastChangedDate: 2015-12-13 17:34:07 -0700 (Sun, 13 Dec 2015) $
# $Rev: 107 $      $Author: obs $


CC=gcc
CFLAGS=-Wall -O -g -Wno-unused-result -Wno-return-type -Wno-missing-braces -Wno-unused-variable
LIBFLAGS=
OBJS= gpio.o spi.o i2c.o Commands.o cipComm.o

all: cipComm

cipComm: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cipComm $(LIBFLAGS)

clean :
	rm -f *.o *~ cipComm
