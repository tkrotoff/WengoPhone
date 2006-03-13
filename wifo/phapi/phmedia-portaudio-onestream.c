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
#ifdef ENABLE_PORTAUDIO
#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#ifndef T_MSVC
#include <sys/ioctl.h>
#include <sys/time.h>
#endif
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

#include <portaudio.h>

static void no_printf(const char *x, ...) { }
 
#define ph_printf  printf

#ifdef T_MSVC
#define strncasecmp strnicmp
#endif

struct pa_dev {
  PaStream *stream;
  ph_audio_cbk cbk;
  int bufsize;
};

#define ADEV(x) ((struct pa_dev *)((x)->drvinfo))


#define PADEV(x) (ADEV(x)->stream)

void pa_stream_start(phastream_t *as);
int  pa_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);


int  pa_stream_get_out_space(phastream_t *as, int *used);
int  pa_stream_get_avail_data(phastream_t *as);
void pa_stream_close(phastream_t *as);


struct ph_audio_driver ph_pa_driver = {
  "pa",
  PH_SNDDRVR_REC_CALLBACK|PH_SNDDRVR_PLAY_CALLBACK,
  0,
  pa_stream_start,
  pa_stream_open,
  NULL,
  NULL,
  pa_stream_get_out_space,
  pa_stream_get_avail_data,
  pa_stream_close
  
};


static int 
ph_pa_callback(const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
  phastream_t *as = (phastream_t *) userData;
  int outCount = (int) frameCount * 2;
  int needMore;

  ADEV(as)->cbk(as, (void *) input, (int) frameCount*2, output, &outCount);

  needMore = frameCount*2 - outCount;
  if (needMore)
    memset(outCount + (char *)output, 0, needMore);

  return as->ms.running ? paContinue : paAbort;

}
  


void ph_pa_driver_init()
{
  int err = Pa_Initialize();
  if ( err == paNoError )
    ph_register_audio_driver(&ph_pa_driver);
}


PaStream *pa_dev_open(phastream_t *as, char *name, int rate, int framesize, int latencymsecs)
{
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;
  char *in, *out;
  static double standardSampleRates[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 /* negative terminated  list */
  };
  int i;
  int rateIndex;
  double drate = (double) rate;

  ph_printf("pa_dev_open\n");  
  if (!strncasecmp(name, "pa:", 3))
    name += 3;

  if (in = strstr(name,"IN="))
    {
      inputParameters.device = atoi(in + 3);
    }
  else
    {
      inputParameters.device = Pa_GetDefaultInputDevice();
    }

  if (out = strstr(name,"OUT="))
    {
      outputParameters.device = atoi(out + 4);
    }
  else
    {
      outputParameters.device = Pa_GetDefaultOutputDevice();
    }

  printf("PA Input %d, PA Output %d\n", inputParameters.device,
		outputParameters.device);
   
  inputParameters.channelCount = 1;    
  inputParameters.sampleFormat = paInt16;
  inputParameters.suggestedLatency = latencymsecs / 1000.0;
  inputParameters.hostApiSpecificStreamInfo = 0;
  

  outputParameters.channelCount = 1;   
  outputParameters.sampleFormat = paInt16;
  outputParameters.suggestedLatency = latencymsecs / 1000.0;
  outputParameters.hostApiSpecificStreamInfo = 0;


  /* find the nearest matching entry in the table */
  rateIndex = -1;
  for (i = 0; standardSampleRates[i] > 0; i++ )
    {
      if (drate <= standardSampleRates[i])
	{
	  rateIndex = i;
	  break;
	}
    }


  if (rateIndex == -1)
    {
      return 0;
    }

  /* check if the initial match is accepted */
  err = Pa_IsFormatSupported( &inputParameters, &outputParameters, standardSampleRates[rateIndex] );
  if ( err == paFormatIsSupported )
    {
      as->actual_rate = (int) standardSampleRates[rateIndex];
    }
  else 
    {
      /* find a smapling rate that IS accepted */
      i = rateIndex + 1;
      rateIndex =  -1;
      for (i = 0; standardSampleRates[i] > 0; i++ )
	{
	  err = Pa_IsFormatSupported( &inputParameters, &outputParameters, standardSampleRates[i] );
	  if ( err == paFormatIsSupported )
	    {
	      rateIndex = i;
	      break;
	    }
	}

      if (rateIndex == -1)
	return 0;
    }

  as->actual_rate = (int) standardSampleRates[rateIndex];


  /* we need to recalculate the frame size? */
  if (rate !=  as->actual_rate)
    {
      int frameDuration =  1000 * (framesize / 2) / rate;
      
      framesize = frameDuration * as->actual_rate/1000 * 2;
    }


   err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              standardSampleRates[rateIndex], 
              framesize/2, 
              0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
              ph_pa_callback,
              as );
    if( err != paNoError ) goto error;


  return stream;

 error:

  return 0;
}



int pa_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk)
{

  struct pa_dev *pd;

  pd = calloc(sizeof(*pd), 1);
  if (!pd)
    return -PH_NORESOURCES;

  
  ph_printf("pa_stream_open\n");

  pd->bufsize = (rate * 80 * 2) / 1000;
  pd->stream  = pa_dev_open(as, name, rate, framesize, 80);

  if (!pd->stream)
    {
      free(pd);
      return -PH_NORESOURCES;
    }


  pd->cbk = cbk;
  as->drvinfo = pd;

  PH_SNDDRVR_USE();

  return 0;


}

void pa_stream_close(phastream_t *as)
{
  ph_printf("pa_stream_close\n");

  if (!as->drvinfo)
    {
      ph_printf("pa stream already closed\n");
      return;
    }

  Pa_AbortStream(PADEV(as));
  Pa_CloseStream(PADEV(as));

  free(as->drvinfo);
  as->drvinfo = 0;

  PH_SNDDRVR_UNUSE();


}


void pa_stream_start(phastream_t *as)
{
  
  Pa_StartStream(PADEV(as));

}




int pa_stream_get_out_space(phastream_t *as, int *used)
{
  int free = (int) Pa_GetStreamWriteAvailable(PADEV(as));

  *used = ADEV(as)->bufsize - free*2;

  return free*2;
 
}

int pa_stream_get_avail_data(phastream_t *as)
{
  return (int) Pa_GetStreamReadAvailable(PADEV(as));
}

#endif

