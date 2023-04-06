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


/* Video frames fixed transmitting width */
#define PHMEDIA_VIDEO_FRAME_WIDTH 176
#define PHMEDIA_VIDEO_FRAME_HEIGHT 144

struct ph_media_payload_s
{
  int  number;
  char string[32];
  int  rate;
};

typedef struct ph_media_payload_s  ph_media_payload_t;

int ph_media_supported_payload(ph_media_payload_t *pt, const char *ptstring);

int ph_media_init(int usetunnel);
int ph_media_cleanup(void);


int ph_media_start(phcall_t *ca, int port, int videoport, 
				   void (*dtmfCallback)(phcall_t *ca, int event), 
				   void (*endCallback)(phcall_t *ca, int event),
                   phFrameDisplayCbk frameDisplay,
		   const char * deviceId, unsigned vad, int cng, int jitter, int noaec);


void ph_media_stop(phcall_t *ca);


int ph_media_send_dtmf(phcall_t *ca, int dtmf, int mode);

void ph_media_suspend(phcall_t *ca);
void ph_media_resume(phcall_t *ca);
int ph_media_is_stream_stopped(phcall_t *ca);
int ph_media_send_sound_file(phcall_t *ca, const char *filename);

int ph_media_set_recvol(phcall_t *ca, int level);
int ph_media_set_spkvol(phcall_t *ca, int level);

# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
	        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
	        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \



/* */
#endif
