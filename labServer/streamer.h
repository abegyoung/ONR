/* $LastChangedDate: 2016-08-26 00:43:49 +0000 (Fri, 26 Aug 2016) $ */
/* $Rev: 166 $      $Author: obs $ */

#ifndef STREAMER_H
#define STREAMER_H

#define STREAM_PORT 59000

/* 2015 definition of position_t: the UDP stream packet */

typedef struct
{
  float X;
  float Y;
  float Z;
  float longitude; /* degrees */
  float latitude;  /* degrees */
  float altitude;  /* meters */
} posBuffer_t;

posBuffer_t posBuffer;

#endif

