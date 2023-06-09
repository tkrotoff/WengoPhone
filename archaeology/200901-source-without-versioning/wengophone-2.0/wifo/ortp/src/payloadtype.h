/*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc1889) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PAYLOADTYPE_H
#define PAYLOADTYPE_H
#include "rtpport.h"

typedef enum{
	PAYLOAD_TYPE_ALLOCATED = 1
}PayloadTypeFlags;

struct _PayloadType
{
	gint type;
	#define PAYLOAD_AUDIO_CONTINUOUS 0
	#define PAYLOAD_AUDIO_PACKETIZED 1
	#define	PAYLOAD_VIDEO 2
	#define PAYLOAD_OTHER 3  /* ?? */
	gint clock_rate;
	double bytes_per_sample;		/* in case of continuous audio data */
	char *zero_pattern;
	gint pattern_length;
	/* other usefull information */
	gint normal_bitrate;	/*in bit/s */
        char mime_type[32];
	PayloadTypeFlags flags;
	void *user_data;
};

#ifndef PayloadType_defined
#define PayloadType_defined
typedef struct _PayloadType PayloadType;
#endif

PayloadType *payload_type_new();
PayloadType *payload_type_clone(PayloadType *payload);
void payload_type_destroy(PayloadType *pt);
#define payload_type_set_flag(pt,flag) (pt)->flags|=(flag)
#define payload_type_unset_flag(pt,flag) (pt)->flags&=(~flag)

struct _RtpProfile
{
	char *name;
	PayloadType *payload[256];
};


typedef struct _RtpProfile RtpProfile;


extern RtpProfile av_profile;

#define payload_type_set_user_data(pt,p)	(pt)->user_data=(p)
#define payload_type_get_user_data(pt)		((pt)->user_data)



#define rtp_profile_get_name(profile) 	(profile)->name
#define rtp_profile_set_name(profile,nm) 	(profile)->name=(nm)
#define rtp_profile_set_payload(profile,index,pt)  (profile)->payload[(index)]=(pt)
#define rtp_profile_clear_payload(profile,index)	(profile)->payload[(index)]=NULL
#define rtp_profile_clear_all(profile)	memset((void*)(profile),0,sizeof(RtpProfile))
#define rtp_profile_get_payload(profile,index)	((profile)->payload[(index)])
PayloadType * rtp_profile_get_payload_from_mime(RtpProfile *profile,const char *mime);
PayloadType * rtp_profile_get_payload_from_rtpmap(RtpProfile *profile, const char *rtpmap);
gint rtp_profile_get_payload_number_from_mime(RtpProfile *profile,const char *mime);
gint rtp_profile_get_payload_number_from_rtpmap(RtpProfile *profile, const char *rtpmap);
gint rtp_profile_find_payload_number(RtpProfile *prof,const gchar *mime,int rate);
PayloadType * rtp_profile_find_payload(RtpProfile *prof,const gchar *mime,int rate);
gint rtp_profile_move_payload(RtpProfile *prof,int oldpos,int newpos);

RtpProfile * rtp_profile_new(char *name);
/*clone a profile and its payloads */
RtpProfile * rtp_profile_clone_full(RtpProfile *prof);
void rtp_profile_destroy(RtpProfile *prof);

/* some payload types */
/* audio */
extern PayloadType pcmu8000;
extern PayloadType pcma8000;
extern PayloadType pcm8000;
extern PayloadType lpc1016;
extern PayloadType gsm;
extern PayloadType lpc1015;
//extern PayloadType speex_nb;
extern PayloadType speex_wb;
extern PayloadType truespeech;

/* video */
extern PayloadType mpv;
extern PayloadType h261;




#endif
