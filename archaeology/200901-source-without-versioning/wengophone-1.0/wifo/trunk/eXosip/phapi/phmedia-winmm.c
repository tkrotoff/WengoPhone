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
#include <fcntl.h>
#include <stdlib.h>

#include <ortp.h>
#include <ortp-export.h>

#include "phapi.h"
#include "phcall.h"
#include "phmedia.h"
#include "phcodec.h"
#include "tonegen.h"
#include "phmbuf.h"

#include "phmstream.h"
#include "phastream.h"
#include "phaudiodriver.h"

static void no_printf(const char *x, ...) { }

 
#define ph_printf  printf

#define DBG(x)
#define DBG1(x)
#define DBG2(x,y)
#define DBG3(x,y,z)


#define ADEV(x) ((struct winaudiodev *)(x->drvinfo))

void winmm_stream_start(phastream_t *as);
int  winmm_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
int  winmm_stream_write(phastream_t *as, void *buf,  int len);
int  winmm_stream_read(phastream_t *as, void *buf,  int len);
int  winmm_stream_get_out_space(phastream_t *as, int *used);
int  winmm_stream_get_avail_data(phastream_t *as);
void winmm_stream_close(phastream_t *as);

int winmm_driver_match(struct ph_audio_driver *drv, const char *name)
{
    return strncmp(name, "IN=", 3);
}

#if defined(USE_WAVEIN_CBK)
#define IN_MASK PH_SNDDRVR_REC_CALLBACK
#else
#define IN_MASK 0
#endif

#if defined(USE_WAVEOUT_CBK)
#define OUT_MASK PH_SNDDRVR_PLAY_CALLBACK
#else
#define OUT_MASK 0
#endif



struct ph_audio_driver ph_winmm_driver = {
  "winmm",
  IN_MASK | OUT_MASK,
  0,
  winmm_stream_start,
  winmm_stream_open,
  winmm_stream_write,
  winmm_stream_read,
  winmm_stream_get_out_space,
  winmm_stream_get_avail_data,
  winmm_stream_close,
  0,
  winmm_driver_match    
};


#define MAX_IN_BUFFERS 16
#define USED_IN_BUFFERS 16
#define MAX_OUT_BUFFERS 16
#define USED_OUT_BUFFERS 16
#define AUDIO_INBUF_SIZE  4096
#define AUDIO_OUTBUF_SIZE 4096



struct winaudiodev
{
  WAVEHDR			waveHdrOut[MAX_OUT_BUFFERS];
  HWAVEOUT			hWaveOut;
  char				dataBufferOut[MAX_OUT_BUFFERS][AUDIO_OUTBUF_SIZE];
  HWAVEIN			hWaveIn;
  WAVEHDR			waveHdrIn[MAX_IN_BUFFERS];
  char				dataBufferIn[MAX_IN_BUFFERS][AUDIO_INBUF_SIZE];

  int inPos;
  int outPos;

  HANDLE event;
  int clock_rate;
  DWORD last_read_time;
  int   inBufferSize, inTimeouts, inBufsReady;
  int   outBufferSize;
  ph_audio_cbk cbk;
  int write_started; /** 0/1 depending whether a first waveOutWrite has been made on hWaveOut */
  int read_closing;
  
};



void ph_winmm_driver_init()
{
  ph_register_audio_driver(&ph_winmm_driver);
}



static void CALLBACK 
winmm_audio_read_cbk(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  MMTIME mmtime_in;
  MMTIME mmtime_out;
  WAVEHDR	*wHdr;
  MMRESULT	mr = NOERROR;
  phastream_t *stream = (phastream_t  *) dwInstance;
  int frameSize = stream->ms.codec->decoded_framesize;
  int recLen;
  char *rawData;
  struct winaudiodev *dev = ADEV(stream);
  
  switch(uMsg) {
  case WIM_OPEN:
    break;
  case WIM_CLOSE:
    break;
  case WIM_DATA:
    wHdr = (WAVEHDR *)dwParam1;
    
    if (!stream->ms.running)
      break;
    
    recLen = wHdr->dwBytesRecorded;
    rawData = wHdr->lpData;
    
    while(recLen > 0)
      {

	if (!stream->ms.running)
	  break;
	
	
#ifdef DO_ECHO_CAN
	if (!dev->read_closing) {
		mmtime_in.wType = TIME_SAMPLES;
		mmtime_out.wType = TIME_SAMPLES;
		waveInGetPosition(dev->hWaveIn, &mmtime_in, sizeof(MMTIME) );
		waveOutGetPosition(dev->hWaveOut, &mmtime_out, sizeof(MMTIME) );
		stream->mic_current_sample = mmtime_in.u.sample - (recLen-frameSize);
		stream->spk_current_sample = mmtime_out.u.sample;		
		DBG3("winmm_audio_read_cbk :: in:%d, out:%d, diff:%d\n", mmtime_in.u.sample, mmtime_out.u.sample, mmtime_in.u.sample-mmtime_out.u.sample);
	}
#endif
	dev->cbk(stream, rawData, frameSize, NULL, NULL);

	rawData += frameSize;
	recLen -= frameSize;
      }

	if (!dev->read_closing)
		mr = waveInAddBuffer(dev->hWaveIn, wHdr, sizeof(*wHdr));
    break;
  }

}

#if 0
static void CALLBACK 
os_audio_write_cbk(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  WAVEHDR    *wHdr;
  MMRESULT	mr = NOERROR;
  phastream_t *stream = (phastream_t  *) dwInstance;
  int framesize = stream->codec->decoded_framesize;
  char *rawData;
  int rawBufSize = stream->fd.outBufferSize;
  int rawLen = 0;
  int t_inc;

  switch(uMsg) {
  case WOM_OPEN:
    break;
  case WOM_CLOSE:
    break;
  case WOM_DONE:
    wHdr = (WAVEHDR *)dwParam1;
    rawData = wHdr->lpData;
    
    while(stream->running && rawBufSize > 0)
      {
	t_inc = 0;
	while (stream->running && (rawBufSize > 0) && (mp=rtp_session_recvm_with_ts(stream->rtp_session,stream->rx_timestamp))!=NULL) 
	  {
	    int 	len=mp->b_cont->b_wptr-mp->b_cont->b_rptr;
	    len = stream->codec->decode(stream->decoder_ctx, mp->b_cont->b_rptr, len, rawData, framesize);
	    freemsg(mp);
	    if (!len)
	      {
		rawBufSize = 0;
		break;
	      }
	    
	    rawLen += len;
	    rawData += len;
	    t_inc = len/2;
	    rawBufSize -= len;
	  }
	stream->rx_timestamp += t_inc;
	if (!mp)
	  rawBufSize = 0;
      }
    
    if (rawLen)
      {
	wHdr->dwBufferLength = rawLen;
      }
    else
      {
	memset(wHdr->lpData, 0, framesize);
	wHdr->dwBufferLength = framesize;			
      }
    mr = waveOutWrite(stream->fd.hWaveOut, wHdr, sizeof(*wHdr));

  }
}

#endif


static void 
winmm_init_deviceID(const char *deviceID, unsigned int *waveinDeviceID, unsigned int *waveoutDeviceID)
{
  const char	*inputDevice;
  const char *outputDevice;
  
  if (!deviceID || !*deviceID) 
    {
      *waveinDeviceID = *waveoutDeviceID = WAVE_MAPPER;
      ph_printf("Bad device identifiers (%s)\n", deviceID ? deviceID : "(null)");
      return;
    }
  
  inputDevice = strstr(deviceID, "IN=");
  outputDevice=strstr(deviceID, "OUT=");
  if (!inputDevice || !outputDevice)
    {
      fprintf(stderr, "Bad device id in=%s out=%s\n", 
	      inputDevice ? inputDevice : "(null)", 
	      outputDevice ? outputDevice : "(null)");
      *waveinDeviceID = *waveoutDeviceID = WAVE_MAPPER;
      return;
    }

  *waveinDeviceID = atoi(inputDevice + 3);
  *waveoutDeviceID = atoi(outputDevice + 4);
  if (*waveinDeviceID >= waveInGetNumDevs())
    *waveinDeviceID = WAVE_MAPPER;
  if (*waveoutDeviceID >= waveOutGetNumDevs())
    *waveoutDeviceID = WAVE_MAPPER;
}



int 
winmm_audio_dev_open(phastream_t *as, const char * deviceID, int framesize, int rate, ph_audio_cbk cbk)
{
  MMRESULT mr = NOERROR;
  WAVEFORMATEX		wfx;
  HWAVEIN hWaveIn;
  HWAVEOUT hWaveOut;
  unsigned int		waveoutDeviceID = WAVE_MAPPER;
  unsigned int		waveinDeviceID = WAVE_MAPPER;
  int i;
  struct winaudiodev *dev;


  dev = calloc(sizeof(*dev), 1);

  if (!dev)
    return -PH_NORESOURCES;

  dev->write_started = 0;
  dev->read_closing = 0;
  
  dev->cbk = cbk;

  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.cbSize = 0;
  wfx.nAvgBytesPerSec = 2*rate;
  wfx.nBlockAlign = 2;
  wfx.nChannels = 1;
  wfx.nSamplesPerSec = rate;
  wfx.wBitsPerSample = 16;



  winmm_init_deviceID(deviceID,  &waveoutDeviceID, &waveinDeviceID);
#ifndef USE_WAVEOUT_CBK
  mr = waveOutOpen(&hWaveOut, waveoutDeviceID, &wfx, (DWORD)0/* SpeakerCallback */, 0/* arg */, CALLBACK_NULL /* CALLBACK_FUNCTION */);
#else
  mr = waveOutOpen(&hWaveOut, waveoutDeviceID, &wfx, (DWORD)winmm_audio_write_cbk, (DWORD) as/* arg */, CALLBACK_FUNCTION);
#endif
  dev->hWaveOut = hWaveOut;

  if (mr != NOERROR)
    {
      ph_printf("Error opening output audio dev %x\n", mr);
      return -1;
    }

  dev->outBufferSize = framesize*2;

  for (i=0; i<USED_OUT_BUFFERS; i++)
    {
      WAVEHDR *whp = &dev->waveHdrOut[i];
      
      whp->lpData = dev->dataBufferOut[i];
      whp->dwBufferLength = AUDIO_OUTBUF_SIZE;  /* frameSize */
      whp->dwFlags = 0;
      whp->dwUser = i;
      
      mr = waveOutPrepareHeader(hWaveOut, whp, sizeof(*whp));
      if (mr != MMSYSERR_NOERROR)
	{
	  ph_printf("waveOutPrepareHeader: %x\n", mr);
	  exit(-1);
	}
      
      whp->dwFlags |= WHDR_DONE;
    }
  
  dev->event = CreateEvent(NULL,FALSE,FALSE,NULL);
#ifndef USE_WAVEIN_CBK
  mr = waveInOpen(&hWaveIn, waveinDeviceID, &wfx, (DWORD) dev->event/*WaveInCallback*/, (DWORD) dev, CALLBACK_EVENT/*CALLBACK_FUNCTION*/);
#else
  mr = waveInOpen(&hWaveIn, waveinDeviceID, &wfx, (DWORD) winmm_audio_read_cbk, (DWORD) as, CALLBACK_FUNCTION);
#endif
  
  dev->hWaveIn = hWaveIn;
  
  if (mr != MMSYSERR_NOERROR)
    {
      ph_printf("Error opening input audio dev %x\n", mr);
      waveInClose(hWaveIn);
      return -1;
    }
  
  dev->inBufferSize = framesize * 2;
  for (i=0; i<USED_IN_BUFFERS; i++) 
    {
      WAVEHDR *whp = &dev->waveHdrIn[i];
      
      whp->lpData = dev->dataBufferIn[i];
      whp->dwBufferLength = dev->inBufferSize;  /* frameSize */
      whp->dwFlags = 0;
      whp->dwUser = 0;
      mr = waveInPrepareHeader(hWaveIn, whp, sizeof(*whp));
      if (mr != MMSYSERR_NOERROR)
	{
	  ph_printf("waveiNPrepareHeader: %x\n", mr);
	  exit(-1);
	}
      mr = waveInAddBuffer(hWaveIn, whp, sizeof (WAVEHDR));
    }


  as->drvinfo = dev;
  return 0;
}



int winmm_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk)
{
  int ret;

  ph_printf("winmm_stream_open\n");
  if (!strnicmp(name, "winmm:", 6))
      name += 6;

  ret = winmm_audio_dev_open(as, name, framesize, rate, cbk);
  if (!ret)
    return ret;


  PH_SNDDRVR_USE();


  return 0;
}



  /* start reading from the device */
void winmm_stream_start(phastream_t *as)
{
  struct winaudiodev *dev = ADEV(as);


  dev->last_read_time = timeGetTime();
  waveInStart(dev->hWaveIn);

}


int winmm_stream_write(phastream_t *as, void *buf,  int len)
{
  MMTIME mmtime_in;
  MMTIME mmtime_out;
  struct winaudiodev *dev = ADEV(as);
  WAVEHDR  *whp = &dev->waveHdrOut[dev->outPos];
  int n = USED_IN_BUFFERS;
  WAVEHDR  *whpin = &dev->waveHdrIn[0];
  DWORD mr;
  int cnt = 8;
  
  //winmm_stream_out_status(as);
    
  while (cnt-- && as->ms.running && !(whp->dwFlags & WHDR_DONE)) {
	  DBG("stream write overrrun\n");
      Sleep(5);
  }
  
  if (!as->ms.running || !(whp->dwFlags & WHDR_DONE))
    return 0;

  memcpy(whp->lpData, buf, len);
  whp->dwBufferLength = len;

    // first time into this function :
	//   the waveOutWrite will start off the out.timer
    //   we therefore reser the in.timer to have a better synch
    if (!dev->write_started) {

		dev->write_started = 1;
		dev->read_closing = 1;
	    waveInReset(dev->hWaveIn);
		while (n--) {
			waveInAddBuffer(dev->hWaveIn, whpin, sizeof(*whpin));
			whpin++;
		}
		dev->read_closing = 0;
		waveInStart(dev->hWaveIn);

	}

  mr = waveOutWrite(dev->hWaveOut, whp, sizeof(*whp));
  

  if (++dev->outPos==USED_OUT_BUFFERS) 
    dev->outPos=0; /* loop over the prepared blocks */

  switch (mr)
    {
      
    case MMSYSERR_NOERROR:	
      break;
      
    case MMSYSERR_INVALHANDLE :	
      DBG2("waveOutWrite: 0x%x MMSYSERR_INVALHANDLE\n", mr);
      break;
      
    case MMSYSERR_NODRIVER :
      DBG2("waveOutWrite: 0x%x MMSYSERR_NODRIVER\n", mr);
      break;
      
    case MMSYSERR_NOMEM :
      DBG2("waveOutWrite: 0x%x MMSYSERR_NOMEM\n", mr);
      break;
      
    case WAVERR_UNPREPARED :
      DBG2("waveOutWrite: 0x%x WAVERR_UNPREPARED\n", mr);
      break;
      
    case WAVERR_STILLPLAYING :
      DBG2("waveOutWrite: 0x%x WAVERR_STILLPLAYING\n", mr);
      break;
      
    default :
      DBG2("waveOutWrite: 0x%x\n", mr);
      
    }

	// note that the start IN timestamp here is not 0
	mmtime_in.wType = TIME_SAMPLES;
	mmtime_out.wType = TIME_SAMPLES;
	waveInGetPosition(dev->hWaveIn, &mmtime_in, sizeof(MMTIME) );
	waveOutGetPosition(dev->hWaveOut, &mmtime_out, sizeof(MMTIME) );
	DBG3("winmm_stream_write :: in:%d, out:%d, diff:%d\n", mmtime_in.u.sample, mmtime_out.u.sample, mmtime_in.u.sample-mmtime_out.u.sample);

  return len;

}


int winmm_stream_read(phastream_t *as, void *buf,  int len)
{
  MMTIME mmtime;
  struct winaudiodev *dev = ADEV(as);

  WAVEHDR  *whp = &dev->waveHdrIn[dev->inPos];
  DWORD mr;
  int nloops = 0;
  DWORD waitResult = -1;
  DWORD timeout = 1;

  mmtime.wType = TIME_SAMPLES;
  waveInGetPosition(dev->hWaveIn, &mmtime, sizeof(MMTIME) );
  DBG1("winmm_stream_read :: position %d\n", mmtime.u.sample);


  if (!(whp->dwFlags & WHDR_DONE))
    {
      waitResult = timeout ? WaitForSingleObject(dev->event, timeout) : WAIT_TIMEOUT;
      if (waitResult == WAIT_TIMEOUT)
	{
	  dev->inTimeouts++;
	  if (!(whp->dwFlags & WHDR_DONE))
	    return 0;
	}

      if (!(whp->dwFlags & WHDR_DONE))
	{
	  DBG2("BANG BANG inPos = %d\n", dev->inPos);
	  return 0;
	}
    }

  if (!whp->dwUser)
    dev->inBufsReady++;

  if (!as->ms.running)
    return 0;


  if (len > whp->dwBytesRecorded)
    len = whp->dwBytesRecorded;
  
  if (!len)
  {
	DBG("os_audio_read len==0\n");
  }
  
  if (len)
       memcpy(buf, whp->lpData + whp->dwUser , len);
  

 
  whp->dwBytesRecorded = -len;
  whp->dwBufferLength = dev->inBufferSize;
  whp->dwUser += len;

  if (!whp->dwBytesRecorded)
    {
      whp->dwUser = 0;
      mr = waveInAddBuffer(dev->hWaveIn, whp, sizeof(*whp));
      if (++dev->inPos==USED_IN_BUFFERS) 
	dev->inPos=0; /* loop over the prepared blocks */
    }
  else
    mr = MMSYSERR_NOERROR;
	
  switch (mr)
    {
    case MMSYSERR_NOERROR :
      break;
      
    case MMSYSERR_INVALHANDLE : 
      DBG("waveInAddBuffer : Specified device handle is invalid.\n");
      break;
      
    case MMSYSERR_NODRIVER : 
      DBG("waveInAdBuffer : No device driver is present.\n");
      break;
      
    case MMSYSERR_NOMEM : 
      DBG("waveInAddBuffer : Unable to allocate or lock memory.\n");
      break;
      
    case WAVERR_UNPREPARED : 
      DBG("waveInAddBuffer : The buffer pointed to by the pwh parameter hasn't been prepared.\n");
      break;
      
    case WAVERR_STILLPLAYING :
      DBG("waveInAddBuffer : still something playing.\n");
      break;
      
    default :
      DBG2("waveInAddBuffer error = 0x%x\n", mr);
    }
  
  return len;



}

void winmm_stream_close(phastream_t *as)
{
  struct winaudiodev *dev = ADEV(as);
  int i;
  DWORD mr;

  if (dev->hWaveIn)
    {
      waveInReset(dev->hWaveIn);
      
      for (i=0; i<USED_IN_BUFFERS; i++)
	{
	  WAVEHDR *whp = &dev->waveHdrIn[i];
	  mr = waveInUnprepareHeader(dev->hWaveIn, whp, sizeof(*whp));
	}


      waveInClose(dev->hWaveIn);
      dev->hWaveIn = 0;
    }

  if (dev->hWaveOut)
    {
      waveOutReset(dev->hWaveOut);

      for (i=0; i<USED_OUT_BUFFERS; i++)
	{
	  WAVEHDR *whp = &dev->waveHdrOut[i]; 
	  mr = waveOutUnprepareHeader(dev->hWaveOut, whp, sizeof(*whp));
	}

      waveOutClose(dev->hWaveOut);
      dev->hWaveOut = 0;
	  dev->write_started = 0;
    }

  CloseHandle(dev->event);

  PH_SNDDRVR_UNUSE();

}

int winmm_stream_out_status(phastream_t *as)
{
  struct winaudiodev *dev = ADEV(as);
  int n = USED_OUT_BUFFERS;
  WAVEHDR  *whp = &dev->waveHdrOut[0];
  int countWHDR_BEGINLOOP = 0;
  int countWHDR_DONE = 0;
  int countWHDR_ENDLOOP = 0;
  int countWHDR_INQUEUE = 0;
  int countWHDR_PREPARED = 0;
  int length = 0;
  
  while(n--)
    {
      if (whp->dwFlags & WHDR_BEGINLOOP)
		countWHDR_BEGINLOOP ++;	  
      if (whp->dwFlags & WHDR_DONE)
		countWHDR_DONE ++;	  
      if (whp->dwFlags & WHDR_ENDLOOP)
		countWHDR_ENDLOOP ++;	  
      if (whp->dwFlags & WHDR_INQUEUE) {
			countWHDR_INQUEUE ++;
			length += whp->dwBufferLength;  
	  }
      if (whp->dwFlags & WHDR_PREPARED)
		countWHDR_PREPARED ++;	  
      whp++;
    }
  
	printf("out status: done:%d, %d, %d, %d(%d), %d\n", countWHDR_BEGINLOOP, countWHDR_DONE, countWHDR_ENDLOOP, countWHDR_INQUEUE, length, countWHDR_PREPARED);
 
}

int winmm_stream_get_out_space(phastream_t *as, int *used)
{
  struct winaudiodev *dev = ADEV(as);
  int n = USED_OUT_BUFFERS;
  WAVEHDR  *whp = &dev->waveHdrOut[0];
  int amount = 0;
  int availSpace = 0;
  int usedSpace = 0;

  while(n--)
    {
      if (whp->dwFlags & WHDR_DONE)
	availSpace += whp->dwBytesRecorded;
      else
	usedSpace += whp->dwBufferLength;
	  
      whp++;

    }
  
  *used = usedSpace;
  availSpace = amount;
 
}


int winmm_stream_get_avail_data(phastream_t *as)
{
  struct winaudiodev *dev = ADEV(as);
  int n = USED_IN_BUFFERS;
  WAVEHDR  *whp = &dev->waveHdrIn[dev->inPos];
  int amount = 0;

  while(n && (whp->dwFlags & WHDR_DONE))
    {
      amount += whp->dwBytesRecorded;
      whp++;
      if (whp > &dev->waveHdrIn[USED_IN_BUFFERS])
	{
	  whp = &dev->waveHdrIn[0];
	}
    }
  
  return amount;
}




