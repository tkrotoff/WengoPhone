/*
  The phmedia-oss  module implements interface to OSS audio devices for phapi
  Copyright (C) 2004  Vadim Lebedev  <vadim@mbdsys.com>
  
  this module is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  eXosip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ortp.h>
#include <ortp-export.h>
#include <telephonyevents.h>
#include "phapi.h"
#include "phcall.h"
#include "phmedia.h"
#include "phcodec.h"
#include "tonegen.h"
#include "phmbuf.h"

#include "phmstream.h"
#include "phastream.h"
#include "phaudiodriver.h"

#define no_printf(a...)
 
#define ph_printf  printf

#define DBG(x...)

#define ADEV(x) ((int)(x->drvinfo))

void oss_stream_start(phastream_t *as);
int  oss_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
int  oss_stream_write(phastream_t *as, void *buf,  int len);
int  oss_stream_read(phastream_t *as, void *buf,  int len);
int  oss_stream_get_out_space(phastream_t *as, int *used);
int  oss_stream_get_avail_data(phastream_t *as);
void oss_stream_close(phastream_t *as);
int oss_stream_get_fds(phastream_t *as, int fds[2]);


struct ph_audio_driver ph_oss_driver = {
  "oss",
  PH_SNDDRVR_FDS,
  0,
  oss_stream_start,
  oss_stream_open,
  oss_stream_write,
  oss_stream_read,
  oss_stream_get_out_space,
  oss_stream_get_avail_data,
  oss_stream_close,
  oss_stream_get_fds
};



void ph_oss_driver_init()
{
  ph_register_audio_driver(&ph_oss_driver);
}




int oss_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk)
{
  int fd, p;
  int blocksize = 512;
  int min_size;
  int cond;
  struct audio_buf_info info;
  int bufsize;

  ph_printf("oss_stream_open\n");
  if (!strncasecmp(name, "oss:", 4))
      name += 4;

  fd=open(name, O_RDWR|O_NONBLOCK);

  if (fd<0)
    { 
      perror("error opening opening AUDIO device");
      exit(1);
    }
      
      
  ioctl(fd, SNDCTL_DSP_RESET, 0);
      
  p =  16;  /* 16 bits */
  ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &p);
      
  p =  1;
  ioctl(fd, SNDCTL_DSP_CHANNELS, &p);
      
  p = AFMT_S16_NE; /* choose LE or BE (endian) */
  ioctl(fd, SNDCTL_DSP_SETFMT, &p);
      
      
  p =  rate;  /* rate in hz*/
  ioctl(fd, SNDCTL_DSP_SPEED, &p);
  
  ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &min_size);
  if (min_size>blocksize)
    {
      cond=1;
      p=min_size/blocksize;
      while(cond)
	{
	  int i=ioctl(fd, SNDCTL_DSP_SUBDIVIDE, &p);
	  if (i != 0)
	    ph_printf( "SUB_DIVIDE %d said error=%i,errno=%i\n",p,i,errno);
	  if ((i==0) || (p==1)) cond=0;
	  else p=p/2;
	}
    }
  ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &min_size);
  if (min_size>blocksize)
    {
      blocksize = min_size;
    }
  else
    {
      /* no need to access the card with less latency than needed*/
      min_size=blocksize;
    }
  
  
  if( 0 > ioctl(fd, SNDCTL_DSP_GETISPACE, &info))
    {
      perror("AUDIO DEV IOCTL error");
      close(fd);
      exit(1);
    }
  else
    {
      DBG("Audio in buffer %d %d\n",info.fragstotal,info.fragsize);
    }
  
  bufsize =  info.fragstotal*info.fragsize;
     

      
      
  if( 0 > ioctl(fd, SNDCTL_DSP_GETOSPACE, &info))
    {
      perror("AUDIO DEV IOCTL error");
      close(fd);
      exit(1);
    }
  else
    {
      DBG("Audio out buffer %d %d\n",info.fragstotal,info.fragsize);
    }
      


  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)&~O_NONBLOCK);


  as->drvinfo = (int)  fd;

  PH_SNDDRVR_USE();


  return 0;
}



  /* start reading from the device */
void oss_stream_start(phastream_t *as)
{

  int enable_bits  = PCM_ENABLE_INPUT|PCM_ENABLE_OUTPUT;
  int i;

  i = ioctl(ADEV(as), SNDCTL_DSP_SETTRIGGER, &enable_bits);
  if (i < 0)
    ph_printf( "Can't start audio intput\n");
  
}


int oss_stream_write(phastream_t *as, void *buf,  int len)
{

  return write(ADEV(as), buf, len);

}


int oss_stream_read(phastream_t *as, void *buf,  int len)
{

  return read(ADEV(as), buf, len);

}

void oss_stream_close(phastream_t *as)
{

  if (ADEV(as) == -1)
    return;

  close(ADEV(as));

  as->drvinfo = (void *)-1;

  PH_SNDDRVR_UNUSE();

}


int oss_stream_get_out_space(phastream_t *as, int *used)
{

  struct audio_buf_info info;


  if( 0 > ioctl(ADEV(as), SNDCTL_DSP_GETOSPACE, &info))
    return -1;

  *used = info.fragsize*info.fragstotal  - info.bytes;
  return info.bytes;
 


}


int oss_stream_get_avail_data(phastream_t *as)
{

  struct audio_buf_info info;


  if( 0 > ioctl(ADEV(as), SNDCTL_DSP_GETISPACE, &info))
    return -1;


  return info.bytes;
}



int oss_stream_get_fds(phastream_t *as, int fds[2])
{
  fds[0] = fds[1] = ADEV(as);
  return 0;
}
