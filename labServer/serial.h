/* $LastChangedDate: 2015-08-10 08:53:54 -0700 (Mon, 10 Aug 2015) $ */
/* $Rev: 58 $      $Author: peters $ */

#define SERIAL_ERRORSTR_SIZE 128
#define SERIAL_COMMSTR_SIZE  512

#define SERIAL_ERROR(str) {               \
              sprintf (serial_errorstr, str); \
              return  _ERROR;                \
            }

#define _ERROR    -1
#define _NO_ERROR  0


    // -----------------  INCLUDES  ----------------- //
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

unsigned int serial_fd[2];
char *serial_dev[2] = {"/dev/ttyTHS0", "/dev/ttyS0"};
fd_set readfs;
int maxfd;
char serial_errorstr [SERIAL_ERRORSTR_SIZE];

struct timeval Timeout;
