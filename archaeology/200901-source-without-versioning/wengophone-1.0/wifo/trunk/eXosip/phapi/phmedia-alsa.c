/*
  The phmedia-alsa  module implements interface to ALSA audio devices for phapi
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
#ifdef ENABLE_ALSA
#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
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


struct alsa_dev {
  snd_pcm_t *ain, *aout;
};

#define ADEV(x) ((struct alsa_dev *)((x)->drvinfo))

void alsa_stream_start(phastream_t *as);
int  alsa_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
int  alsa_stream_write(phastream_t *as, void *buf,  int len);
int  alsa_stream_read(phastream_t *as, void *buf,  int len);
int  alsa_stream_get_out_space(phastream_t *as, int *used);
int  alsa_stream_get_avail_data(phastream_t *as);
void alsa_stream_close(phastream_t *as);
int  alsa_stream_get_fds(phastream_t *as, int fds[2]);

struct ph_audio_driver ph_alsa_driver = {
  "alsa",
  PH_SNDDRVR_FDS,
  0,
  alsa_stream_start,
  alsa_stream_open,
  alsa_stream_write,
  alsa_stream_read,
  alsa_stream_get_out_space,
  alsa_stream_get_avail_data,
  alsa_stream_close,
  alsa_stream_get_fds
};



void ph_alsa_driver_init()
{
  ph_register_audio_driver(&ph_alsa_driver);
}


snd_pcm_t *alsa_dev_open(char *name, int type, int rate, int framesize, int latencymsecs)
{
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_sw_params_t *sparams;
  unsigned int val, val2;
  int dir;
  snd_pcm_uframes_t frames;
  int rc;
  char *nmend = 0;


  ph_printf("alsa_dev_open\n");  
  if (!strncasecmp(name, "alsa:", 5))
    name += 5;

  if (type == SND_PCM_STREAM_CAPTURE)
    { 
      if (!strncasecmp(name, "in=", 3))
	{
	  nmend = strchr(name+3, ' ');
	  if (nmend)
	    {
	      *nmend = 0;
	    }
	}
    }
  else if (!strncasecmp(name, "out=", 4))
    {
      nmend = strchr(name+4, ' ');
      if (nmend)
	{
	      *nmend = 0;
	}
    }
    
  /* Open PCM device */
  rc = snd_pcm_open(&handle, name, type, 0);

  /* restore the overwritten space */
  if (nmend)
    *nmend = ' ';

  if (rc < 0) {
    ph_printf( 
            "unable to open <%s> pcm device: %s\n",
            name, snd_strerror(rc));
    return 0;
  }
  
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Two channels (stereo) */
  rc = snd_pcm_hw_params_set_channels(handle, params, 1);
  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters(channels=1): %s\n",
            snd_strerror(rc));
    goto err;
  }



  /* Interleaved mode */
  rc = snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

#if 1
  /* Write the parameters to the driver */
  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters(SND_PCM_ACCESS_RW_INTERLEAVED): %s\n",
            snd_strerror(rc));
    goto err;
  }

#endif

  /* Signed 16-bit little-endian format */
  rc = snd_pcm_hw_params_set_format(handle, params, 
                              SND_PCM_FORMAT_S16_LE);


  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters(SND_PCM_FORMAT_S16_LE): %s\n",
            snd_strerror(rc));
    goto err;
  }






 
  /* Set period size */
  frames = framesize;
  rc = snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, 0);



  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters(period_size=%d): %s\n", (int) frames,
            snd_strerror(rc));
    goto err;
  }



  val = rate;
  rc = snd_pcm_hw_params_set_rate_near(handle, 
                                 params, &val, 0);

  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters(rate=%d dir=%d): %s\n", val, dir,
            snd_strerror(rc));
    goto err;
  }





  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    ph_printf( 
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    goto err;
  }

  if (!latencymsecs)
    return handle;

  snd_pcm_sw_params_alloca(&sparams);

  /* retrieve the parameters from the driver */
  rc = snd_pcm_sw_params_current(handle, sparams);
  if (rc < 0) {
    ph_printf( 
            "unable to get sw parameters: %s\n",
            snd_strerror(rc));
    goto err;
  }


  frames = rate/1000 * latencymsecs;
  rc = snd_pcm_sw_params_set_start_threshold(handle, sparams, frames);
  if (rc < 0) {
    ph_printf( 
            "unable to set start threshold: %s\n",
            snd_strerror(rc));
    goto err;
  }


  /* Write the parameters to the driver */
  rc = snd_pcm_sw_params(handle, sparams);
  if (rc < 0) {
    ph_printf( 
            "unable to set sw parameters: %s\n",
            snd_strerror(rc));
    goto err;
  }

  /* Write the parameters to the driver */
  rc = snd_pcm_nonblock(handle, 1);
  if (rc < 0) {
    ph_printf( 
            "unable to set noblocking mode:%s\n",
            snd_strerror(rc));
    goto err;
  }

  

  return handle;


err:
  snd_pcm_close(handle);

  return 0;
}


int alsa_dev_get_fd(snd_pcm_t *s)
{
  int err;
  struct pollfd ufd;

  if ((err = snd_pcm_poll_descriptors(s, &ufd, 1)) < 0) 
   {
     ph_printf( "Unable to obtain poll descriptors for device: %s\n", snd_strerror(err));
     return -1;
   }

  return ufd.fd;
}

int alsa_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk)
{

  struct alsa_dev *ad = 0;
  
  
  ph_printf("alsa_stream_open\n");

  ad = calloc(sizeof(*ad), 1);
  if (!ad)
    return -PH_NORESOURCES;

  ad->aout  = alsa_dev_open(name, SND_PCM_STREAM_PLAYBACK, rate, framesize/2, 80);
  if (!ad->aout)
    {
      free(ad);
      return -PH_NORESOURCES;
    }


  ad->ain  = alsa_dev_open(name, SND_PCM_STREAM_CAPTURE, rate, framesize/2, 0);
  if (!ad->ain)
    {
      snd_pcm_close(ad->aout);
      free(ad);
      return -PH_NORESOURCES;
    }

  as->drvinfo = ad;

  PH_SNDDRVR_USE();

  return 0;


}

void alsa_stream_close(phastream_t *as)
{
  ph_printf("alsa_stream_close\n");

  if (!as->drvinfo)
    {
      ph_printf("alsa stream already closed\n");
      return;
    }


  snd_pcm_drop(ADEV(as)->ain);
  snd_pcm_drop(ADEV(as)->aout);
  snd_pcm_close(ADEV(as)->ain);
  snd_pcm_close(ADEV(as)->aout);

  free(as->drvinfo);
  as->drvinfo = 0;

  PH_SNDDRVR_UNUSE();


}


void alsa_stream_start(phastream_t *as)
{
  snd_pcm_prepare(ADEV(as)->ain);
  snd_pcm_start(ADEV(as)->ain);
}



int alsa_stream_write(phastream_t *as, void *buf,  int len)
{
  int i;


  i = snd_pcm_writei(ADEV(as)->aout, buf, len/2);
  if (i < 0)
    {
      ph_printf("error %s writing to alsa device\n",
            snd_strerror(i));

      snd_pcm_prepare(ADEV(as)->aout);
      return 0;
    }

   return i*2;
  
}

int alsa_stream_read(phastream_t *as, void *buf,  int len)
{
  int i;


  i = snd_pcm_readi(ADEV(as)->ain, buf, len/2);
  if (i < 0)
    {
      ph_printf("error %s reading from alsa device\n",
            snd_strerror(i));
       snd_pcm_prepare(ADEV(as)->ain);
      return 0;
    }
  if (i == 0)
    ph_printf("no mic data\n");


   return i*2;
  
}

int alsa_stream_get_out_space(phastream_t *as, int *used)
{

  snd_pcm_status_t *st;

  snd_pcm_status_alloca(&st);

  
  if (0 < snd_pcm_status(ADEV(as)->aout, st))
    {
      *used = 0;
      return 320;
    }
  
  *used = 2 * (int) snd_pcm_status_get_delay(st);    
  return 2 * (int) snd_pcm_status_get_avail(st);    

 
}

int alsa_stream_get_avail_data(phastream_t *as)
{
  snd_pcm_status_t *st;

  snd_pcm_status_alloca(&st);

  
  if (0 < snd_pcm_status(ADEV(as)->ain, st))
    {
      return 0;
    }
  

  return 2 * (int) snd_pcm_status_get_avail(st);    

 
}

int alsa_stream_get_fds(phastream_t *as, int fds[2])
{
  fds[0] = fds[1] = -1;

  fds[0] = alsa_dev_get_fd(ADEV(as)->ain);
  fds[1] = alsa_dev_get_fd(ADEV(as)->aout);

  return 0;
}





#endif

