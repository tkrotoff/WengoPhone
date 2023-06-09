/*
 * phmedia -  Phone Api media streamer
 *
 * Copyright (C) 2004 Vadim Lebedev <vadim@mbdsys.com>
 * Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <osip2/osip_mt.h>
#include <osip2/osip.h>
#ifndef WIN32
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
#include "phmstream.h"

#ifdef USE_HTTP_TUNNEL
#include <httptunnel.h>
#endif

int ph_media_use_tunnel = 0;

static void no_printf(const char *x, ...) { }
 
#define ph_printf  printf

void
ph_tvdiff(register struct timeval *diff, register struct timeval *out, register struct timeval *in)
{
  diff->tv_sec = out->tv_sec;
  diff->tv_usec = out->tv_usec;
  
  diff->tv_usec -= in->tv_usec;
  
  while(diff->tv_usec < 0) {
    --diff->tv_sec;
    diff->tv_usec += 1000000;
  }
  
  diff->tv_sec -= in->tv_sec;
}


int ph_media_cleanup()
{
  return 0;
}


int ph_media_start(phcall_t *ca, int port, int videoport, 
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
		           phFrameDisplayCbk frameDisplay,
		   const char * deviceId, unsigned vad, int cng, int jitter, int noaec)

{

  int ret;

  ret = ph_media_audio_start(ca, port, dtmfCallback, endCallback, deviceId, vad, cng, jitter, noaec);

#ifdef PHAPI_VIDEO_SUPPORT
  if (!ret && (frameDisplay != 0))
    {
    ph_media_video_start(ca, videoport, frameDisplay);
    }
  else
    ph_media_video_stop(ca);
#endif

  return ret;

}

void ph_media_stop(phcall_t *ca)
{
  int i;

#ifdef PHAPI_VIDEO_SUPPORT
  ph_media_video_stop(ca);
#endif
  ph_media_audio_stop(ca);
}
  

static PayloadType ilbc =
{
	TYPE( PAYLOAD_AUDIO_PACKETIZED),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE( 0),
	ZERO_PATTERN(NULL),
	PATTERN_LENGTH( 0),
	NORMAL_BITRATE( 13330),
	MIME_TYPE ("ILBC")
};

static PayloadType amr={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(8000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(12000),    /* set mode MR 122 as default */
    MIME_TYPE ("AMR")
};
static PayloadType amr_wb={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(16000),
    BYTES_PER_SAMPLE(320),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(24000),   /* set mode 2385 as default */
    MIME_TYPE ("AMR-WB")
};
static PayloadType cng_8={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(8000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(8000),  
    MIME_TYPE ("CN")
};
static PayloadType cng_16={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(16000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(8000),  
    MIME_TYPE ("CN")
};

 

static PayloadType h263={
	PAYLOAD_VIDEO,
	90000,
	0,
	NULL,
	0,
	256000,
	"H263"
};

static PayloadType h264={
	PAYLOAD_VIDEO,
	90000,
	0,
	NULL,
	0,
	256000,
	"H264"
};


static PayloadType mpeg4 ={
	PAYLOAD_VIDEO,
	90000,
	0,
	NULL,
	0,
	256000,
	"MP4V-ES"
};





int 
ph_media_init(int useTunnel)
{
  static int first_time = 1;
  RtpProfile *profile;


  ph_media_use_tunnel = useTunnel;
  
  if (!first_time)
    return 0;

  ph_media_audio_init();
  ph_media_codecs_init();



  ortp_init();

  profile = get_av_profile();


   
  rtp_profile_set_payload(profile,PH_MEDIA_DTMF_PAYLOAD,
			  get_telephone_event());
  rtp_profile_set_payload(profile,PH_MEDIA_ILBC_PAYLOAD, &ilbc);
  rtp_profile_set_payload(profile,PH_MEDIA_AMR_PAYLOAD, &amr);
  rtp_profile_set_payload(profile,PH_MEDIA_AMR_WB_PAYLOAD, &amr_wb);
  rtp_profile_set_payload(profile,PH_MEDIA_CN_PAYLOAD, &cng_8);

#ifdef PHAPI_VIDEO_SUPPORT
  rtp_profile_set_payload(profile,PH_MEDIA_H263_PAYLOAD, &h263);
  rtp_profile_set_payload(profile,PH_MEDIA_H264_PAYLOAD, &h264);
  rtp_profile_set_payload(profile,PH_MEDIA_MPEG4_PAYLOAD, &mpeg4);
#endif

  ortp_scheduler_init();
  ortp_set_debug_file("oRTP", NULL);
  first_time = 0;  
  return 0;
}



int ph_media_supported_payload(ph_media_payload_t *pt, const char *ptstring)
{
  PayloadType *rtppt;
  RtpProfile *profile = get_av_profile();

  pt->number = rtp_profile_get_payload_number_from_rtpmap(profile, ptstring);
  if (pt->number == -1)
    return 0;

  rtppt = rtp_profile_get_payload(profile, pt->number);

  strncpy(pt->string, rtppt->mime_type, sizeof(pt->string));
  pt->rate = rtppt->clock_rate;
 
  return 1;
  
}

int ph_media_is_stream_stopped(phcall_t *ca)
{
  phmstream_t* stream;
	
  if (ca->cid == -1)
    return -1;

  stream = (phmstream_t*) ca->ph_audio_stream;
  if (!stream)
    return -1;

  return stream->is_terminated;
}


phcodec_t *ph_media_lookup_codec(int payload)
{
  RtpProfile *profile = get_av_profile();
  PayloadType *pt = rtp_profile_get_payload(profile, payload);
  phcodec_t *codec = ph_codec_list;
  int plen = strlen(pt->mime_type);
  
while(codec)
    {
      int mlen = strlen(codec->mime);
      if(mlen == plen){
#ifdef WIN32
	if (!strnicmp(codec->mime, pt->mime_type, mlen)) {
#else
	if (!strncasecmp(codec->mime, pt->mime_type, mlen)){
#endif
	  return codec;
	}
      }
      codec = codec->next;
    }

  return 0;


}

void ph_tvsub(register struct timeval *out, register struct timeval *in)
{
	out->tv_usec -= in->tv_usec;

	while(out->tv_usec < 0) {
		--out->tv_sec;
		out->tv_usec += 1000000;
	}

	out->tv_sec -= in->tv_sec;
}


void
tvdiff(register struct timeval *diff, register struct timeval *out, register struct timeval *in)
{
  diff->tv_sec = out->tv_sec;
  diff->tv_usec = out->tv_usec;
  
  diff->tv_usec -= in->tv_usec;
  
  while(diff->tv_usec < 0) {
    --diff->tv_sec;
    diff->tv_usec += 1000000;
  }
  
  diff->tv_sec -= in->tv_sec;
}


int ph_media_get_clock_rate(int payload)
{
  RtpProfile *profile = get_av_profile();
  PayloadType *pt = rtp_profile_get_payload(profile, payload);
  return pt->clock_rate;
}


void tvsub_phapi(register struct timeval *out, register struct timeval *in)
{
	out->tv_usec -= in->tv_usec;

	while(out->tv_usec < 0) {
		--out->tv_sec;
		out->tv_usec += 1000000;
	}

	out->tv_sec -= in->tv_sec;
}





#ifdef USE_HTTP_TUNNEL
RtpTunnel *
rtptun_new()
{
	RtpTunnel *tn;

	tn = (RtpTunnel *) malloc(sizeof(*tn));

	if (!tn)
		return 0;

  	tn->tunnel_get_socket = http_tunnel_get_socket;
	tn->tunnel_close = http_tunnel_close;
	tn->tunnel_recv = http_tunnel_recv;
	tn->tunnel_send = http_tunnel_send;

	return tn;

}

RtpTunnel *
rtptun_connect(const char *address, int port)
{
    RtpTunnel *tn = rtptun_new();
    
    if (!tn)
        return 0;
    
    tn->h_tunnel = http_tunnel_open(address, port, HTTP_TUNNEL_VAR_MODE);
    
    return tn;    
}


void
rtptun_free(RtpTunnel *tn)
{
	free(tn);
}
#endif

