/* $LastChangedDate: 2015-08-10 16:39:14 -0700 (Mon, 10 Aug 2015) $ */
/* $Rev: 59 $      $Author: peters $ */

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define SERV_LOGFILE "./Server.log"

void strtime (char *time) {
    struct timeval tv;
    struct tm*     ptm;
    time_t uEpoch;

    gettimeofday (&tv, NULL);
    uEpoch = tv.tv_sec;		/* Avoid compiler warning message */
    ptm = localtime (&uEpoch);

    strftime (time, MAXLINE, "[%Y-%m-%d %H:%M:%S] :: ", ptm);
}

int appendlog (char *str) {

    char logtime [MAXLINE];
    FILE *fd;

    strtime (logtime);

    if ((fd = fopen (SERV_LOGFILE, "a")) == NULL
    ||  write (fileno (fd), logtime, strlen (logtime)) < 0
    ||  write (fileno (fd), str    , strlen (str    )) < 0
    ||  fclose (fd) == EOF)
        return _ERROR;
    return _NO_ERROR;
}
