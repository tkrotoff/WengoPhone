/*
 * phapi   phone api
 *
 * Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software F undation; either version 2,
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

/**
 * @file phapi.c
 * @brief softphone  API
 *
 * phapi is a library providing simplified api to create VOIP sessions
 * using eXosip library oSIP stack and oRTP stcak 
 * <P>
 */

#include "phglobal.h"
#ifdef OS_WIN32
#include <winsock2.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#ifndef OS_WIN32
//#include "config.h"
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#ifndef OS_MACOSX
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#else  /* WIN32 */
#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp stricmp
#define usleep(usecs) Sleep((usecs)/1000)
#endif

#include <osip2/osip_mt.h>
#include <eXosip/eXosip.h>
#include <eXosip/eXosip_cfg.h>

#include "phdebug.h"
#include "phapi.h"
#include "phcall.h"
#include "phrpc.h"
#include "phmedia.h"
#include "stun/stun.h"

#ifdef USE_HTTP_TUNNEL
#include "httptunnel.h"
#endif
/*
#define G711_ONLY
*/

#define DBG(x)  printf(x)
#define DBG2(x,y)  printf(x,y)

#define SKIP(x)

#ifdef PHAPI_VIDEO_SUPPORT
#include <avcodec.h>
#endif

phcall_t *ph_locate_call(eXosip_event_t *je, int creatit);
phcall_t *ph_allocate_call(int cid);
phcall_t *ph_locate_call_for_refer(eXosip_event_t *je);

void ph_wegot_dtmf(void  *ctx, int dtmfEvent);
static int ph_event_get();


static char *ph_get_call_contact(phcall_t *ca);

#define MEDIA_SUSPEND

#ifndef PH_STREAM_AUDIO
#define PH_STREAM_AUDIO (1 << 0)
#define PH_STREAM_VIDEO_RX (1 << 1)
#define PH_STREAM_VIDEO_TX (1 << 2)
#endif

#define PH_STREAM_CNG (1 << 30)


static int ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags);
static int ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int flags, int resumeflag);



void ph_release_call(phcall_t *ca);
static  int timeout = 500;

#define PORT_STRING_LEN 16
static char ph_nat_router_addr[32];
static char ph_nat_type_str[32];
static char ph_nat_port_str[PORT_STRING_LEN];
static char ph_nat_video_port_str[16];

static char ph_nat_sip_port_str[16];

static char vcontact[256];

static NatType ph_nat_type = StunTypeOpen;

static char * _get_local_sip_port();
static void  _get_local_video_sdp_port(char buf[]);
static void  _get_local_audio_sdp_port(char buf[]);

static char*  _get_public_sip_port();



void ph_message_progress(eXosip_event_t *je);
static void ph_keep_refreshing();
static void  ph_call_requestfailure(eXosip_event_t *je);

static void ph_frame_display_cbk(void *ctx, void *event);

static int ph_call_media_stop(phcall_t * ca);
static int ph_call_media_suspend(phcall_t *ca, int localhold);

#define nonempty(x)  ((x) && (x)[0])


#define USE_VLINES 1
#define PH_MAX_VLINES 16


struct vline
{
  char  *displayname;
  char  *username;
  char  *server;
  int    port;
  char  *proxy;
  char  *contact;
  time_t   regTimeout;
  time_t   lastRegTime;
  int   rid;
#define VL_USED 1
#define VL_DELETING 2
  int   used;
  int   busy;
  char  *followme;
};


struct vline ph_vlines[PH_MAX_VLINES];

static void
ph_build_cname(char *buf, int n, struct vline *vl);


int phvlRegister(int vlid);
static struct vline *vline_alloc();
static void vline_free(struct vline *vl);
#define PHM_IGNORE_PORT 1
#define PHM_IGNORE_HOST 2
static struct vline *ph_find_matching_vline(const char *userid, int ignore);
static struct vline *ph_find_matching_vline2(const char *username, const char* host, int ignore);
static struct vline *ph_find_matching_vline3(const char *username, const char* host, int port, int ignore);  
static struct vline *ph_find_vline_by_rid(int rid);

static char *ph_get_proxy(const char *from);
static int   ph_get_vline_id(const char *userid, const char *altid);


int getPublicPort(char *local_voice_port, char *local_video_port, char *public_voice_port, char *public_video_port);



#define ph_vlid2vline(vlid) (ph_vlines + (vlid) - 1)
#define ph_vline2vlid(vl) ((vl) - ph_vlines + 1)

#define clear(x) memset(&x, 0, sizeof(x))

static struct vline *
ph_valid_vlid(int vlid)
{
  struct vline *vl = 0;

  if (vlid > 0 && vlid <= PH_MAX_VLINES)
    {
      vl = ph_vlid2vline(vlid);
      if (!vl->used || vl->used == VL_DELETING)
	vl = 0;
    }
  return vl;
}









MY_DLLEXPORT phCallbacks_t *phcb;

MY_DLLEXPORT int phIsInitialized;

MY_DLLEXPORT int phDebugLevel = 0;
MY_DLLEXPORT char *phLogFileName = 0;




static char ph_follow_me_addr[256];

unsigned short phCallBackPort = PH_CALLBACK_PORT; 
MY_DLLEXPORT  unsigned short phServerPort = PH_SERVER_PORT; 

static int ph_busyFlag;


static FILE *ph_log_file;

#define ph_printf printf

void *
ph_api_thread(void *arg);

#define name(x) #x

static char *evtnames[] =
  {
  name(EXOSIP_REGISTRATION_NEW),              /* announce new registration.       */
  name(EXOSIP_REGISTRATION_SUCCESS),          /* user is successfully registred.  */
  name(EXOSIP_REGISTRATION_FAILURE),          /* user is not registred.           */
  name(EXOSIP_REGISTRATION_REFRESHED),        /* registration has been refreshed. */
  name(EXOSIP_REGISTRATION_TERMINATED),       /* UA is not registred any more.    */

  /* for UAC events */
  name(EXOSIP_CALL_NOANSWER),        /* announce no answer within the timeout */
  name(EXOSIP_CALL_PROCEEDING),      /* announce processing by a remote app   */
  name(EXOSIP_CALL_RINGING),         /* announce ringback                     */
  name(EXOSIP_CALL_ANSWERED),        /* announce start of call                */
  name(EXOSIP_CALL_REDIRECTED),      /* announce a redirection                */
  name(EXOSIP_CALL_REQUESTFAILURE),  /* announce a request failure            */
  name(EXOSIP_CALL_SERVERFAILURE),   /* announce a server failure             */
  name(EXOSIP_CALL_GLOBALFAILURE),   /* announce a global failure             */

  /* for UAS events */
  name(EXOSIP_CALL_NEW),             /* announce a new call                   */
  name(EXOSIP_CALL_ACK),             /* ACK received for 200ok to INVITE      */
  name(EXOSIP_CALL_CANCELLED),       /* announce that call has been cancelled */
  name(EXOSIP_CALL_TIMEOUT),         /* announce that call has failed         */
  name(EXOSIP_CALL_HOLD),            /* audio must be stopped                 */
  name(EXOSIP_CALL_OFFHOLD),         /* audio must be restarted               */
  name(EXOSIP_CALL_CLOSED),          /* a BYE was received for this call      */

  /* for both UAS & UAC events */
  name(EXOSIP_CALL_STARTAUDIO),         /* audio must be established           */
  name(EXOSIP_CALL_RELEASED),           /* call context is cleared.            */

  /* for UAC events */
  name( EXOSIP_OPTIONS_NOANSWER),        /* announce no answer within the timeout */
  name(EXOSIP_OPTIONS_PROCEEDING),      /* announce processing by a remote app   */
  name(EXOSIP_OPTIONS_ANSWERED),        /* announce a 200ok                      */
  name(EXOSIP_OPTIONS_REDIRECTED),      /* announce a redirection                */
  name(EXOSIP_OPTIONS_REQUESTFAILURE),  /* announce a request failure            */
  name(EXOSIP_OPTIONS_SERVERFAILURE),   /* announce a server failure             */
  name(EXOSIP_OPTIONS_GLOBALFAILURE),   /* announce a global failure             */

  name(EXOSIP_INFO_NOANSWER),        /* announce no answer within the timeout */
  name(EXOSIP_INFO_PROCEEDING),      /* announce processing by a remote app   */
  name(EXOSIP_INFO_ANSWERED),        /* announce a 200ok                      */
  name(EXOSIP_INFO_REDIRECTED),      /* announce a redirection                */
  name(EXOSIP_INFO_REQUESTFAILURE),  /* announce a request failure            */
  name(EXOSIP_INFO_SERVERFAILURE),   /* announce a server failure             */
  name(EXOSIP_INFO_GLOBALFAILURE),   /* announce a global failure             */

  /* for UAS events */
  name(EXOSIP_OPTIONS_NEW),             /* announce a new options method         */
  name(EXOSIP_INFO_NEW),               /* new info request received           */

  name(EXOSIP_MESSAGE_NEW),            /* announce new incoming MESSAGE. */
  name(EXOSIP_MESSAGE_SUCCESS),        /* announce a 200ok to a previous sent */
  name(EXOSIP_MESSAGE_FAILURE),        /* announce a failure. */


  /* Presence and Instant Messaging */
  name(EXOSIP_SUBSCRIPTION_NEW),          /* announce new incoming SUBSCRIBE.  */
  name(EXOSIP_SUBSCRIPTION_UPDATE),       /* announce incoming SUBSCRIBE.      */
  name(EXOSIP_SUBSCRIPTION_CLOSED),       /* announce end of subscription.     */

  name(EXOSIP_SUBSCRIPTION_NOANSWER),        /* announce no answer              */
  name(EXOSIP_SUBSCRIPTION_PROCEEDING),      /* announce a 1xx                  */
  name(EXOSIP_SUBSCRIPTION_ANSWERED),        /* announce a 200ok                */
  name(EXOSIP_SUBSCRIPTION_REDIRECTED),      /* announce a redirection          */
  name(EXOSIP_SUBSCRIPTION_REQUESTFAILURE),  /* announce a request failure      */
  name(EXOSIP_SUBSCRIPTION_SERVERFAILURE),   /* announce a server failure       */
  name(EXOSIP_SUBSCRIPTION_GLOBALFAILURE),   /* announce a global failure       */
  name(EXOSIP_SUBSCRIPTION_NOTIFY),          /* announce new NOTIFY request     */

  name(EXOSIP_SUBSCRIPTION_RELEASED),        /* call context is cleared.        */

  name(EXOSIP_IN_SUBSCRIPTION_NEW),          /* announce new incoming SUBSCRIBE.*/
  name(EXOSIP_IN_SUBSCRIPTION_RELEASED),     /* announce end of subscription.   */

  name(EXOSIP_CALL_REFERED),              /* announce incoming REFER           */
  name(EXOSIP_CALL_REFER_STATUS),         /* announce incoming NOTIFY          */
  name(EXOSIP_CALL_REFER_FAILURE),         /* announce error during refer */

  name(EXOSIP_CALL_REPLACES)         /* announce incoming INVITE with Rpelaces    */

  };

#undef name



phcall_t ph_calls[PH_MAX_CALLS];


/* 
//#define FORCE_VAD   1
#define FORCE_CNG   1
*/




MY_DLLEXPORT phConfig_t *phGetConfig()
{
  return &phcfg;
}






#ifdef EMBED
MY_DLLEXPORT phConfig_t phcfg = { "10600", "", 
			/* sipport  */ "5060", 
			 /* identity */ "", 
			/*  proxy */    "" ,
			 /* nattype */  "" ,
			 /* codecs */   "" ,
			 0 ,0 ,
		     /* audio_dev */   "" ,
			 /* softboost */ 0,0,0,
			 /* vad */ 0,0,0,
			 0,0,0,
		     /* stunserver */ "80.118.132.74",
			 0
};
#else
MY_DLLEXPORT phConfig_t phcfg = { "10600", "", "10700", "", 
			/* sipport  */ "5060", 
			 /* identity */ "", 
			/*  proxy */    "" ,
			 /* nattype */  "" ,
			 /* codecs */   "" ,"",
			 0 ,0 ,
		     /* audio_dev */   "" ,
			 /* softboost */ 0,0,0,
			 /* vad */ 0,0,0, 0,
			 0,0,0,
		     /* stunserver */  "80.118.132.74",
			 0
};

#endif

static int _is_video_enabled(int streams) 
{
 return (streams & (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX));
}


static int _is_audio_enabled(int streams) 
{
 return (streams & PH_STREAM_AUDIO);
}


static int 
ph_port_inuse(int port)
{
  int i;
  phcall_t *ca;

  /* scan all active calls and check that the given port is inuse */
  for(i=0; i<PH_MAX_CALLS;  i++)
    {
      ca = &ph_calls[i];
      if (ca->cid != -1)
	{
	  /* active call */
	  if(ca->local_sdp_audio_port == port || port == ca->local_sdp_video_port)
	    return 1;
	}
    }
  
  return 0;
}

static void 
_get_local_audio_sdp_port(char buf[])
{

/* <MINHPQ>
 * To get arround the problem of connect in winsock ( connect returns 
 * an error: 10048: Address already in use connect is called an the local address is in used
 * within the last 2 or 4 minutes), we should not bind the rtp socket to a specific local port. Hence, 
 * we should return "0" here to let the system choose a random port number.
 */

  /* base port number*/
  int port = atoi(phcfg.local_rtp_port);
  int i = 0;

  buf[0] = 0;


  while(1)
    {
      if (!ph_port_inuse(port))
      {
	sprintf(buf, "%d", port);
	return;
      }
    else
      port += 2;    /* try next pair */
  }
}

static void 
_get_local_video_sdp_port(char buf[]) 
{
  int port = atoi(phcfg.local_rtp_port) + 2;
  int i;


  buf[0] = 0;

  while(1)
    {
      if (!ph_port_inuse(port))
      {
	sprintf(buf, "%d", port);
	return;
      }
    else
      port += 2;    /* try next pair */
  }
}



static char *
_get_local_sip_port()
{
     return phcfg.sipport;
}

static char *
_get_public_sip_port()
{
  if (ph_nat_sip_port_str[0])
    return ph_nat_sip_port_str;

  return _get_local_sip_port();
  
}



phcall_t *
ph_locate_call_by_cid(int cid)
{
  phcall_t *ca;


  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
      if (ca->cid == cid)
	return ca;
    }

  return 0;
}

#if 0
void phReleaseTerminatedCalls()
{
	phcall_t *ca;
	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
		if ((ca->cid != -1) && (ph_media_is_stream_stopped(ca) == 1))
			ph_release_call(ca);
    }
}
#endif

phcall_t *
ph_locate_call_by_rcid(int cid)
{
  phcall_t *ca;


  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
      if (ca->rcid == cid)
	return ca;
    }

  return 0;
}

phcall_t *
ph_locate_call_by_rdid(int did)
{
  phcall_t *ca;


  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
      if (ca->rdid == did)
	return ca;
    }

  return 0;
}


phcall_t *
ph_allocate_call(int cid)
{
  phcall_t *ca = ph_locate_call_by_cid(-1);

  if (!ca)
    return 0;

  ca->redirs = 0;
  ca->cid = cid;
  return ca;
}


phcall_t *
ph_locate_call(eXosip_event_t *je, int creatit)
{
  phcall_t *ca, *found = 0, *newca = 0;


  /* lookup matching call descriptor */
  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
      if (ca->cid == -1 && !newca)
	newca = ca;

      if (ca->cid == je->cid)
	{
	  found  = ca;
	  break;
	}
    }


  ca = found;

  if (!ca)   /* we didn't find a matching call descriptor */
    {
    if (creatit)   
      {
	/* allocate a new one */
	if (!newca)  
	  return 0; /* !!! BUG !!! */
	ca = newca;
	memset(ca, 0, sizeof(*ca));
	ca->cid = -1;
      }
    }

 
  
  if (!ca)
    return 0;


  /* update the call information */

  if (!ca->localrefer)
    {
      ca->cid = je->cid;
      ca->did = je->did;
    }

  if (je->remote_sdp_audio_ip[0])
    {
      strncpy(ca->remote_sdp_audio_ip, je->remote_sdp_audio_ip, sizeof(ca->remote_sdp_audio_ip));
      ca->remote_sdp_audio_port = je->remote_sdp_audio_port;
      strncpy(ca->audio_payload_name, je->payload_name, sizeof(ca->audio_payload_name));
      ca->audio_payload = je->payload;
    }
    
 if (je->remote_sdp_video_ip[0])
    {
      strncpy(ca->remote_sdp_video_ip, je->remote_sdp_video_ip, sizeof(ca->remote_sdp_video_ip));
      ca->remote_sdp_video_port = je->remote_sdp_video_port;
      strncpy(ca->video_payload_name, je->video_payload_name, sizeof(ca->video_payload_name));
      ca->video_payload = je->video_payload;
    }


  return ca;
}




void ph_release_call(phcall_t *ca)
{
  ca->cid = -1;

  if (ph_call_hasaudio(ca))
    {
      ph_call_media_stop(ca);
    }

  memset(ca, 0, sizeof(phcall_t));


}


int ph_has_active_calls()
{
  phcall_t *ca;
  int count = 0;

  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
      if (ca->cid != -1 && ph_call_hasaudio(ca))
	{
	  if (!ca->remotehold && !ca->localhold)
	    count++;
	}
    }

  return count;

}

int ph_call_hasaudio(phcall_t *ca)
{
  if (ca->mses && (ca->mses->activestreams & (1 << PH_MSTREAM_AUDIO1)))
    return 1;

  return 0;
}

void ph_stream_ended(void *ctx, int event)
{
    phcall_t *ca = (phcall_t *)ctx;

    ca->closereq = 1;
}

void ph_wegot_dtmf(void *ctx, int dtmfEvent)
{
    phCallStateInfo_t info;
    phcall_t *ca = (phcall_t *)ctx;

    clear(info);

    info.event = phDTMF;
    info.u.dtmfDigit = dtmfEvent;
    info.vlid = ca->vlid;
    phcb->callProgress(ca->cid, &info);

}

void ph_frame_display_cbk(void *ctx, void *event)
{
  phcall_t *ca = (phcall_t *) ctx;

  phcb->onFrameReady(ca->cid, event);


}





int ph_same_str(const char *str1, const char *str2)
{
  if (str1 == 0)
    return str2 == 0;

  if (str2 == 0)
    return str1 == 0;

  return (0 == strcasecmp(str1, str2));
}

int ph_same_uri(const char *uristr1, const char *uristr2)
{
  osip_contact_t *uri1, *uri2;
  int ret;

  osip_contact_init(&uri1);
  osip_contact_init(&uri2);

  osip_contact_parse(uri1, uristr1);
  osip_contact_parse(uri2, uristr2);

  /* if we've got and invalid URI return TRUE */
  if (!uri1 || !uri2 || !uri1->url || !uri2->url)
    ret = 1;
  else
    ret = ph_same_str(uri1->url->username, uri2->url->username) &&
      ph_same_str(uri1->url->host, uri2->url->host) &&
      ph_same_str(uri1->url->port, uri2->url->port);

  osip_contact_free(uri1);
  osip_contact_free(uri2);

  return ret;
}

   

int 
phGetVersion()
{
  static char version[] = PHAPI_VERSION;
  char *subv = strstr(version, ".");
  int v,s,r;

  v = atoi(version);
  s = atoi(subv+1);
  r = atoi(strstr(subv+1, ".")+1);

  return (v << 16) | (s << 8) | r;
} 
#undef stringize


MY_DLLEXPORT int 
phGetAudioVersion()
{
  int fd, ret=-1;
#if !defined(WIN32) && !defined(__APPLE__) && !defined(__FreeBSD__)
#ifndef EMBED
  fd = open("/dev/dsp", O_RDWR, O_NONBLOCK);
#else
  fd = open("/dev/sound/mixer0", O_RDWR, O_NONBLOCK);
#endif
  if (fd>=0)
    {
#if defined(EMBED)
      if(0>ioctl(fd, SOUND_MIXER_PRIVATE5, &ret))
#else
      if(0>ioctl(fd, OSS_GETVERSION, &ret))
#endif
	ret = -1;
      close(fd);
    }
  return ret;
#else
  return 0;
#endif
} 

MY_DLLEXPORT int 
phGetNatInfo(char *ntstr, int ntlen, char *fwip, int fwiplen)
{
  assert(ntstr);
  assert(fwip);

  strncpy(ntstr, ph_nat_type_str, ntlen);
  strncpy(fwip, ph_nat_router_addr, fwiplen);
  return 0;
}




static void
ph_build_from(char *buf, int n, struct vline *vl)
{
  char *un, *s;

  assert(buf);
  assert(vl);

  un = nonempty(vl->username) ? vl->username : "unknown";
  s = nonempty(vl->server) ? vl->server : "localhost";



  if (vl->displayname && vl->displayname[0])
    {
      if (!strchr(vl->displayname, ' '))
	{
	  snprintf(buf, n, "%s <sip:%s@%s>", vl->displayname, un, s);
	}
      else
	{
	  snprintf(buf, n, "\"%s\" <sip:%s@%s>", vl->displayname, un, s);
	}

    }
  else
    {
      snprintf(buf, n, "<sip:%s@%s>", un, s);
    }
}


static void
ph_build_cname(char *buf, int n, struct vline *vl)
{
  char *un, *s;
  assert(buf);
  assert(vl);

  un = nonempty(vl->username) ? vl->username : "unknown";
  s = nonempty(vl->server) ? vl->server : "localhost";

  snprintf(buf, n, "%s@%s", un, s);

}



MY_DLLEXPORT int 
phLinePlaceCall2(int vlid, const char *uri, void *userdata, int rcid, int streams)
{
   return phLinePlaceCall_withCa(vlid, uri, userdata, rcid, streams, 0);
}

 

#define optional(x) (x[0] ? x : 0)

int
phLinePlaceCall_withCa(int vlid, const char *uri, void *userdata, int rcid, int streams, phcall_t *ca0)
{
  int i;
  osip_message_t *invite;
  phcall_t *ca = 0;
  char *proxy = phcfg.proxy;
  struct vline *vl;
  char from[512];
  char  local_voice_port[16];
  char  local_video_port[16];
#ifdef STUN_ENABLE
  char  public_voice_port[16];
  char  public_video_port[16];

   public_video_port[0] = 0;
   public_voice_port[0] = 0;
#endif
   
   DBG4_SIP_NEGO("phLinePlaceCall_withCa: a new call has been placed\n",0,0,0);


   local_video_port[0] = 0;
   local_voice_port[0] = 0;


  if (!nonempty(uri))
    return -PH_BADARG;
  
  vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  if (rcid)
    {
      ca = ph_locate_call_by_cid(rcid);
      if (!ca)
	return -PH_BADCID;
    }

  ph_build_from(from, sizeof(from), vl);

  proxy = vl->proxy;

  i = eXosip_build_initial_invite(&invite,
				  uri,
				  from,
				  proxy,
				  "");
  if (i!=0)
      return -1;

  if (_is_video_enabled(streams)) 
      _get_local_video_sdp_port(local_video_port);


  _get_local_audio_sdp_port(local_voice_port);
	
	
#ifdef STUN_ENABLE
	
  if (0!=strncasecmp(eXosip_get_nattype(), "none", 4) && (0!=strncasecmp(eXosip_get_nattype(), "sym", 3) ))
    {

	int ret = getPublicPort(local_voice_port, local_video_port, public_voice_port, public_video_port );

	if (ret <1)
	{
		DBG4_SIP_NEGO("unable to alocate public port ...",0,0,0);
		return;
	}
	
	eXosip_lock();
	i = eXosip_initiate_call(invite, userdata, NULL,  local_voice_port,  optional(local_video_port), optional(public_voice_port), 
				 optional(public_video_port)); 
    }
  else
    {
#endif
      eXosip_lock();
      i = eXosip_initiate_call(invite, userdata, NULL, local_voice_port, optional(local_video_port),  0, 0);

#ifdef STUN_ENABLE
    }
#endif

 
  if (!ca0)
    ca0 = ph_allocate_call(i);
  else
    ca0->cid = i;

  ca0->local_sdp_audio_port = atoi(local_voice_port);
  if (_is_video_enabled(streams)) 
    ca0->local_sdp_video_port = atoi(local_video_port);


  ca0->user_mflags = streams;
  ca0->nego_mflags = streams;

  if (rcid)
    ca0->rcid = rcid;

  ca0->vlid = ph_vline2vlid(vl);

  eXosip_unlock();  
  return i;

}


MY_DLLEXPORT int 
phLinePlaceCall(int vlid, const char *uri, void *userdata, int rcid)
{
  return phLinePlaceCall_withCa(vlid, uri, userdata, rcid, PH_STREAM_AUDIO, 0);
}


MY_DLLEXPORT int
phLineSendMessage(int vlid, const char *uri, const char *buff)
{
    int i;
    struct vline *vl;
    char from[512];
    
    vl = ph_valid_vlid(vlid);
    if (!vl) {
        return -PH_BADVLID;
    }
    
    ph_build_from(from, sizeof(from), vl);
    
    if ( !nonempty(uri)) {
        return -PH_BADARG;
    }
    
    eXosip_lock();
    i = eXosip_message((char *)uri, from, vl->proxy, buff);
    eXosip_unlock();  
    return i;
}


MY_DLLEXPORT int 
phSendMessage(const char *from, const char *uri, const char *buff)
{
  int i;

if (!nonempty(from) || !nonempty(uri))
    return -PH_BADARG;

  eXosip_lock();
  i = eXosip_message((char *)uri, (char*) from, ph_get_proxy(from), buff);
  eXosip_unlock();
  return i;
}


MY_DLLEXPORT int 
phLineSubscribe(int vlid, const char *uri, const int winfo)
{
  int i;
  struct vline *vl;
  char from[512];


  vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  ph_build_from(from, sizeof(from), vl);


  if ( !nonempty(uri))
    return -PH_BADARG;


  eXosip_lock();
  i = eXosip_subscribe(uri, from, vl->proxy, winfo);
  eXosip_unlock();  
  return i;
}




MY_DLLEXPORT int
phSubscribe(const char *from, const char *to, const int winfo)
{
  int i;

  if (!nonempty(to) || !nonempty(from))
    return -PH_BADARG;

  eXosip_lock();
  i = eXosip_subscribe(to, from, ph_get_proxy(from), winfo);
  eXosip_unlock();  
  return i;
}



MY_DLLEXPORT int 
phLinePublish(int vlid, const char *uri, const int winfo, const char * content_type, const char * content)
{
  int i;
  struct vline *vl;
  char from[512];


  vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  ph_build_from(from, sizeof(from), vl);


  if ( !nonempty(uri))
    return -PH_BADARG;


  eXosip_lock();
  i = eXosip_publish(uri, from, vl->proxy, winfo, content_type,content);
  eXosip_unlock();  
  return i;
}



MY_DLLEXPORT int
phPublish(const char *from, const char *to, const int winfo, const char * content_type, const char * content)
{
  int i;

  if (!nonempty(to) || !nonempty(from) || !nonempty(content_type) || !nonempty(content))
    return -PH_BADARG;


  i = eXosip_publish(to, from, ph_get_proxy(from), winfo, content_type,content);
	eXosip_unlock();  
	return i;
}

MY_DLLEXPORT void
phRefresh()
{
	if (!phIsInitialized)
		return;
    eXosip_lock();
	eXosip_update();
    eXosip_unlock();
}


MY_DLLEXPORT int
phLineSendOptions(int vlid, const char *to)
{
  int i;
  struct vline *vl;
  char from[512];


  vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  ph_build_from(from, sizeof(from), vl);


  if (!nonempty(to))
    return -PH_BADARG;


  eXosip_lock();
  i = eXosip_options(to, from, vl->proxy);
  eXosip_unlock();  
  return i;

}



MY_DLLEXPORT int 
phSendOptions(const char *from, const char *uri)
{
  int i;

  if (!nonempty(from) || !nonempty(uri))
     return -PH_BADARG;
    
  eXosip_lock();
  i = eXosip_options((char *)uri, (char*) from, ph_get_proxy(from));
  eXosip_unlock();  
  return i;
}


MY_DLLEXPORT int 
phAcceptCall2(int cid, void *userData)
{
	return phAcceptCall3(cid, userData, PH_STREAM_AUDIO);
}


MY_DLLEXPORT int 
phAcceptCall3(int cid, void *userData, int streams)
{
    int i;
    phcall_t *ca = ph_locate_call_by_cid(cid);
    char  local_video_port[16];
    char  local_voice_port[16];
#ifdef STUN_ENABLE
    char  public_voice_port[16];
    char  public_video_port[16];
#endif

    DBG4_SIP_NEGO("SIP NEGO: phAcceptCall3\n", 0, 0, 0);
    if (!ca) {
        return -PH_BADCID;
    }

    ca->user_mflags = streams; // trace of what the user decided
    ca->nego_mflags = ca->user_mflags; // current negociated media flags
  
    if (_is_video_enabled(streams)) {
        _get_local_video_sdp_port(local_video_port);
    }

    _get_local_audio_sdp_port(local_voice_port);

#ifdef STUN_ENABLE
    DBG4_SIP_NEGO(" phAcceptCall3 nattype: %s \n", eXosip_get_nattype(), 0, 0);

    if (0!=strncasecmp(eXosip_get_nattype(), "none", 4) && (0!=strncasecmp(eXosip_get_nattype(), "sym", 3) ))
    {
        int ret = getPublicPort(local_voice_port, local_video_port, public_voice_port , public_video_port );
        if (ret <1)
        {
            DBG4_SIP_NEGO("unable to alocate public port ...",0,0,0);
            return;
        }

        eXosip_lock();
        i = eXosip_answer_call(ca->did, 200, local_voice_port, ph_get_call_contact(ca), local_video_port , public_voice_port , public_video_port);
    }
    else 
    { // start ifdef'ed else clause
#endif
    
        
    eXosip_lock();
    i = eXosip_answer_call(ca->did, 200, local_voice_port, ph_get_call_contact(ca), local_video_port, local_voice_port, local_video_port);


#ifdef STUN_ENABLE
    } // end ifdef'ed else clause
#endif 
 
    if (i == 0)
    {
        i = ph_call_retrieve_payloads(ca, NULL, streams | PH_STREAM_CNG );

        ca->local_sdp_audio_port = atoi(local_voice_port);
        if (_is_video_enabled(streams)) {
            ca->local_sdp_video_port = atoi(local_video_port);
        }

    }
  
    eXosip_unlock();

    if (i == 0)
    {
        i = ph_call_media_start(ca, NULL, streams, 0);
    }

    return i;
}

  
int 
ph_answer_request(int did, int reason, char *ctct)
{
  int i;

  eXosip_lock();
  i = eXosip_answer_call(did, reason, 0, ctct, 0, 0, 0);
  eXosip_unlock();

  return i;
}


MY_DLLEXPORT int 
phRejectCall(int cid, int reason)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  
  if (!ca)
    return -PH_BADCID;

  i = ph_answer_request(ca->did, reason, ph_get_call_contact(ca));

  return i;

}


MY_DLLEXPORT int 
phRingingCall(int cid)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  
  if (!ca)
    return -PH_BADCID;

  i = ph_answer_request(ca->did, 180, ph_get_call_contact(ca));

  return i;

}



MY_DLLEXPORT int 
phCloseCall(int cid)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
	phCallStateInfo_t info;
  int did;
  
  DBG4_SIP_NEGO("phCloseCall %d\n", cid,0,0);
  clear(info);
  info.event = phCALLCLOSED;
  
  if (!ca)
    return -PH_BADCID;
    
  if (ca->isringing)
  {
  	 info.event = phCALLCLOSEDandSTOPRING;
  	 ca->isringing = 0;
  }


  info.vlid = ca->vlid;
  did = ca->did;

  ph_release_call(ca);

  eXosip_lock();
  i = eXosip_terminate_call(cid, did);
  eXosip_unlock();



  if (i)
    return i;

  info.userData = 0;
 
  info.u.errorCode = 0;
  phcb->callProgress(cid, &info);

  
  return i;

}



MY_DLLEXPORT int 
phBlindTransferCall(int cid, const char *uri)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  if (!nonempty(uri))
    return -PH_BADARG;

  if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
    return -PH_REDIRLOOP;  


  ca->localrefer = 1;
  phHoldCall(cid);

  eXosip_lock();
  i = eXosip_transfer_call(ca->did, (char *)uri);
  eXosip_unlock();

  return i;

}


MY_DLLEXPORT int 
phTransferCall(int cid, int tcid)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);
  phcall_t *txca = ph_locate_call_by_cid(tcid);
  int i;


  if (!ca || !txca)
    return -PH_BADCID;

  ca->localrefer = 1;
  ca->txcid = tcid;

  eXosip_lock();
  i = eXosip_transfer_call_to(ca->did, txca->did);
  eXosip_unlock();


  return i;
}

MY_DLLEXPORT  int 
phSetContact(int vlid, const char *uri)
{
  struct vline *vl;


  if (!vlid)
    {
      /* set contact for all VL */
      strncpy(vcontact, uri, sizeof(vcontact));
      return 0;
    }


  vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  if (vl->contact)
      osip_free(vl->contact);

  {
    char contact[256];
    eXosip_guess_contact_uri(uri, contact, sizeof(contact), 1);
    vl->contact = osip_strdup(contact);
  }
  return 0;
}



MY_DLLEXPORT int 
phConf(int cid1, int cid2)
{
  phcall_t *ca1 = ph_locate_call_by_cid(cid1);
  phcall_t *ca2 = ph_locate_call_by_cid(cid2);

  if(!ca1 || !ca2)
    return -PH_BADCFID;

  if (0 > ph_msession_conf_start(ca1->mses, ca2->mses, phcfg.audio_dev))
    return PH_NORESOURCES;
  else
    return 0;
}

MY_DLLEXPORT int 
phStopConf(int cid1, int cid2)
{
  phcall_t *ca1 = ph_locate_call_by_cid(cid1);
  phcall_t *ca2 = ph_locate_call_by_cid(cid2);

  if(!ca1 || !ca2)
    return -PH_BADCFID;

  if( 0 > ph_msession_conf_stop(ca1->mses, ca2->mses))
    return PH_NORESOURCES;
  else
    return 0;
}

#define CONF_MODE 1


MY_DLLEXPORT int 
phResumeCall(int cid)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);
  int i;


  if (!ca)
    return -PH_BADCID;

  if (!ca->localhold)
    return -PH_HOLDERR;

#ifndef CONF_MODE
  if (ph_has_active_calls())
    return -PH_NORESOURCES;
#endif

  ca->localhold = 0;
  ca->localresume = 1;

  eXosip_lock();
  i = eXosip_off_hold_call(ca->did, 0, 0);
  eXosip_unlock();

  return i;
}


MY_DLLEXPORT int 
phHoldCall(int cid)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);
  int i;


  if (!ca)
    return -PH_BADCID;

  if (ca->localhold)
    return -PH_HOLDERR;

  ca->localhold = 1;

  eXosip_lock();
  i = eXosip_on_hold_call(ca->did);
  eXosip_unlock();

  if (!i && ph_call_hasaudio(ca))
    {
#ifndef MEDIA_SUSPEND
    ph_call_media_stop(ca);
#else
    ph_call_media_suspend(ca, 1);
#endif
    }
  return i;
}


MY_DLLEXPORT int 
phSetFollowMe(const char *uri)
{
  if (!uri)
    ph_follow_me_addr[0] = 0;

  if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
    return -PH_REDIRLOOP;  

  strncpy(ph_follow_me_addr, uri, sizeof(ph_follow_me_addr));
  return 0;
  

}




MY_DLLEXPORT int 
phSetBusy(int busyFlag)
{
  ph_busyFlag = busyFlag;
  return 0;
}


MY_DLLEXPORT int 
phLineSetFollowMe(int vlid, const char *uri)
{
  struct vline *vl = ph_valid_vlid(vlid);

  if (!vl)
    return -PH_BADVLID;

  if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
    return -PH_REDIRLOOP;

  if (vl->followme)
    osip_free(vl->followme);

  vl->followme = osip_strdup(uri);

  return 0;

}






MY_DLLEXPORT int 
phLineSetBusy(int vlid, int busyFlag)
{
  struct vline *vl = ph_valid_vlid(vlid);


  if (!vl)
    return -PH_BADVLID;

  vl->busy = busyFlag;
  return 0;
}






MY_DLLEXPORT int
phAddAuthInfo(const char *username, const char *userid,
			       const char *passwd, const char *ha1,
	      const char *realm)
{
  int ret;

  if (!username)
    return -PH_BADARG;

  if (!userid)
    return -PH_BADARG;

  if (!passwd)
    return -PH_BADARG;

  if (!realm)
    return -PH_BADARG;

  eXosip_lock();

  ret = eXosip_add_authentication_info(username, userid, passwd, ha1, realm);

  eXosip_unlock();

  return ret;
}


int
phvlRegister(int vlid)
{
  struct vline *vl = ph_vlid2vline(vlid);
  int ret = -1;
  char utmp[256];
  char stmp[256];
  char *server;


  assert(vl);
  assert(vl->username);
  assert(vl->server);

  snprintf(utmp, sizeof(utmp), "sip:%s@%s", vl->username, vl->server);


  server = stmp;
  if (vl->port && vl->port != 5060)
    {
      /*     snprintf(stmp, sizeof(stmp), "sip:%s@%s:%d", vl->username, vl->server, vl->port); */
      snprintf(stmp, sizeof(stmp), "sip:%s:%d", vl->server, vl->port);
    }
  else
    {
      /*     snprintf(stmp, sizeof(stmp), "sip:%s@%s:%d", vl->username, vl->server, vl->port); */
      snprintf(stmp, sizeof(stmp), "sip:%s", vl->server); 
    }


    
      

  eXosip_lock();

  vl->rid = eXosip_register_init(utmp, server, vl->contact, vl->proxy);

  if (vl->rid >= 0)
    {
      ret = eXosip_register(vl->rid, vl->regTimeout);
     
      if (ret == 0)
	{
	ret = vl->rid;
	vl->lastRegTime = time(0);
	}
    }

  eXosip_unlock();

  return ret;

}

 
MY_DLLEXPORT int 
phSendDtmf(int cid, int dtmfEvent, int mode)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  if (!ph_call_hasaudio(ca))
    return -PH_NOMEDIA;


  return ph_msession_send_dtmf(ca->mses, dtmfEvent, mode);

}





MY_DLLEXPORT int 
phPlaySoundFile(const char *fileName , int loop)
{
  return -1;
}



MY_DLLEXPORT int 
phSendSoundFile(int cid, const char *fileName)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  if (!ph_call_hasaudio(ca))
    return -PH_NOMEDIA;


  return ph_msession_send_sound_file(ca->mses, fileName);

  
}




MY_DLLEXPORT int 
phStopSoundFile()
{
  return -1;
}


MY_DLLEXPORT int 
phSetSpeakerVolume(int cid,  int volume)
{
#if 0
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  return(ph_media_set_spkvol(ca, volume));
#else
  return 0;
#endif
}


MY_DLLEXPORT int 
phSetRecLevel(int cid,  int level)
{
#if 0
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  return(ph_media_set_recvol(ca, level));
#else
  return 0;
#endif
}




MY_DLLEXPORT int 
phAddVline(const char* username, const char *server, const char*  proxy,  int regTimeout)
{
  return phAddVline2(NULL, username, server, proxy,  regTimeout);  
}

/*
  scrap the :port part from the host uri
 */
static char *
ph_scrap_port(char *buf, int bufsize, const char *host, int *port)
{

  assert(buf != 0);
  assert(port != 0);

  *port = 0;

  if (!host)
    return 0;

  if (!strchr(host, ':'))
    return host;


  strncpy(buf, host, bufsize);
  host = strchr(buf, ':');
  if (host)
    {
    *( char *) host = 0;
    *port = atoi(host+1);
    }

  return buf;

}

static const char emptystr[] = { 0 };
#define nonull(x) ((x) ? (x) : emptystr)

MY_DLLEXPORT int
phAddVline2(const char *displayname, const char* username, const char *server, const char*  proxy,  int regTimeout)
{
  struct vline *vl;
  int oldTimeout = 0;
  char srvbuf[256];
  char *srv2;
  int port;


  if (phDebugLevel)
    DBG8_SIP_NEGO("AddVline2(dn = %s, un=%s, srv=%s pxy=%s regT=%d)\n", nonull(displayname), nonull(username), nonull(server), nonull(proxy), regTimeout,0,0);

  srv2 = ph_scrap_port(srvbuf, sizeof(srvbuf), server, &port);

  if (!port)
    port = 5060;

  if (!username)
    username = "";

  
  vl  = ph_find_matching_vline3(username, srv2, port, 0);

  if (0 < regTimeout && regTimeout < 200)
    regTimeout = 200;

  

  if (!vl)
    {
      vl = vline_alloc();
      if (!vl)
	return -PH_NORESOURCES;

      vl->username = osip_strdup(username);
    }
  else
    {
      if (vl->proxy)
	{
	osip_free(vl->proxy);
	vl->proxy = 0;
	}

      if (vl->displayname)
	{
	osip_free(vl->displayname);
	vl->displayname = 0;
	}

      if (vl->contact)
	{
	osip_free(vl->contact);
	vl->contact = 0;
	}
      oldTimeout = vl->regTimeout;

    }

  vl->port = port;

  if (nonempty(proxy))
     {
       if (0 == strstr(proxy, "sip:"))
	 {
	   int l = 15 + strlen(proxy);
	   vl->proxy = osip_malloc(l);
	   snprintf(vl->proxy, l, "<sip:%s;lr>", proxy);
	 }
       else
	 vl->proxy = osip_strdup(proxy);
     }



  if (nonempty(srv2) && !vl->server)
    vl->server = osip_strdup(srv2);

  if (nonempty(displayname))
    vl->displayname = osip_strdup(displayname);

  vl->regTimeout = regTimeout;


  if(vcontact[0])
    vl->contact = osip_strdup(vcontact);
  else
  {
    char contact[512];
    char from[512];

    ph_build_from(from, sizeof(from), vl);
    eXosip_guess_contact_uri(from, contact, sizeof(contact), 1);
    vl->contact = osip_strdup(contact);

  }



  if (nonempty(srv2) && (oldTimeout > 0 || regTimeout > 0))
    phvlRegister(ph_vline2vlid(vl));

  return ph_vline2vlid(vl);
}


MY_DLLEXPORT int 
phDelVline(int vlid)
{
  struct vline *vl;
  phcall_t *ca;

  if (!(vl = ph_valid_vlid(vlid)))
      return -PH_BADVLID;


  /* forbid deletion of the lines which have pending calls */  
  for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS]; ca++)
    {
      if (ca->vlid == vlid && ca->cid > 0)
	return -PH_VLBUSY;
    }

  
  /* 
     if the line has an associatied timeout, 
     it means it is regsitered on some server,
     so we need to unregister 
  */
  if (vl->regTimeout)
    {
      
      eXosip_lock();
      eXosip_register(vl->rid, 0);
      vl->regTimeout = 0;
      vl->used = VL_DELETING;
      eXosip_unlock();

    }
  
  if (vl->used != VL_DELETING)
    return 0;

  return 0;
}



MY_DLLEXPORT int phChangeAudioDevices(const char *devstr)
{
  if (devstr)
    strncpy(phcfg.audio_dev, devstr, sizeof(phcfg.audio_dev));

  return 0;
}




static struct vline *
vline_alloc()
{
  int i;

  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      struct vline *vl = ph_vlines + i;

      if (!vl->used)
	{
	  memset(vl, 0, sizeof(*vl));
	  vl->used = 1;
	  vl->port = 0;
	  return vl;
	}

    }
  return 0;
}

static void
vline_free(struct vline *vl)
{
  if (vl->used)
    {
      osip_free(vl->username);
      if (vl->server)
	osip_free(vl->server);
      if (vl->proxy)
	osip_free(vl->proxy);
      if (vl->displayname)
	osip_free(vl->displayname);
      if (vl->followme)
	osip_free(vl->followme);
      vl->used = 0;
    }
}




static struct vline *
ph_find_vline_by_rid(int rid)
{
  int i;

  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      struct vline *vl = ph_vlines + i;

      if (vl->used && rid == vl->rid)
	return vl;
    }
  return 0;
}


static osip_from_t *
ph_parse_from(const char *userid)
{
  osip_from_t *from;

  osip_from_init(&from);
  if (!from)
    return 0;

  osip_from_parse(from, userid); 
  if (from->url && from->url->port && !strcmp(from->url->port, "5060"))
    {
      osip_free(from->url->port);
      from->url->port = NULL;
    }

  return from;

}

static struct vline *
ph_find_matching_vline(const char *userid, int ignore)
{
  osip_from_t *from;
  struct vline *vl;
  char *host;
  char hostport[256];
  char *uname;

  hostport[0] = 0;
  from = ph_parse_from(userid);

  if (!from)
    return 0;

  if (from->url)
    uname = from->url->username;
  else
    uname = hostport;


  if (!(ignore & PHM_IGNORE_PORT) && from->url && from->url->port)
    {
      snprintf(hostport, sizeof(hostport), "%s:%s", from->url->host, from->url->port);
      host = hostport;
    }
  else if (ignore & PHM_IGNORE_HOST)
    host = 0;
  else if (from->url)
    {
      host = from->url->host;
    }
  else
    host = hostport;

  vl = ph_find_matching_vline2(uname, host, ignore);

  osip_from_free(from);

  return vl;

}


static struct vline *
ph_find_matching_vline2(const char *username, const char* host, int ignore)
{
  return ph_find_matching_vline3(username, host, 0, ignore);
}

static struct vline *
ph_find_matching_vline3(const char *username, const char* host, int port, int ignore)
{
  int i;
  int   hostlen;
  int   unamelen;
  struct vline *vl, *defaultvl = 0;

  if (!username)
    username = "";
  
  hostlen = host ? strlen(host) : 0;
  unamelen =  strlen(username);

  if (port == 0)
    port = 5060;

  for( i = 0; i < PH_MAX_VLINES; i++)
    {
      int len;
      vl = ph_vlines + i;

      if (!vl->used)
	continue;

      if (!vl->server)
	{
	  defaultvl = vl;
	  continue;
	}
      
      len = strlen(vl->server);
      
      if ((ignore & PHM_IGNORE_HOST) || ((len == hostlen) && !strcasecmp(host, vl->server)))
	{
	  len = strlen(vl->username);

	  if ((len == unamelen) && !strcasecmp(username, vl->username))
	    {
	      if (ignore & PHM_IGNORE_PORT)
		{
		  defaultvl = vl;
		  break;
		}
	      if (port == vl->port)
		{
		  defaultvl = vl;
		  break;
		} 
	    }
	}
    }



  return defaultvl;
}



static void
ph_nat_refresh(struct vline *vl)
{
  char buf[128];
  char to[128];
  assert(vl);

  ph_build_from(buf, sizeof(buf), vl);
  snprintf(to, sizeof(to), "sip:ping@%s", vl->server);
  phSendOptions(buf, to);

#if 0
  int port;


  osip_uri_init(&uri);
  osip_uri_parse(uri,  (vl->proxy && vl->proxy[0]) ? vl->proxy : vl->server);

  port = atoi((uri->port && uri->port[0]) ? uri->port : "5060");
  phPing(uri->host, port, 30);

  osip_uri_free(uri);
#endif

}

static time_t last_vline_refresh, last_nat_refresh;
static void 
ph_refresh_vlines()
{
  time_t now = time(NULL);

  if (now - last_vline_refresh > 5)
    {
      int i;
      struct vline *vl;

      for( i = 0; i < PH_MAX_VLINES; i++)
	{
	  vl = ph_vlines + i;  
	  if (!vl->used)
	    continue;

	  if (vl->server && vl->server[0] && vl->regTimeout > 0)
	    {
	      if ((now - vl->lastRegTime) > (vl->regTimeout - 5))
		phvlRegister(ph_vline2vlid(vl));
	    }
	}
      last_vline_refresh = time(0);
    }

  if (!phcfg.use_tunnel && phcfg.nat_refresh_time > 0)
    {
      int i;
      struct vline *vl;

      if (now - last_nat_refresh > phcfg.nat_refresh_time)
	    {
	    for( i = 0; i < PH_MAX_VLINES; i++)
          {
          vl = ph_vlines + i;  
          if (!vl->used)
		     continue;

          if (vl->server && vl->server[0] && vl->regTimeout > 0)
		    {
		    ph_nat_refresh(vl);
		    }
          }
	
	  last_nat_refresh = time(0);
	}
    }


}
  

/*
  scan call marked for close and close them while delivering phCALLCLOSED to phApi client
 */
static void
ph_scan_calls()
{
  int i;
  phcall_t *ca;

  ca = ph_calls;
  for(i = PH_MAX_CALLS; i; i--, ca++)
    {
      if (ca->cid > 0 && ca->closereq)
	{
	  phCallStateInfo_t info;
	  int cid = ca->cid;
	  
	  memset(&info, 0, sizeof(info));
	  info.vlid = ca->vlid;
	  info.event = phCALLCLOSED;

	  ph_release_call(ca);
	  


	  phcb->callProgress(cid, &info);
	}
    }

}

static char *
ph_get_proxy(const char *userid)
{
  struct vline *vl = ph_find_matching_vline(userid, PHM_IGNORE_PORT);

  if (!vl)
    return "";

  return vl->proxy ? vl->proxy : "";
}


static char *
ph_get_call_contact(phcall_t *ca)
{
  struct vline *vl;
  assert(ca);

  vl = ph_vlid2vline(ca->vlid);
  if (!vl)
    return 0;

 return vl->contact;
}
  

static int
ph_get_vline_id(const char *userid, const char *altid)
{

  struct vline *vl = ph_find_matching_vline(userid, PHM_IGNORE_PORT);

  if (vl)
    return ph_vline2vlid(vl);

  if (nonempty(altid))
    {
      vl = ph_find_matching_vline(altid, PHM_IGNORE_PORT);
      if (vl)
	return ph_vline2vlid(vl);
    }

  vl = ph_find_matching_vline(userid, PHM_IGNORE_HOST|PHM_IGNORE_PORT);
  if (vl)
    return ph_vline2vlid(vl);


  if (nonempty(altid))
    {
      vl = ph_find_matching_vline(altid, PHM_IGNORE_HOST|PHM_IGNORE_PORT);
      if (vl)
	return ph_vline2vlid(vl);
    }

  return 0;

}

static void setup_payload(const char *ptstring)
{
    char  tmp[64];
    char  num[8];
    ph_media_payload_t  pt;

    DBG4_CODEC_LOOKUP("trying to setup codec in eXosip: %s\n", ptstring, 0, 0);
    if (ph_media_supported_payload(&pt, ptstring))
    {
        DBG4_CODEC_LOOKUP("...setup accepted : %d - %s/%d\n", pt.number, pt.string, pt.rate);
        snprintf(num, sizeof(num), "%d", pt.number);
        snprintf(tmp, sizeof(tmp), "%d %s/%d/1", pt.number, pt.string, pt.rate);
    
        eXosip_sdp_negotiation_add_codec(
                                        osip_strdup(num),
                                        NULL,
                                        osip_strdup("RTP/AVP"),
                                        NULL, NULL, NULL,
                                        NULL,NULL,
                                        osip_strdup(tmp));
        return;
    }
    DBG4_CODEC_LOOKUP("...setup refused - not found in ortp profile", 0, 0, 0);

}



static void setup_video_payload(const char *ptsring)
{
  char  tmp[64];
  char  num[8];
  ph_media_payload_t  pt;

  if (ph_media_supported_payload(&pt, ptsring))
    {
      snprintf(num, sizeof(num), "%d", pt.number);
      snprintf(tmp, sizeof(tmp), "%d %s/%d/1", pt.number, pt.string, pt.rate);

      eXosip_sdp_negotiation_add_videocodec(osip_strdup(num),
				   NULL,
				   osip_strdup("RTP/AVP"),
				   NULL, NULL, NULL,
				   NULL,NULL,
				   osip_strdup(tmp));
    } 
  else 
    {
      DBG("unsupported payload");
    }
}





/**
 * Initialize the phoneapi module
 */
static eXosip_tunnel_t *phTunnel;


#ifdef OS_WIN32

static int
wsock_init()
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int i;
    
  wVersionRequested = MAKEWORD(1,1);
  if(i = WSAStartup(wVersionRequested,  &wsaData))
       {
	return -1;
       }
  return 0;
}
#else
#define wsock_init() 0
#endif


static int
ph_debug_init()
{
  const char *dbgstr;

  dbgstr = getenv("PH_DEBUG_LEVEL");
  if (dbgstr)
    phDebugLevel = atoi(dbgstr);

  if (phDebugLevel > 0)
    {
      if (!phLogFileName)
	phLogFileName = getenv("PH_LOG_FILENAME");

	  
      ph_log_file = phLogFileName ? fopen (phLogFileName, "w+") : stdout;

      
      if (!ph_log_file)
        {
          perror ("phapi: log file");
          return -1;
        }
      TRACE_INITIALIZE (phDebugLevel, ph_log_file);
    }

  return 0;

}


static void 
ph_avcodec_init()
{
#ifdef PHAPI_VIDEO_SUPPORT
  avcodec_init();
  avcodec_register_all();
  phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_128KBPS;
#endif

}


static void
ph_calls_init()
{
  int i;

  for( i = 0; i < PH_MAX_CALLS; i++)
    ph_calls[i].cid = -1;

}

static int
ph_tunnel_init()
{
#ifdef USE_HTTP_TUNNEL
  eXosip_tunnel_t *tunnel = NULL;
  int port;
  char *c;
  char buf[256];
  int tunerr;

  if (!phcfg.use_tunnel) 
    return 0;

  http_tunnel_init_host(phcfg.httpt_server, phcfg.httpt_server_port, 0);
  http_tunnel_init_proxy(phcfg.http_proxy,phcfg.http_proxy_port, 
			 phcfg.http_proxy_user, phcfg.http_proxy_passwd);
  
  tunnel = malloc(sizeof(eXosip_tunnel_t));
  if (tunnel) 
    {
      strncpy(buf, phcfg.proxy, sizeof(buf));
      c = strstr(buf, ":");
      
      port = 5060;
      if (c) 
	{
	  *c++ = 0;
	  port = atoi(c);
	}
      tunnel->h_tunnel = http_tunnel_open(buf, port, HTTP_TUNNEL_VAR_MODE, &tunerr, -1);
      if (!tunnel->h_tunnel)
	{
	  if (!tunnel->h_tunnel)
	    {
	      free(tunnel);
	      return -PH_NOTUNNEL;
	    }
	}
      
      tunnel->tunnel_recv = http_tunnel_recv;
      tunnel->tunnel_send = http_tunnel_send;
      tunnel->get_tunnel_socket = http_tunnel_get_socket;
      phTunnel = tunnel;
      return 0;
    }



  
  return -PH_NORESOURCES;
#else
  return 0;
#endif
}

static void
ph_nat_init()
{
  char *ntstr = 0;

  ph_nat_router_addr[0] = 0;
  ph_nat_port_str[0] = 0;
  ph_nat_type_str[0] = 0;

  if (!phcfg.use_tunnel && phcfg.nattype[0])
    {
      if (!strncasecmp(phcfg.nattype, "auto", 4))
	{
		NatType ntype;
	 int resPort = 0, hairpin = 0;
	 int needMappedAddress = 0;
	 StunAddress4 stunServerAddr;

	 ntstr = "sym";

	 stunParseServerName(phcfg.stunserver, &stunServerAddr);
	 ph_nat_type = ntype = stunNatType( &stunServerAddr,  phDebugLevel > 1, &resPort, &hairpin, 
                                      0, 0);


	switch (ntype)
	  {
	    
	    
	  case StunTypeOpen:
	    ntstr = 0;
	    break;
	    
	  case StunTypeConeNat:
	    ntstr = "fcone";
	    needMappedAddress = 1;
	    break;
	    
	  case StunTypeRestrictedNat:
	    ntstr = "rcone";
	    needMappedAddress = 1;
	    break;
	    
	  case StunTypePortRestrictedNat:
	    ntstr = "prcone";
	    needMappedAddress = 1;
	    break;
	    
	  case StunTypeSymFirewall:
	  case StunTypeSymNat:
	    needMappedAddress = 1;
	    break;
	    
	    
	  case StunTypeFailure:
	  case StunTypeUnknown:
	  case StunTypeBlocked:
	  default:
	    break;
	    
	  }
	
	if (needMappedAddress)
	  {
	    StunAddress4 mappedAddr;
	    Socket  sock;
	    
	    sock = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(_get_local_sip_port()), NULL, 0); 
	    
	    if (-1 !=  (int) sock)
	      {
		ipv4tostr(ph_nat_router_addr, mappedAddr);
		snprintf(ph_nat_sip_port_str, sizeof(ph_nat_sip_port_str), "%d", mappedAddr.port);
		stunCloseSocket(sock);
	      }
	    
	    
	  }
	
	
	}
      else if (!strncasecmp(phcfg.nattype, "fcone", 5) ||
	       !strncasecmp(phcfg.nattype, "rcone", 5) ||
	       !strncasecmp(phcfg.nattype, "prcone", 6) ||
	       !strncasecmp(phcfg.nattype, "sym", 3))
	
	ntstr = phcfg.nattype;
    }


  if (ntstr)
    {
      eXosip_set_nattype(ntstr);
      if (!phcfg.nat_refresh_time)
	phcfg.nat_refresh_time = 15;

      strncpy(ph_nat_type_str, ntstr, sizeof(ph_nat_type_str));
      if (ph_nat_router_addr[0] && strcmp(ntstr, "sym"))
	{
	  eXosip_set_mediaip(ph_nat_router_addr);
	  eXosip_set_firewallip(ph_nat_router_addr);
	  eXosip_set_firewallport(ph_nat_sip_port_str);
	}

    }
  else
    {
      phcfg.nat_refresh_time = 0;
      strcpy(ph_nat_type_str, "open");
    }

}

/**
 * @brief initialize the payload/codecs that are allowed to be handled by the SIP stack
 */
static void
ph_payloads_init()
{

  /* reset all payload to fit application capabilities */
  eXosip_sdp_negotiation_remove_audio_payloads();

  // init payload/codecs for VIDEO
    
#ifdef PHAPI_VIDEO_SUPPORT
  setup_video_payload("H263/90000");

#if 0
  if (!phcfg.video_codecs[0]) {
      setup_video_payload("H263/90000");
      setup_video_payload("H264/90000");
  } else {
      char tmp[32];
      char *tok = strtok(phcfg.video_codecs, ",");

      while(tok) {
	snprintf(tmp, sizeof(tmp), "%s/90000", tok);
	setup_video_payload(tmp);
	DBG2("phapi: added video codec: %s\n",tmp);
	tok = strtok(0, ",");
	}
  }
#endif
#endif

  // init payload/codecs for AUDIO

  // add codecs out of an ENV var
  {
    char *aclist = getenv("PH_AUDIO_CODECS");
    if (aclist) {
      strncpy(phcfg.audio_codecs, aclist, sizeof(phcfg.audio_codecs));
    }
    
   }


  // limit codecs to G711 codecs according to compile time DEFINE
#ifdef G711_ONLY
  strcpy(phcfg.audio_codecs, "PCMU/8000,PCMA/8000");
#endif

  // if at this stage, no codecs are required, fix a default list
  if (!phcfg.audio_codecs[0])
    {
      setup_payload("PCMU/8000");
      setup_payload("PCMA/8000");
      setup_payload("GSM/8000");
      setup_payload("ILBC/8000");
      setup_payload("SPEEX/16000");
      setup_payload("SPEEX/8000");
      setup_payload("AMR/8000");
      setup_payload("AMR-WB/16000");

    }
    // phapi.h client has required a specific list of codecs
  else
    {
    
        /*
        The list is "," separated
        some hacks are present to allow for :
        - payload usurpation
        - default /8000 clockrate when not specified
        - ... look at the code
        */
        
        
      char tmp[32];
      char *tok = strtok(phcfg.audio_codecs, ",");

      while(tok)
	{
	  if(!strcmp(tok, "AMR-WB"))
	  {
#ifdef AMR_OVER_G729_HACK
	    snprintf(tmp, sizeof(tmp), "G729/8000");
#else
	    snprintf(tmp, sizeof(tmp), "%s/16000", tok);
#endif
	  }
#ifdef SPEEX_OVER_G729_HACK
	  else if(!strcmp(tok, "SPEEX/16000"))
	  {
	    snprintf(tmp, sizeof(tmp), "G729/8000");
	  }
#endif
	  else if (strchr(tok, '/'))
	    strncpy(tmp, tok, sizeof(tmp));
	  else
	    snprintf(tmp, sizeof(tmp), "%s/8000", tok);
	  
	  if (ph_media_can_handle_payload(tmp))
	    {
	      setup_payload(tmp);
	    }

	  tok = strtok(0, ",");
	}
    }

  // set codec in sip stack for CNG (confort noise generator=
  if(phcfg.cng)
    setup_payload("CN/8000");
  
  // set codec in sip stack for DTMF
  setup_payload("telephone-event/8000");
}


MY_DLLEXPORT int 
phInit(phCallbacks_t *cbk, char * server, int asyncmode)
{
  int i;
  char buf[200];
    
  memset(vcontact, 0, sizeof(vcontact));

  i = ph_debug_init();

  if (i)
    return i;

  ph_avcodec_init();
  ph_calls_init();

  if (phcfg.use_tunnel) 
    {
      i = ph_tunnel_init();
      if (i)
        return i;
    }

  if (phcfg.proxy[0])
    {
      char tmp[256];
      
      if (!strchr(phcfg.proxy, '<'))
        {
          snprintf(tmp, sizeof(tmp), "<sip:%s;lr>", phcfg.proxy); 
          strncpy(phcfg.proxy, tmp, sizeof(phcfg.proxy));
        }
    }

#ifdef FORCE_VAD
  /* HACK for test */
#ifdef EMBED
  phcfg.vad = VAD_VALID_MASK | (500 & VAD_THRESHOLD_MASK);
#else
  phcfg.vad = VAD_VALID_MASK | (1000 & VAD_THRESHOLD_MASK);
#endif
#endif

#ifdef FORCE_CNG
  /* HACK for test */
  phcfg.cng = 1;
#endif

  ph_media_init(phcfg.use_tunnel, phcfg.plugin_path);

  i = eXosip_init(0, 0, atoi(phcfg.sipport), phTunnel);

  if (i)
    return -1;

  {
    const char * ua  = "phapi/eXosip/" PHAPI_VERSION;
    eXosip_set_user_agent(ua);
  }

  ph_nat_init();

  // if (phDebugLevel > 0)
  DBG4_SIP_NEGO("NAT type: %s fw=%s \n", ph_nat_type_str, ph_nat_router_addr,0);

  if (phcfg.force_proxy)
    eXosip_force_proxy(phcfg.proxy);

  {
    char contact[512];
   
    eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
    eXosip_set_answer_contact(contact);
  }
  
  memset(ph_vlines, 0, sizeof(ph_vlines));

  ph_payloads_init();

  /* register callbacks? */
  eXosip_set_mode(EVENT_MODE);
  phcb = cbk;
  phcfg.asyncmode = asyncmode;
  if (!asyncmode)
    timeout = 1;
  else
    timeout = 500;

  if (asyncmode)
    osip_thread_create(20000, ph_api_thread, 0);

  phIsInitialized = 1;


  return 0;
}



int phTunnelConfig(const char* http_proxy, const int http_proxy_port,
								const char* httpt_server, const int httpt_server_port, 
								const char *proxy_user, const char* proxy_passwd,
								int autoconf)
{
  phcfg.httpt_server[0] = 0;
  phcfg.http_proxy[0] = 0;
  phcfg.use_tunnel = 0;
  
  if (!httpt_server) {
    return -1;
  }
  
  phcfg.http_proxy_port = http_proxy_port;
  if (!httpt_server_port)
    if (!http_proxy) {
      phcfg.httpt_server_port = 80;
    }
    else {
      phcfg.httpt_server_port = 443;
    }
  else
    phcfg.httpt_server_port = httpt_server_port;
  
  if (httpt_server)
    strncpy(phcfg.httpt_server, httpt_server, sizeof(phcfg.httpt_server));
  if (http_proxy)
    strncpy(phcfg.http_proxy, http_proxy, sizeof(phcfg.http_proxy));
  
  if (proxy_user)
    strncpy(phcfg.http_proxy_user, proxy_user, sizeof(phcfg.http_proxy_user));
  if (proxy_passwd)
    strncpy(phcfg.http_proxy_passwd, proxy_passwd, sizeof(phcfg.http_proxy_passwd));
  
  phcfg.use_tunnel = PH_TUNNEL_USE;
  if (autoconf)
    phcfg.use_tunnel |= PH_TUNNEL_AUTOCONF;
  return 0;
}



/**
 * terminate ph api
 */
MY_DLLEXPORT void 
phTerminate()
{
  int i;
	
  DBG4_SIP_NEGO("SIP NEGO: phTerminate\n",0,0,0);
  if (!phIsInitialized)
	  return;


  for(i = 0; i < PH_MAX_CALLS; i++)
    if (ph_calls[i].cid != -1)
      ph_release_call(&ph_calls[i]);



  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      phDelVline(i+1);
    }


    
  usleep(200000);

  phPoll();
    
  phIsInitialized = 0;

  eXosip_quit();
  
#ifdef USE_HTTP_TUNNEL
  if (phTunnel)
    {
      http_tunnel_close(phTunnel->h_tunnel);
      free(phTunnel);
      phTunnel = 0;
    }
#endif
  
  ph_media_cleanup();




  if (phLogFileName && phDebugLevel > 0)
    fclose(ph_log_file);
  if (phDebugLevel > 0)
    for (i = 0; i <= phDebugLevel && i < END_TRACE_LEVEL; ++i)
      TRACE_DISABLE_LEVEL(i);




}

 

/**
 * poll for phApi events.c 
 */
MY_DLLEXPORT int
phPoll()
{
  if (!phIsInitialized)
    return -1;

  if (!phcfg.asyncmode)
    {
      ph_event_get();
      ph_keep_refreshing();
    }
  return 0;
}



void ph_refer_notify(int did, int status, const char* msg, int final)
{
  char  statusmsg[128];

  snprintf(statusmsg, sizeof(statusmsg), "SIP/2.0 %d %s", status, msg);

  eXosip_lock();

  eXosip_transfer_send_notify(did, final ? EXOSIP_SUBCRSTATE_TERMINATED : EXOSIP_SUBCRSTATE_ACTIVE,
                             statusmsg);

  eXosip_unlock();

}

static int
ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags)
{
    int  i = 0;

    DBG4_SIP_NEGO("looking for payloads...\n", 0, 0, 0);
    DBG4_SIP_NEGO("audio...\n", 0, 0, 0);
    if (_is_audio_enabled(flags))
    {
        i = eXosip_retrieve_negotiated_audio_payload(ca->did, &ca->audio_payload, ca->audio_payload_name, sizeof(ca->audio_payload_name));
        DBG4_SIP_NEGO("remote_audio=%s payload=%s(%d)\n", ca->remote_sdp_audio_ip, ca->audio_payload_name, ca->audio_payload); 
    }

    DBG4_SIP_NEGO("video...\n", 0, 0, 0);
    ca->video_payload = 0;
    if (ca->remote_sdp_video_ip[0] && (_is_video_enabled(flags)))
    {
        i = eXosip_retrieve_negotiated_video_payload(ca->did, &ca->video_payload, ca->video_payload_name, sizeof(ca->video_payload_name));
        DBG4_SIP_NEGO("remote_video=%s payload=%s(%d)\n", ca->remote_sdp_video_ip, ca->video_payload_name, ca->video_payload); 
    }

    DBG4_SIP_NEGO("cng...\n", 0, 0, 0);
    if(!i && phcfg.cng && (flags & PH_STREAM_CNG)) {
        ca->cng = !eXosip_retrieve_negotiated_specific_payload(ca->did, PH_MEDIA_CN_PT_STR, strlen(PH_MEDIA_CN_PT_STR));
        DBG4_SIP_NEGO("cng: %d", ca->cng, 0, 0);
    }

    return i;
}


MY_DLLEXPORT int phCallGetCodecs(int cid, char *audioCodecBuf, int aBufLen, char *videoCodecBuf, int vBufLen)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);
  

  if (!ca)
    return -PH_BADCID;


  if (audioCodecBuf)
    strncpy(audioCodecBuf, ca->audio_payload_name, aBufLen);


  if (videoCodecBuf)
    strncpy(videoCodecBuf, ca->video_payload_name, vBufLen);


  return 0;

  
}

static void
ph_parse_payload_mime(struct ph_media_payload_s *pt, const char *mime, int rate, int chans)
{
  const char *rp;
  const char *cp;
  int len;

  rp = strchr(mime, '/');
  strncpy(pt->string, mime, sizeof(pt->string));
  pt->rate = rate;
  pt->chans = chans;

  if (!rp)
    return;

  rp++;
  if (!*rp)
    return;
 
  pt->rate = atol(rp);

  cp = strchr(rp, '/');
  if (!cp)
    return;

  cp++;
  if (!*cp)
    return;

  pt->chans = atol(cp);
}



static int 
ph_call_media_stop(phcall_t *ca)
{
  if (ca->mses)
    {
      if (!ph_msession_stopped(ca->mses))
	ph_msession_stop(ca->mses, phcfg.audio_dev);
      free(ca->mses);
      ca->mses = 0;
    }

}

static int
ph_call_media_suspend(phcall_t *ca, int localhold)
{
  if (ca->mses)
    {
      ph_msession_suspend(ca->mses, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev);
    }

  return 0;
  

}


static int
ph_call_media_resume(phcall_t *ca, int localhold)
{
  if (ca->mses)
    {
      ph_msession_resume(ca->mses, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev);
    }

  return 0;
  

}




static int 
ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int flags, int resumeflag)
{
  int i = 0;
  int port_audio, port_video;
  phFrameDisplayCbk  frameDisplay = 0;
  struct ph_msession_s *s, *olds;

  if (phcfg.nomedia || ca->localhold || ca->remotehold) 
    {
      return 0;
    }

  s = ca->mses;

  if (!s)
   s = ca->mses = (struct ph_msession_s *)calloc(sizeof(struct ph_msession_s), 1);

  if (!s)
    return -PH_NORESOURCES;


  s->cbkInfo = ca;
  s->newstreams = 0;

  port_audio = ca->local_sdp_audio_port;
  port_video = ca->local_sdp_video_port;
    
  if ((_is_video_enabled(ca->user_mflags)) && ca->video_payload && ca->remote_sdp_video_ip[0])
    {
      ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_RX;
      ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_TX;
      frameDisplay = phcb->onFrameReady;
      DBG4_SIP_NEGO("will have video stream ip: %s payload=%d\n", ca->remote_sdp_video_ip, ca->video_payload,0);
      DBG4_SIP_NEGO("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags,0); 
    } 
  else 
    {
	// video is not negociated
	ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_RX;
	ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_TX;
	DBG4_SIP_NEGO("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags,0); 
    }


  if (ca->nego_mflags & (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_RX))
    {
      struct ph_mstream_params_s *msp = &s->streams[PH_MSTREAM_VIDEO1];
      int ttype;

      s->newstreams |= (1 << PH_MSTREAM_VIDEO1);

      ttype = ca->nego_mflags & (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_RX);
      
      if (ttype == (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_RX))
	msp->traffictype = PH_MSTREAM_TRAFFIC_IO;
      else if (ttype == PH_STREAM_VIDEO_RX)
	msp->traffictype = PH_MSTREAM_TRAFFIC_IN;
      else if (ttype == PH_STREAM_VIDEO_TX)
	msp->traffictype = PH_MSTREAM_TRAFFIC_OUT;

      if (phcfg.use_tunnel)
	msp->flags |= PH_MSTREAM_FLAG_TUNNEL;

      if (je)
	strncpy(msp->remoteaddr, je->remote_sdp_video_ip, sizeof(msp->remoteaddr));
      else
	strncpy(msp->remoteaddr, ca->remote_sdp_video_ip, sizeof(msp->remoteaddr));


      msp->localport = port_video;
      msp->remoteport = ca->remote_sdp_video_port;
      msp->ipayloads[0].number = ca->video_payload;
      ph_parse_payload_mime(&msp->ipayloads[0], ca->video_payload_name, 90000, 1);
      msp->opayloads[0] = msp->ipayloads[0];
      
      s->frameDisplayCbk =  ph_frame_display_cbk;

      msp->videoconfig = phcfg.video_config.video_line_configuration;
    }



#ifdef PHAPI_VIDEO_LOCAL_HACK
    s->frameDisplayCbk = ph_display_cbk;
    ca->video_payload = 34;
#endif

    if (_is_audio_enabled(ca->nego_mflags) && (!je || je->remote_sdp_audio_ip[0]))
      {
	struct ph_mstream_params_s *msp = &s->streams[PH_MSTREAM_AUDIO1];
	
	s->newstreams |= (1 << PH_MSTREAM_AUDIO1);

	s->dtmfCallback = ph_wegot_dtmf;
	s->endCallback = ph_stream_ended;


	if (phcfg.vad & 0x80000000)
	  {
	    msp->flags |= PH_MSTREAM_FLAG_VAD;
	    msp->vadthreshold = phcfg.vad & 0x7fff;
	  }

	if (phcfg.cng)
	  msp->flags |= PH_MSTREAM_FLAG_CNG;


	msp->jitter = phcfg.jitterdepth;

	if (!phcfg.noaec)
	  msp->flags |= PH_MSTREAM_FLAG_AEC;

	msp->traffictype = PH_MSTREAM_TRAFFIC_IO;
	if (phcfg.use_tunnel)
	  msp->flags |= PH_MSTREAM_FLAG_TUNNEL;

	if (je)
	  strncpy(msp->remoteaddr, je->remote_sdp_audio_ip, sizeof(msp->remoteaddr));
	else
	  strncpy(msp->remoteaddr, ca->remote_sdp_audio_ip, sizeof(msp->remoteaddr));


	// SPIKE_HDX
	if (phcfg.hdxmode == PH_HDX_MODE_MIC)
	  {
	    msp->flags |= PH_MSTREAM_FLAG_MICHDX;
	    msp->vadthreshold = phcfg.vad & 0x7fff;
	  }

	if (phcfg.hdxmode == PH_HDX_MODE_SPK)
	  {
	    msp->flags |= PH_MSTREAM_FLAG_SPKHDX;
	    msp->vadthreshold = phcfg.vad & 0x7fff;
	  }



	msp->localport = port_audio;
	msp->remoteport = ca->remote_sdp_audio_port;
	msp->ipayloads[0].number = ca->audio_payload;
	ph_parse_payload_mime(&msp->ipayloads[0], ca->audio_payload_name, 8000, 1);
	msp->opayloads[0] = msp->ipayloads[0];

      }

    if (s->newstreams | s->activestreams)
      {
	if (resumeflag)
	  {
	    if (ph_msession_resume(s, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev))
	      i = -PH_NOMEDIA;
	  }
	else if (ph_msession_start(s, phcfg.audio_dev))
	  {
	    i = -PH_NOMEDIA;
	  }
		
      }
    else
      {
	i = -PH_NOMEDIA;
      }

    return i;
}


void 
ph_call_new(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca;
  struct vline *vl;

  clear(info);
  if (ph_busyFlag)
    {
      ph_answer_request(je->did, 486, 0);
      return;
    }


  if (ph_follow_me_addr[0])
    {
      ph_answer_request(je->did, 302, ph_follow_me_addr);
      return;
    }


  info.vlid = ph_get_vline_id(je->local_uri, je->req_uri);

  if (!info.vlid)
    {
      ph_answer_request(je->did, 404, 0);
      return;
    }

  vl = ph_vlid2vline(info.vlid);

  assert(vl);

  if (vl->busy)
    {
      ph_answer_request(je->did, 486, vl->contact);
      return;
    }


  if (vl->followme && vl->followme[0])
    {
      ph_answer_request(je->did, 302, vl->followme);
      return;
    }

  
  ca = ph_locate_call(je, 1);

  if (ca)
    {
    ca->vlid = info.vlid;
    ph_build_cname(ca->cname, sizeof(ca->cname), ph_vlid2vline(ca->vlid));

    info.userData = je->external_reference;
    info.event = phINCALL;
    info.u.remoteUri = je->remote_uri; 
    info.localUri = je->local_uri;
    info.streams = PH_STREAM_AUDIO;
    if (ca->video_payload)
      info.streams |= PH_STREAM_VIDEO_RX;

    phcb->callProgress(ca->cid, &info);
    }
  else
    {
      ph_answer_request(je->did, 500, vl->contact);
    }

}

void 
ph_call_replaces(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *oldca;

  clear(info);


  ca = ph_locate_call(je, 1);

  if (ca)
  {
    oldca = ph_locate_call_by_cid(je->replacedcid);

    if (oldca)
      {
	info.userData = je->external_reference;
	info.event = phCALLREPLACED;
	info.u.remoteUri = je->remote_uri; 
	info.localUri = je->local_uri;
	info.newcid =  je->cid;
       
	info.vlid = oldca->vlid;
	
	if (!ca->vlid)
	  ca->vlid = info.vlid;

	phcb->callProgress(oldca->cid, &info);

	ph_call_media_stop(oldca);

	phAcceptCall(je->cid);


      }
  }

}



void 
ph_call_answered(eXosip_event_t *je)
{
    phCallStateInfo_t info;
    phcall_t *ca, *rca=0;

    DBG4_SIP_NEGO("SIP NEGO: ph_call_answered\n", 0, 0, 0);
    clear(info);
    
    ca = ph_locate_call(je, 1);
    if (ca)
    {
        rca = ph_locate_call_by_cid(ca->rcid);
    }
    
    ph_call_retrieve_payloads(ca, je, -1);
    ph_call_media_start(ca, je, -1, ca->localresume);
    
    info.localUri = je->local_uri;
    info.userData = je->external_reference;
    if (ca->localhold)
    {
        info.event = phHOLDOK;
    }
    else if (ca->localresume)
    {
        info.event = phRESUMEOK;
        ca->localresume = 0;
    }
    else {
        info.event = phCALLOK;
    }
    
    info.u.remoteUri = je->remote_uri; 
    info.vlid = ca->vlid;
    
    info.streams = PH_STREAM_AUDIO;
    if (ca->video_payload)
    {
        info.streams |= PH_STREAM_VIDEO_RX;
    }
    
    if (!ca->localrefer)
    {
        phcb->callProgress(ca->cid, &info);
    }
    
    if (rca)
    {
        ph_refer_notify(rca->rdid, je->status_code, "Answered", 1);
    }

}


void 
ph_call_proceeding(eXosip_event_t *je)
{
    phCallStateInfo_t info;
    phcall_t *ca, *rca=0;
    int cng = 0;
    
    DBG4_SIP_NEGO("SIP NEGO: ph_call_proceeding\n", 0, 0, 0);
    clear(info);
    
    ca = ph_locate_call(je, 1);
    if (ca)
    {
        rca = ph_locate_call_by_cid(ca->rcid);
    }
    
    if (!ca->localrefer)
    {
        ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);
        ph_call_media_start(ca, je, -1, 0);
    
        info.userData = je->external_reference;
        info.event = phDIALING;
        info.u.remoteUri = je->remote_uri; 
        info.vlid = ca->vlid;
        
        info.streams = ca->nego_mflags;
        
        phcb->callProgress(ca->cid, &info);
    }
    
    if (rca)
    {
        ph_refer_notify(rca->rdid, je->status_code, "Proceeding", 0);
    }
    
}

void 
ph_call_redirected(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca;
  int validUris;


  ca = ph_locate_call(je, 1);
  
  if (!ca)
    return;

  validUris = nonempty(je->remote_contact) && nonempty(je->remote_uri);

  if ((ca->redirs > 8) || !validUris || ph_find_matching_vline(je->remote_contact, PHM_IGNORE_PORT) || ph_same_uri(je->remote_contact, je->remote_uri))
    {
      ph_call_requestfailure(je);
      return;
    }
 

  clear(info);

  
  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  info.event = phCALLREDIRECTED;
  info.u.remoteUri = je->remote_contact;
  info.vlid = ca->vlid;
  info.newcid = 0;
  
  if (phcfg.autoredir)
    {
      phcall_t *newca;

      newca = ph_allocate_call(-2);
      newca->redirs = ca->redirs + 1;
      info.newcid = phLinePlaceCall_withCa(info.vlid, je->remote_contact, 0, 0, ca->user_mflags, newca);
    }

  ph_release_call(ca);

  phcb->callProgress(je->cid, &info);
}


void ph_callStopRinging(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca;
  
  clear(info);
  
  ca = ph_locate_call(je, 1);
  if (ca->isringing)
    {
      ca->isringing = 0;
      info.event = phRINGandSTOP;
      
      info.localUri = je->local_uri;
      info.userData = je->external_reference;
      
      info.u.remoteUri = je->remote_uri; 
      info.vlid = ca->vlid;
      
      phcb->callProgress(je->cid, &info);
    }
}

void 
ph_call_ringing(eXosip_event_t *je)
{
    int ret = 0;
    phCallStateInfo_t info;
    phcall_t *ca, *rca=0;
    int cng=0;
    
    DBG4_SIP_NEGO("SIP NEGO: ph_call_ringing\n", 0, 0, 0);
    
    clear(info);
    
    ca = ph_locate_call(je, 1);
    if (ca)
    {
        rca = ph_locate_call_by_cid(ca->rcid);
    }
    
    ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);
    
    ret = ph_call_media_start(ca, je, -1, 0);
    
    info.event = phRINGING;
    if (ret == -PH_NOMEDIA && !ph_call_hasaudio(ca) && !ca->isringing) /*  no audio and softPhone is now not ringing and has no sound */
    {
        ca->isringing = 1;
        info.event = phRINGandSTART;
    }
    else if (ca->isringing )
    {
        ca->isringing = 0;
        info.event = phRINGandSTOP;
    }
    
    
    info.localUri = je->local_uri;
    info.userData = je->external_reference;
    
    info.u.remoteUri = je->remote_uri; 
    info.vlid = ca->vlid;
    
    info.streams = ca->nego_mflags;
    
    phcb->callProgress(je->cid, &info);
    
    if (rca)
    {
        ph_refer_notify(rca->rdid, 180, "Ringing", 0);
    }

}


static void 
ph_call_requestfailure(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;
  int i;

  clear(info);

  ca = ph_locate_call(je, 0);
  if (!ca)
    return;

  rca = ph_locate_call_by_cid(ca->rcid);
  info.vlid = ca->vlid;
  ph_release_call(ca);


  
  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  if (je->status_code == 486)
    {
      info.event = phCALLBUSY;
      info.u.remoteUri = je->remote_uri;
    }
  else
    {
      info.event = phCALLERROR;
      info.u.errorCode = je->status_code;
    }

  phcb->callProgress(je->cid, &info);
  if (rca)
    {
    ph_refer_notify(rca->rdid, je->status_code, je->status_code == 486 ? "Busy" : "Request failure", 1);
    }

}


void 
ph_call_serverfailure(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;

  clear(info);

  ca = ph_locate_call(je, 0);
  if (ca)
    {
    rca = ph_locate_call_by_cid(ca->rcid);
    info.vlid = ca->vlid;
    ph_release_call(ca);
    }

  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  info.event = phCALLERROR;
  info.u.errorCode = je->status_code;


  phcb->callProgress(je->cid, &info);

  if (rca)
    {
    ph_refer_notify(rca->rdid, je->status_code, "Server failure", 1);
    }


}

void 
ph_call_globalfailure(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;

  clear(info);

  ca = ph_locate_call(je, 0);
  if (ca)
    {
    rca = ph_locate_call_by_cid(ca->rcid);
    info.vlid = ca->vlid;
    ph_release_call(ca);
    }



  info.userData = je->external_reference;
  info.localUri = je->local_uri;

  if (je->status_code == 600)
    {
      info.event = phCALLBUSY;
      info.u.remoteUri = je->remote_uri;
    }
  else
    {
      info.event = phCALLERROR;
      info.u.errorCode = je->status_code;
    }

  phcb->callProgress(je->cid, &info);

  if (rca)
    {
    ph_refer_notify(rca->rdid, je->status_code, "Global failure", 1);
    }



}

void 
ph_call_noanswer(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;
  
  clear(info);

  ca = ph_locate_call(je, 1);
  if (ca)
    {
    rca = ph_locate_call_by_cid(ca->rcid);
    info.vlid = ca->vlid;
    ph_release_call(ca);
    }

  info.userData = je->external_reference;
  info.event = phNOANSWER;
  info.u.remoteUri = je->remote_uri;
  info.localUri = je->local_uri;

  phcb->callProgress(je->cid, &info);


  if (rca)
    {
    ph_refer_notify(rca->rdid, je->status_code, "No answer", 1);
    }


}



void 
ph_call_closed(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;

  clear(info);

  ca = ph_locate_call(je, 0);
  if (ca)
    {
    rca = ph_locate_call_by_cid(ca->rcid);
    info.vlid = ca->vlid;
    ph_release_call(ca);

    info.userData = je->external_reference;
    info.event = phCALLCLOSED;
    info.u.errorCode = 0;
    phcb->callProgress(je->cid, &info);
    }

  if (rca)
    {
    ph_refer_notify(rca->rdid, je->status_code, "Closed", 1);
    }


}


void 
ph_call_onhold(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca;

  clear(info);

  ca = ph_locate_call(je, 0);
  
  if (!ca)
    return;

  info.vlid = ca->vlid;
  if (ph_call_hasaudio(ca))
    {    
#ifndef MEDIA_SUSPEND      
    ph_call_media_stop(ca);
#else
    ph_call_media_suspend(ca, 0);
#endif
    }

  ca->remotehold = 1;
  info.userData = je->external_reference;
  info.event = phCALLHELD;
  phcb->callProgress(je->cid, &info);

  


}

void 
ph_call_offhold(eXosip_event_t *je)
{
    phCallStateInfo_t info;
    phcall_t *ca;
    int cng=0;
    int remhold;
    
    DBG4_SIP_NEGO("SIP NEGO: ph_call_offhold\n", 0, 0, 0);
    
    ca = ph_locate_call(je, 0);
    if (!ca)
    {
        return;
    }
    
    clear(info);
    
    info.vlid = ca->vlid;
    // ph_media_resume(ca);
    
    //  if (ph_call_hasaudio(ca))
    //      ph_media_stop(ca);
    
    remhold = ca->remotehold;
    ca->remotehold = 0;
    
    ph_call_retrieve_payloads(ca, je, -1);
    ph_call_media_start(ca, je, -1, remhold);
    
    if (remhold)
    {
        info.userData = je->external_reference;
        info.event = phCALLRESUMED;
        info.streams = ca->nego_mflags;
        
        phcb->callProgress(je->cid, &info);
    }
    
    ca->remotehold = 0;

}


void ph_reg_progress(eXosip_event_t *je)
{
  int i;
  struct vline *vl = 0;
  int vlid;
  int mask = 0;

  vl =  ph_find_vline_by_rid(je->rid);
  
  if (!vl)
    return;

  if (vl->regTimeout == 0)
    mask = PH_UNREG_MASK;
  vlid = ph_vline2vlid(vl);



  DBG4_SIP_NEGO("REGPROGRESS reg=%d for vlid=%d\n", je->rid, vlid,0);

  if (je->type == EXOSIP_REGISTRATION_SUCCESS)
    {
      if (je->expires)
	vl->regTimeout = je->expires;
      phcb->regProgress(vlid, 0 | mask);
      if (vl->used == VL_DELETING)
	vline_free(vl);
    }    
  else if (je->type == EXOSIP_REGISTRATION_FAILURE)
    {
      int newtimeout = -1;

      if (je->status_code == 423)  /* Interval to brief */
	{
	  if (je->minexpires)
	    {
	      vl->regTimeout = newtimeout = je->minexpires;
	    }
	}

      if (je->status_code == 401 || je->status_code == 407 || (newtimeout > 0))
	{
	  eXosip_lock();
	  i = eXosip_register(je->rid, newtimeout);
	  eXosip_unlock();
	  
	  SKIP(printf("Retrying reg=%d for vlid=%d i=%d t=%d\n", je->rid, vlid, i, newtimeout))
	  if (i == 0) return;
	}
      phcb->regProgress(vlid, mask | (je->status_code ? je->status_code : 500) );

    if (vl->used == VL_DELETING)
      vline_free(vl);

    }
    
}



void ph_notify_handler(eXosip_event_t *je)
{
  if (phcb->onNotify) 
    {
      phcb->onNotify(je->sip_event, je->remote_uri, je->msg_body);
    }
}


void ph_call_refered(eXosip_event_t *je)
{
  phcall_t *ca;
  phCallStateInfo_t info;
  int nCid;
  struct vline *vl = 0;

  ca = ph_locate_call_by_cid(je->cid);
  
  if (ca)
    vl = ph_valid_vlid(ca->vlid);


  /* 
     we're rejecting the requests refering unxisting calls, vlines and
     if we detect URI loop
  */
  if (!ca || !vl || ph_find_matching_vline(je->refer_to, 0))
    {
      eXosip_lock();
      eXosip_answer_refer(je->did, (!ca) ? 481 : 488);
      eXosip_unlock();
      return;
    }


  eXosip_lock();
  eXosip_answer_refer(je->did, 202);
  eXosip_unlock();

  clear(info);
  ca->rdid = je->did;

  ph_call_media_stop(ca);


  info.newcid = phLinePlaceCall2(ca->vlid, je->refer_to,  0, je->cid, ca->user_mflags);
  

  info.event = phXFERREQ;
  info.u.remoteUri = je->refer_to;
  info.vlid = ca->vlid;
  
  phcb->callProgress(je->cid, &info);

}


void ph_call_refer_status(eXosip_event_t *je)
{
  phcall_t *ca;
  phCallStateInfo_t info;
  int cheat = 0;
  const char *resultstr;
  int status = 0;
  int txcid;

  ca = ph_locate_call_by_cid(je->cid);
  
  if (!ca)
    return;

  clear(info);

  if (je->type == EXOSIP_CALL_REFER_STATUS)
    {
      if (phDebugLevel)
	ph_printf("refer_status sdp=%s\n", je->msg_body);

      if (je->ss_status == EXOSIP_SUBCRSTATE_TERMINATED)
	{
	  /* cheat and suppose that the transfer succeeded */
	  cheat  = 200;
	}
      
      resultstr = strchr(je->msg_body, ' ');

      if (resultstr)
	status = atoi(resultstr);


    }
  else
    status = je->status_code;
    

  if (!status)
    {
      if (!cheat)
	return;

      status = cheat;
    }

  if ((status < 200) && cheat)
    status = cheat;

  info.u.errorCode = status;
  info.vlid = ca->vlid;


  if (!ca->txcid && status == 180)  
    {
      /* blind transfer:  RINGING is good enough for us */ 
      info.event = phXFEROK;
    }
  else if (status < 200 && status >= 100)
    {
      info.event = phXFERPROGRESS;
    }
  else if (status >= 200 && status < 300)
    {
      info.event = phXFEROK;
    }
  else
    info.event = phXFERFAIL;


  txcid = ca->txcid;

  phcb->callProgress(je->cid, &info);

  if (info.event == phXFEROK || info.event == phXFERFAIL)
    {
      if (txcid > 0)  /* assisted transfer, close both calls  */
	  phCloseCall(txcid);
      if (ca->cid > 0)  /* assisted or blind transfer, close call  */
	  phCloseCall(ca->cid);
    }
}


void ph_message_progress(eXosip_event_t *je)
{
  phMsgStateInfo_t info;
  
  memset(&info, 0, sizeof(info));
  
  if (je->type == EXOSIP_MESSAGE_NEW) 
    {
      info.event = phMsgNew;
      info.content = je->msg_body;
      info.ctype = je->i_ctt->type;
      info.subtype = je->i_ctt->subtype;
      info.to = je->local_uri;
      info.from = je->remote_uri;
      if (phcb->msgProgress != NULL)
	phcb->msgProgress(0, &info);
    }
  else if (je->type == EXOSIP_MESSAGE_SUCCESS)
    {
      info.event = phMsgOk;
      info.to = je->local_uri;
      info.from = je->remote_uri;
      if (phcb->msgProgress != NULL)
	phcb->msgProgress(je->mid, &info);
    }
  else if (je->type == EXOSIP_MESSAGE_FAILURE)
    {
      info.to = je->local_uri;
      info.from = je->remote_uri;
      info.event = phMsgError;
      if (phcb->msgProgress != NULL)
	phcb->msgProgress(je->mid, &info);
    }
}


void ph_subscription_progress(eXosip_event_t *je)
{
  phSubscriptionStateInfo_t info;

  memset(&info, 0, sizeof(info));
  
  if (je->type == EXOSIP_SUBSCRIPTION_ANSWERED) 
    {
      info.event = phSubscriptionOk;
      info.from = je->local_uri;		
      info.to = je->remote_uri;
      
      if (phcb->subscriptionProgress != NULL)
	phcb->subscriptionProgress (je->sid, &info);
    }
  else if (je->type == EXOSIP_SUBSCRIPTION_REQUESTFAILURE)
    {
      info.event = phSubscriptionError;
      if (je->status_code == 404) {
	info.event = phSubscriptionErrNotFound;
      }		
      info.from = je->local_uri;
      info.to = je->remote_uri;
      if (phcb->subscriptionProgress != NULL)
	phcb->subscriptionProgress (je->sid, &info);
    }
}

static int
ph_event_get()
{
  int counter =0;
  /* use events to print some info */
  eXosip_event_t *je;

  //  phReleaseTerminatedCalls();
  for (;;)
    {
      je = eXosip_event_wait(0,timeout);
      if (je==NULL)
	break;
      counter++;

      if (phDebugLevel > 0)
	ph_printf("\n<- %s (%i %i) [%i %s] %s requri=%s\n",
		   evtnames[je->type], je->cid, je->did, 
		   je->status_code,
		   je->reason_phrase,
		   je->remote_uri,
		   je->req_uri);

      switch(je->type)
	{
	case EXOSIP_CALL_NEW:
	  ph_call_new(je);
	  break;
	
	case EXOSIP_CALL_ANSWERED:
		ph_callStopRinging(je);
	  ph_call_answered(je);
	  break;

	case EXOSIP_CALL_PROCEEDING:
	  ph_call_proceeding(je);
	  break;

	case EXOSIP_CALL_RINGING:
	  ph_call_ringing(je);
	  break;

	case EXOSIP_CALL_REDIRECTED:
	  ph_call_redirected(je);
	  break;

	case EXOSIP_CALL_REPLACES:
	  ph_call_replaces(je);
	  break;

	case EXOSIP_CALL_REQUESTFAILURE:
	  ph_call_requestfailure(je);
	  break;


	case EXOSIP_CALL_SERVERFAILURE:
	  ph_call_serverfailure(je);
	  break;

	case EXOSIP_CALL_GLOBALFAILURE:
	  ph_call_globalfailure(je);
	  break;

	case EXOSIP_CALL_NOANSWER:
	  ph_call_noanswer(je);
	  break;

	case EXOSIP_CALL_CLOSED:
	  ph_call_closed(je);
	  break;

	case EXOSIP_CALL_HOLD:
	  ph_call_onhold(je);
	  break;

	case EXOSIP_CALL_OFFHOLD:
	  ph_call_offhold(je);
	  break;

	case EXOSIP_REGISTRATION_SUCCESS:
	  ph_reg_progress(je);
	  break;

	case EXOSIP_REGISTRATION_FAILURE:
	  ph_reg_progress(je);
	  break;

	case EXOSIP_CALL_REFERED:
	  ph_call_refered(je);
	  break;

        case EXOSIP_CALL_REFER_STATUS:
        case EXOSIP_CALL_REFER_FAILURE:
	  ph_call_refer_status(je);
	  break;


	case EXOSIP_MESSAGE_NEW:
	case EXOSIP_MESSAGE_SUCCESS:        /* announce a 200ok to a previous sent */
	case EXOSIP_MESSAGE_FAILURE:
		ph_message_progress(je);
		break;

	case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
	case EXOSIP_SUBSCRIPTION_ANSWERED:
		ph_subscription_progress(je);
		break;

	case EXOSIP_SUBSCRIPTION_NOTIFY:
		ph_notify_handler(je);
		break;


	default:
      if (phDebugLevel > 0)
	  ph_printf("event(%i %i %i %i) text=%s\n", je->cid, je->sid, je->nid, je->did, je->textinfo);
	  break;
	}
	
      eXosip_event_free(je);
    }


    ph_refresh_vlines();
    ph_scan_calls();

  if (counter>0)
    return 0;
  return -1;
}


static void
ph_keep_refreshing()
{
  time_t now;
  static time_t last_refresh;
  
  time(&now);
  if (now - last_refresh > PH_REFRESH_INTERVAL)
    {
      phRefresh();
      last_refresh = now;
    }
}



/** 
 * eXosip event reader thread
 */
void *
ph_api_thread(void *arg)
{
  time_t t1,t2;
  t1 = 0;


  phIsInitialized = 1;
    
  time(&t1);
  while(1) 
    {
#ifdef WIN32
      Sleep(100);
#endif		

      if (!phIsInitialized)
	return 0;
      
      ph_keep_refreshing();
      ph_event_get();
    }
  return 0;
}

#ifdef QOS_DEBUG_ENABLE
MY_DLLEXPORT void phrtcp_QoS_enable_rtcp_report(int ToF)
{
	ortcp_enable_rtcp_report(ToF);
}
MY_DLLEXPORT void phrtcp_report_set_cb(jt_rtcpCallbacks_t *cbk)
{
	ortcp_report_set_cb(cbk);
}
MY_DLLEXPORT int phrtcp_report_begin()
{
	phrtcp_QoS_enable_rtcp_report(1);
	ortcp_report_begin();
}
MY_DLLEXPORT int phrtcp_report_end()
{
	ortcp_report_end();
}
#endif /* end of QOS_DEBUG_ENABLE */


int getPublicPort(char *  local_voice_port, char * local_video_port, char * public_voice_port, char * public_video_port)
{
	
  Socket  sock;
  Socket  sock1;
  Socket  sock2;
  Socket  sock3;
  StunAddress4 mappedAddr;
  StunAddress4 stunServerAddr;
  int res;
  int res2;
  int loop = 0;

  if (phcfg.use_tunnel)
    return 1;

  stunParseServerName(phcfg.stunserver, &stunServerAddr);
		
  do {

    printf("get pub video: %s", local_voice_port);
    res = stunOpenSocketPair(&stunServerAddr, &mappedAddr,&sock, &sock1,atoi(local_voice_port), NULL, 1);

    if (-1 !=  res)
      {
	snprintf(local_voice_port, 9, "%i", res);
	snprintf(public_voice_port, 9, "%d", mappedAddr.port);
	stunCloseSocket(sock1);
	stunCloseSocket(sock);
      } 
    else
      {
	stunCloseSocket(sock);
	stunCloseSocket(sock1);
	snprintf(local_voice_port, 9, "%i",atoi(local_voice_port)+5);
	loop++; 
      }
  } while (res <0 && loop < 4 );

  if (res <0)
    {
      return -1;
    }

  loop = 0;
  
  do {
    printf("get pub video: %s", local_video_port);
    
    res2 = stunOpenSocketPair(&stunServerAddr, &mappedAddr,&sock2, &sock3,atoi(local_video_port), NULL, 1);
    //sock2 = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(&local_video_port), NULL, 0); 
    
    if (-1 !=  res2)
      {
	snprintf(local_video_port, 9, "%i", res2);
	snprintf(public_video_port, 9, "%d", mappedAddr.port);
	stunCloseSocket(sock2);
	stunCloseSocket(sock3);
      }
    else
      {
	stunCloseSocket(sock2);
	stunCloseSocket(sock3);
	snprintf(local_video_port, 9, "%i",atoi(local_video_port)+5);
	
	loop++; 
      }
  } while(res2 <0 && loop < 4 );
  
  if (res2 <0)
    {
      return -1;
    }
		
  printf("get public port public voice_port: %s video: %s  ------ \n \n ",public_voice_port, public_video_port);
  return 1;
			

}

int phLineGetSipAddress(int vlid, char buf[], int bufsize) {
	int i;
	struct vline *vl;
	
	vl = ph_valid_vlid(vlid);
	
	if (!vl) {
		return PH_BADVLID;
	}
	
	ph_build_from(buf, bufsize, vl);
	return 0;
}

int phCrash() {
    *(int *)0 = 0;
    
}

void phSetDebugLevel(int level) {
    phDebugLevel = level;
}

