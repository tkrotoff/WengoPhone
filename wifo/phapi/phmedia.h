/*
 * phmedia -  Phone Api media streamer
 *
 * Copyright (C) 2004 Vadim Lebedev <vadim@mbdsys.com>
 * Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
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
#ifndef __PHMEDIA_H__
#define __PHMEDIA_H__




#define PH_MEDIA_DTMF_PAYLOAD 101
#define PH_MEDIA_SPEEXNB_PAYLOAD 105
#define PH_MEDIA_G726WB_PAYLOAD 102
#define PH_MEDIA_SPEEXWB_PAYLOAD 103
#define PH_MEDIA_ILBC_PAYLOAD 111
#define PH_MEDIA_AMR_PAYLOAD 123
#define PH_MEDIA_AMR_WB_PAYLOAD 124
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_G729_PAYLOAD 18
#define PH_MEDIA_H263_PAYLOAD 34
#define PH_MEDIA_MPEG4_PAYLOAD 98
#define PH_MEDIA_H264_PAYLOAD 40
#define PH_MEDIA_CN_16000_PAYLOAD 125
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_CN_PAYLOAD 13
#define PH_MEDIA_G729_PAYLOAD 18

#define PH_MEDIA_CN_PT_STR "13"



struct ph_media_payload_s
{
  int  number;       /* payload code for rtp packets*/
  char string[32];   /* mime string (ex: ILBC/8000/1) */ 
  int  rate;         /* sampling rate */
  int  chans;        /* number of channels */
  int  ptime;        /* media duration in one packet in msecs */
  int  psize;        /* (optional) size of payload in one packet */
  int  mode;         /* (optional) codec specific mode parameter */
};



enum ph_media_types {
  PH_MEDIA_TYPE_AUDIO = 1,
  PH_MEDIA_TYPE_VIDEO,
  PH_MEDIA_TYPE_OTHER
};

enum ph_mstream_traffic_type { 
  PH_MSTREAM_TRAFFIC_NONE = 0,
  PH_MSTREAM_TRAFFIC_IN = 1,
  PH_MSTREAM_TRAFFIC_OUT = 2,
  PH_MSTREAM_TRAFFIC_IO = 3


};


enum ph_mstream_flags {
  PH_MSTREAM_FLAG_VAD = 1,
  PH_MSTREAM_FLAG_CNG = 2,
  PH_MSTREAM_FLAG_AEC = 4,
  // SPIKE_HDX
  PH_MSTREAM_FLAG_MICHDX = 8,  /* half duplex mode */
  PH_MSTREAM_FLAG_SPKHDX = 16,  /* half duplex mode */
  PH_MSTREAM_FLAG_RUNNING = 32,
  PH_MSTREAM_FLAG_TUNNEL = 64,
  
};

/* media stream creation parameters */
struct ph_mstream_params_s
{
  int  localport;
  int  medatype;
  int  traffictype;
  int  flags;
  int  jitter;
  char remoteaddr[16];
  int  remoteport;
  int  vadthreshold;
  int  videoconfig; 
  struct ph_media_payload_s ipayloads[16];
  struct ph_media_payload_s opayloads[16];
  
  
  void *streamerData;


};

typedef struct ph_mstream_params_s ph_mstream_params_t;


#define PH_MSESSION_MAX_STREAMS  4
#define PH_MSTREAM_AUDIO1 0
#define PH_MSTREAM_VIDEO1 1
#define PH_MSTREAM_AUDIO2 2
#define PH_MSTREAM_VIDEO2 3

enum ph_mession_conf_flags
  {
    PH_MSESSION_CONF_MEMBER = 1,
    PH_MSESSION_CONF_MASTER = 2
  };


/*  session englobes up to 2 aduio and 2 video streams */
struct ph_msession_s
{
  int    activestreams;   /* bit mask of active streams */
  int    newstreams;      /* bit mask of new streams to be activated */
  int    confflags;       /* when nonzero this session make part of a conference */
  struct ph_msession_s *confsession;
  struct ph_mstream_params_s streams[PH_MSESSION_MAX_STREAMS];

  void (*dtmfCallback)(void *info, int event);
  void (*endCallback)(void  *info, int event);
  void (*frameDisplayCbk)(void *info, void *event);

  void *cbkInfo;


};


typedef struct ph_msession_s ph_msession_t; 




typedef struct ph_media_payload_s  ph_media_payload_t;

int ph_media_init(int usetunnel, const char *pluginpath);
int ph_media_cleanup(void);



int ph_media_supported_payload(ph_media_payload_t *pt, const char *ptstring);
int ph_media_can_handle_payload(const char *mime);



int ph_msession_start(struct ph_msession_s *s, const char *adeviceId);
void ph_msession_stop(struct ph_msession_s *s, const char *adeviceId);
#define ph_msession_stopped(s)  (s->activestreams == 0)
#define ph_msession_stream_active(s, n)  (s->activestreams & (1 << n))
int ph_msession_set_recvol(struct ph_msession_s *s,  int level);
int ph_msession_set_playvol(struct ph_msession_s *s,  int level);
int ph_msession_send_sound_file(struct ph_msession_s *s, const char *filename);
int ph_msession_send_dtmf(struct ph_msession_s *s, int dtmf, int mode);
int ph_msession_conf_start(struct ph_msession_s *s1, struct ph_msession_s *s2, const char *adevice);
int ph_msession_conf_stop(struct ph_msession_s *s1, struct ph_msession_s *s2);
int ph_msession_suspend(struct ph_msession_s *s,  int traffictype, const char *adevice);
int ph_msession_resume(struct ph_msession_s *s, int traffictype, const char *adevice);


#if 0
int ph_media_start(phcall_t *ca, int port, int videoport, 
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
                   phFrameDisplayCbk frameDisplay,
		   const char * deviceId, unsigned vad, int cng, int jitter, int noaec);


void ph_media_stop(phcall_t *ca);


int ph_media_send_dtmf(phcall_t *ca, int dtmf, int mode);

void ph_media_suspend(phcall_t *ca, int local);
void ph_media_resume(phcall_t *ca);
int ph_media_is_stream_stopped(phcall_t *ca);
int ph_media_send_sound_file(phcall_t *ca, const char *filename);

int ph_media_set_recvol(phcall_t *ca, int level);
int ph_media_set_spkvol(phcall_t *ca, int level);
#endif

struct timeval;
void ph_tvdiff(struct timeval *diff,  struct timeval *out,  struct timeval *in);
void ph_tvsub( struct timeval *out,  struct timeval *in);

/* */
#endif
