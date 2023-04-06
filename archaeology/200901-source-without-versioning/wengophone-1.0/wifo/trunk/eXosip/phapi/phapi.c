/*
 * phapi   phone api
 *
 * Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
 * Parts Copyright (C) 2002,2003   Aymeric Moizard <jack@atosc.org>
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


#ifdef WIN32
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

#ifndef WIN32
#include "config.h"
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#ifndef __APPLE__
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#else
#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp stricmp
#define usleep(usecs) Sleep((usecs)/1000)
#endif

#include <osip2/osip_mt.h>

#include <eXosip/eXosip.h>
#include <eXosip/eXosip_cfg.h>

#include "phapi.h"
#include "phcall.h"
#include "phrpc.h"
#include "phmedia.h"
#include "stun/stun.h"



#ifdef USE_HTTP_TUNNEL
#include "httptunnel.h"

#ifdef WIN32
#include "get_net_conf.h"
#endif
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

void ph_wegot_dtmf(phcall_t *ca, int dtmfEvent);


static char *ph_get_call_contact(phcall_t *ca);


#ifndef PH_STREAM_AUDIO
#define PH_STREAM_AUDIO (1 << 0)
#define PH_STREAM_VIDEO_RX (1 << 1)
#define PH_STREAM_VIDEO_TX (1 << 2)
#endif

#define PH_STREAM_CNG (1 << 30)


static int ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags);
static int ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int flags);



void ph_release_call(phcall_t *ca);
static  int timeout = 500;

static char ph_nat_router_addr[32];
static char ph_nat_type_str[32];
static char ph_nat_port_str[16];
static char ph_nat_video_port_str[16];

static NatType ph_nat_type = StunTypeOpen;

static char * _get_local_sdp_port();
static char * _get_local_video_sdp_port();
static char * _get_public_sdp_port();
static char * _get_public_video_sdp_port();



void ph_message_progress(eXosip_event_t *je);
static void ph_keep_refreshing();


#define nonempty(x)  ((x) && (x)[0])


#define USE_VLINES 1
#define PH_MAX_VLINES 16

#if USE_VLINES 
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

#endif







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
#define FORCE_VAD   1
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
			 /* vad */ 0,0,0,
			 0,0,0,
		     /* stunserver */ "80.118.132.74",
			 0
};

#endif

int _is_video_enabled(int streams) {
 return (streams & (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX));
}
int _is_audio_enabled(int streams) {
 return (streams & PH_STREAM_AUDIO);
}

static char *
_get_local_sdp_port()
{

/* <MINHPQ>
 * To get arround the problem of connect in winsock ( connect returns 
 * an error: 10048: Address already in use connect is called an the local address is in used
 * within the last 2 or 4 minutes), we should not bind the rtp socket to a specific local port. Hence, 
 * we should return "0" here to let the system choose a random port number.
 */
	if (phcfg.use_tunnel)
		return "10600";

	return phcfg.local_rtp_port;
}

static char *
_get_local_video_sdp_port() 
{

	if (phcfg.use_tunnel)
		return "10602";

	return phcfg.local_video_rtp_port;
}

static char *
_get_public_sdp_port()
{
  if (ph_nat_port_str[0])
    return ph_nat_port_str;

  return _get_local_sdp_port();
  
}


static char *
_get_public_video_sdp_port()
{
  if (ph_nat_video_port_str[0])
    return ph_nat_video_port_str;

  return _get_local_video_sdp_port();
  
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

void phReleaseTerminatedCalls()
{
	phcall_t *ca;
	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
    {
		if ((ca->cid != -1) && (ph_media_is_stream_stopped(ca) == 1)) {
			//ph_release_call(ca);
			phCloseCall(ca->cid);
		}
    }
}

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
	if (ca->hasaudio)
	{
		ph_media_stop(ca);
		memset(ca, 0, sizeof(phcall_t));
		ca->cid = -1;
	}

}





void ph_stream_ended(phcall_t *ca, int event)
{
	ca->closereq = 1;
}

void ph_wegot_dtmf(phcall_t *ca, int dtmfEvent)
{
    phCallStateInfo_t info;

    clear(info);

    info.event = phDTMF;
    info.u.dtmfDigit = dtmfEvent;
    info.vlid = ca->vlid;
    phcb->callProgress(ca->cid, &info);

}


//#define stringize(x) #x
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



MY_DLLEXPORT int 
phPlaceCall2(const char *from, const char *uri, void *userdata, int rcid)
{
	return phPlaceCall3(from, uri, userdata, rcid, PH_STREAM_AUDIO);
}

	
MY_DLLEXPORT int 
phPlaceCall3(const char *from, const char *uri, void *userdata, int rcid, int streams)
{
  int i;
  osip_message_t *invite;
  phcall_t *ca;
  char *proxy = phcfg.proxy;
  char *vport = 0;
#if USE_VLINES
  struct vline *vl;
  
  vl = ph_find_matching_vline(from, PHM_IGNORE_PORT);

  if (!vl)
    return -PH_BADID;

  proxy = vl->proxy;

  if (rcid)
    {
      ca = ph_locate_call_by_cid(rcid);
      if (!ca)
	return -PH_BADCID;
    }


#endif


  i = eXosip_build_initial_invite(&invite,
				  uri,
				  from,
				  proxy,
				  "");
  if (i!=0)
      return -1;

    if ( _is_video_enabled(streams) )
	vport = _get_local_video_sdp_port();

  
  eXosip_lock();
  i = eXosip_initiate_call(invite, userdata, NULL, _get_local_sdp_port(), vport);
  ca = ph_allocate_call(i);
  if (i == -1)
    { 
      i = -PH_ERROR; goto err;
    }

  if (rcid)
    ca->rcid = rcid;
#if USE_VLINES
  ca->vlid = ph_vline2vlid(vl);
  ph_build_cname(ca->cname, sizeof(ca->cname), vl); 
#endif

err:
  eXosip_unlock();  
  return i;

}

#if USE_VLINES

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
phLinePlaceCall(int vlid, const char *uri, void *userdata, int rcid)
{
   return phLinePlaceCall2(vlid, uri, userdata, rcid, PH_STREAM_AUDIO);
}

 
MY_DLLEXPORT int 
phLinePlaceCall2(int vlid, const char *uri, void *userdata, int rcid, int streams)
{
  int i;
  osip_message_t *invite;
  phcall_t *ca;
  char *proxy = phcfg.proxy;
  struct vline *vl;
  char *vport = 0;
  char from[512];

	ph_printf("phLinePlaceCall2: a new call has been placed\n");

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

	if (_is_video_enabled(streams)) {
		vport = _get_public_video_sdp_port();
	}

  eXosip_lock();

  i = eXosip_initiate_call(invite, userdata, NULL, _get_public_sdp_port(),  vport);
  ca = ph_allocate_call(i);
  ca->user_mflags = streams;
  ca->nego_mflags = ca->user_mflags;
  if (rcid)
    ca->rcid = rcid;
#if USE_VLINES
  ca->vlid = ph_vline2vlid(vl);
#endif

  eXosip_unlock();  
  return i;

}

#endif




MY_DLLEXPORT int
phPlaceCall(const char *from, const char *uri, void *userdata)
{
  return phPlaceCall2(from, uri, userdata, 0);
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
phPublish(const char *from, const char *to, const int winfo, const char * content_type, const char * content)
{
  int i;

  if (!nonempty(to) || !nonempty(from) || !nonempty(content_type) || !nonempty(content))
    return -PH_BADARG;


  i = eXosip_publish(to, from, ph_get_proxy(from), winfo, content_type,content);
	eXosip_unlock();  
	return i;
}

MY_DLLEXPORT int 
phPing(char* host, int port, int ttl)
{
  if (!phIsInitialized)
    return -1;

  if (!host)
    return -PH_BADARG;

	return eXosip_ping(host,port,ttl);
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
  int i,v,cng=0;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  char *vport = 0;
	
  if (!ca)
    return -PH_BADCID;

  ca->user_mflags = streams; // trace of what the user decided
  ca->nego_mflags = ca->user_mflags; // current negociated media flags
  
  if (_is_video_enabled(streams))
    vport = _get_public_video_sdp_port();

  eXosip_lock();
  i = eXosip_answer_call(ca->did, 200, _get_public_sdp_port(), ph_get_call_contact(ca), vport);
  if (i == 0)
  {
    i = ph_call_retrieve_payloads(ca, NULL, streams | PH_STREAM_CNG );
  }
  eXosip_unlock();

  if (i == 0)
  {
    i = ph_call_media_start(ca, NULL, streams);
  }

  return i;
}

  


MY_DLLEXPORT int 
phRejectCall(int cid, int reason)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  
  if (!ca)
    return -PH_BADCID;

  eXosip_lock();
  i = eXosip_answer_call(ca->did, reason, 0, ph_get_call_contact(ca), 0);
  eXosip_unlock();

  return i;

}


MY_DLLEXPORT int 
phRingingCall(int cid)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  
  if (!ca)
    return -PH_BADCID;

  eXosip_lock();
  i = eXosip_answer_call(ca->did, 180, 0, ph_get_call_contact(ca), 0);
  eXosip_unlock();

  return i;

}



MY_DLLEXPORT int 
phCloseCall(int cid)
{
  int i;
  phcall_t *ca = ph_locate_call_by_cid(cid);
  phCallStateInfo_t info;
  
  clear(info);
  if (!ca)
    return -PH_BADCID;


  eXosip_lock();
  i = eXosip_terminate_call(ca->cid, ca->did);
  eXosip_unlock();

  info.vlid = ca->vlid;
  ph_release_call(ca);

  if (i)
    return i;

  info.userData = 0;
  info.event = phCALLCLOSED;
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


MY_DLLEXPORT int 
phResumeCall(int cid)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);
  int i;


  if (!ca)
    return -PH_BADCID;

  if (!ca->localhold)
    return -PH_HOLDERR;



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

  if (!i && ca->hasaudio)
    ph_media_stop(ca);



  return i;
}


MY_DLLEXPORT int 
phSetFollowMe(const char *uri)
{
  if (!uri)
    ph_follow_me_addr[0] = 0;

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

MY_DLLEXPORT int 
phRegister2(const char *username, const char *server, int timeout)
{
  int rid;
  int ret = -1;
  char tmp[256];
  char utmp[256];

  
  if (!username)
    return -PH_BADARG;

  if (!server)
    return -PH_BADARG;
 

  if (0 != strncasecmp(username, "sip:", 4)) 
    {
      snprintf(utmp, sizeof(utmp), "sip:%s@%s", username,server);
      username = utmp;
    }

#if USE_VLINES
  rid = ph_get_vline_id(username, 0);
  if (rid)
    {
      struct vline *vl = ph_vlid2vline(rid);

      vl->regTimeout = timeout;
	  if (vl->server)
		return phvlRegister(rid); 
    }

  {
    osip_uri_t *uri;

    osip_uri_init(&uri);
    osip_uri_parse(uri, username);

    rid = phAddVline(uri->username, server, phcfg.proxy, timeout); 

    osip_uri_free(uri);


    return rid;
  }
#endif

  if (0 != strncasecmp(server, "sip:", 4)) 
    {
      snprintf(tmp, sizeof(tmp), "sip:%s", server);
      server = tmp;
    }



  eXosip_lock();

  rid = eXosip_register_init((char *) username, (char *) server, 0, ph_get_proxy(username));

  if (rid >= 0)
    {
      ret = eXosip_register(rid, timeout);

      if (ret == 0)
	ret = rid;
    }

  eXosip_unlock();

  return ret;

}



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

  if (!ca->hasaudio)
    return -PH_NOMEDIA;


  return ph_media_send_dtmf(ca, dtmfEvent, mode);

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

  if (!ca->hasaudio)
    return -PH_NOMEDIA;


  return ph_media_send_sound_file(ca, fileName);

  
}




MY_DLLEXPORT int 
phStopSoundFile()
{
  return -1;
}


MY_DLLEXPORT int 
phSetSpeakerVolume(int cid,  int volume)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  return(ph_media_set_spkvol(ca, volume));
}


MY_DLLEXPORT int 
phSetRecLevel(int cid,  int level)
{
  phcall_t *ca = ph_locate_call_by_cid(cid);

  if (!ca)
    return -PH_BADCID;

  return(ph_media_set_recvol(ca, level));
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
#if USE_VLINES
  struct vline *vl;
  int oldTimeout = 0;
  char srvbuf[256];
  char *srv2;
  int port;


  if (phDebugLevel)
    ph_printf("AddVline2(dn = %s, un=%s, srv=%s pxy=%s regT=%d)\n", nonull(displayname), nonull(username), nonull(server), nonull(proxy), regTimeout);

  srv2 = ph_scrap_port(srvbuf, sizeof(srvbuf), server, &port);

  if (!port)
    port = 5060;

  if (!username)
    username = "";

  
  vl  = ph_find_matching_vline3(username, srv2, port, 0);
  
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
#else
  return -1;
#endif

}


MY_DLLEXPORT int 
phDelVline(int vlid)
{
#if USE_VLINES
  struct vline *vl;

  if (!(vl = ph_valid_vlid(vlid)))
      return -PH_BADVLID;

  if (vl->regTimeout)
    {
      
      eXosip_lock();
      eXosip_register(vl->rid, 0);
      eXosip_unlock();

      vl->regTimeout = 0;
      vl->used = VL_DELETING;
    }
  
  if (vl->used != VL_DELETING)
    vline_free(vl);
  
  return 0;
#else
  return -1;
#endif
}



MY_DLLEXPORT int phChangeAudioDevices(const char *devstr)
{
  if (devstr)
    strncpy(phcfg.audio_dev, devstr, sizeof(phcfg.audio_dev));

  return 0;
}


#if USE_VLINES

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

  assert(username);
  
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

//  if (!phcfg.use_tunnel && phcfg.nat_refresh_time > 0)
    if (phcfg.nat_refresh_time > 0)
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
  
#endif

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
	  
	  ph_release_call(ca);
	  
	  memset(&info, 0, sizeof(info));
	  info.vlid = ca->vlid;
	  info.event = phCALLCLOSED;
	  phcb->callProgress(cid, &info);
	}
    }

}

static char *
ph_get_proxy(const char *userid)
{
#if USE_VLINES
  struct vline *vl = ph_find_matching_vline(userid, PHM_IGNORE_PORT);

  if (!vl)
    return "";

  return vl->proxy ? vl->proxy : "";
#else
  return phcfg.proxy;
#endif
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
#if USE_VLINES
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


#else
  return 0;
#endif

}



static void setup_payload(const char *ptsring)
{
  char  tmp[64];
  char  num[8];
  ph_media_payload_t  pt;

  if (ph_media_supported_payload(&pt, ptsring))
    {
      snprintf(num, sizeof(num), "%d", pt.number);
      snprintf(tmp, sizeof(tmp), "%d %s/%d/1", pt.number, pt.string, pt.rate);

      eXosip_sdp_negotiation_add_codec(osip_strdup(num),
				   NULL,
				   osip_strdup("RTP/AVP"),
				   NULL, NULL, NULL,
				   NULL,NULL,
				   osip_strdup(tmp));
    }
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
    } else {
	    DBG("@#^\@!\n");
    }
}




#if 0
int phTunnelConfig(const char* local_http_proxy_ip, const int local_http_proxy_port,
								const char* http_proxy_ip, const int http_proxy_port)
{
	phcfg.http_proxy_ip[0] = 0;
	phcfg.local_http_proxy_ip[0] = 0;

	if (http_proxy_ip)
		strncpy(phcfg.http_proxy_ip, http_proxy_ip, sizeof(phcfg.http_proxy_ip));
	if (local_http_proxy_ip)
		strncpy(phcfg.local_http_proxy_ip, local_http_proxy_ip, sizeof(phcfg.local_http_proxy_ip));
	phcfg.http_proxy_port = http_proxy_port;
	phcfg.local_http_proxy_port = local_http_proxy_port;

}
#endif

/**
 * Initialize the phoneapi module
 */
static eXosip_tunnel_t *phTunnel;

MY_DLLEXPORT int 
phInit(phCallbacks_t *cbk, char * server, int asyncmode)
{
  int i, port;
  eXosip_tunnel_t *tunnel = NULL;
  char buf[200];
  char *c;
  char *ntstr = 0;
  char *dbgstr;

#ifdef PHAPI_VIDEO_SUPPORT
  avcodec_init();
  avcodec_register_all();
  phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_128KBPS;
#endif

#if 0    
  phcfg.noaec = 1;
  strcpy(phcfg.audio_codecs, "PCMU");  
#endif

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



  for( i = 0; i < PH_MAX_CALLS; i++)
    ph_calls[i].cid = -1;

#ifdef USE_HTTP_TUNNEL  
  if (phcfg.use_tunnel) 
    {
	http_tunnel_init_host(phcfg.httpt_server, phcfg.httpt_server_port);
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
		tunnel->h_tunnel = http_tunnel_open(buf, port, HTTP_TUNNEL_VAR_MODE);
		if (!tunnel->h_tunnel)
		  {
#ifdef WIN32
			  if (phcfg.use_tunnel & PH_TUNNEL_AUTOCONF)
				{
				net_info_t netInfo;
				GetNetConf(&netInfo, 0);
				http_tunnel_init_proxy(netInfo.Proxy_IP, netInfo.Proxy_Port, 
					phcfg.http_proxy_user, phcfg.http_proxy_passwd);

				tunnel->h_tunnel = http_tunnel_open(buf, port, HTTP_TUNNEL_VAR_MODE);
			
				}
#endif		
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
	}
	else 
	  {
	  return -PH_NORESOURCES;
	}
  }
#endif

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

  ph_media_init(phcfg.use_tunnel);

  ph_nat_router_addr[0] = 0;
  ph_nat_port_str[0] = 0;

  if (!phcfg.use_tunnel && phcfg.nattype[0])
    {
      if (!strncasecmp(phcfg.nattype, "auto", 4))
	{
         NatType ntype;
	 StunAddress4 stunServerAddr;
	 int resPort = 0, hairpin = 0;
	 int  needMappedAddress = 0;

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

	     sock = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(_get_local_sdp_port()), NULL, 0); 

	     if (-1 !=  (int) sock)
	       {
		 ipv4tostr(ph_nat_router_addr, mappedAddr);
		 snprintf(ph_nat_port_str, sizeof(ph_nat_port_str), "%d", mappedAddr.port);
		 stunCloseSocket(sock);
	       }


	     sock = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(_get_local_video_sdp_port()), NULL, 0); 

	     if (-1 !=  (int) sock)
	       {
		 snprintf(ph_nat_video_port_str, sizeof(ph_nat_video_port_str), "%d", mappedAddr.port);
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
  else if (phcfg.use_tunnel)
  {
    ntstr = "open";
  }
  i = eXosip_init(0, 0, atoi(phcfg.sipport), tunnel);


  if (ntstr)
    {

      eXosip_set_nattype(ntstr);
      if (!phcfg.nat_refresh_time)
	phcfg.nat_refresh_time = 15;

      strncpy(ph_nat_type_str, ntstr, sizeof(ph_nat_type_str));
      if (ph_nat_router_addr[0])
	eXosip_set_mediaip(ph_nat_router_addr);

    }
  else
    {
	phcfg.nat_refresh_time = 0;
	strcpy(ph_nat_type_str, "open");
    }


   if (phDebugLevel > 0)
	ph_printf("NAT type: %s fw=%s port=%s  vport=%s\n", ph_nat_type_str, ph_nat_router_addr, ph_nat_port_str, ph_nat_video_port_str);



  if (phcfg.force_proxy)
    eXosip_force_proxy(phcfg.proxy);

  

  {
    char contact[512];
   
    eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
    eXosip_set_answer_contact(contact);
  }





#if USE_VLINES
  memset(ph_vlines, 0, sizeof(ph_vlines));
  if (!phcfg.nodefaultline && phcfg.identity[0])
    {
      osip_from_t *from;

      osip_from_init(&from);
      osip_from_parse(from, phcfg.identity);
      phAddVline2(from->displayname, from->url->username, 0, 0, 0);

      osip_from_free(from);
    }
#endif

  if (i!=0)
    return i;

  /* reset all payload to fit application capabilities */
  eXosip_sdp_negotiation_remove_audio_payloads();

 
#if 0
  setup_payload("G729/8000");
#endif	    

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

#ifdef G711_ONLY
  strcpy(phcfg.audio_codecs, "PCMU/8000,PCMA/8000");
#endif
  
  if (!phcfg.audio_codecs[0])
    {
      setup_payload("PCMU/8000");
      setup_payload("PCMA/8000");
      setup_payload("GSM/8000");
      setup_payload("ILBC/8000");
      setup_payload("AMR/8000");
      setup_payload("AMR-WB/16000");
    }
  else
    {
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
	  else
	    snprintf(tmp, sizeof(tmp), "%s/8000", tok);
	  
	  setup_payload(tmp);

	  DBG2("phapi: added codec: %s\n",tmp);

	  tok = strtok(0, ",");
	}
    }

  if(phcfg.cng)
    setup_payload("CN/8000");
  
  setup_payload("telephone-event/8000");

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

	/* *** JULIEN *** */
	if (proxy_user)
		strncpy(phcfg.http_proxy_user, proxy_user, sizeof(phcfg.http_proxy_user));
	if (proxy_passwd)
		strncpy(phcfg.http_proxy_passwd, proxy_passwd, sizeof(phcfg.http_proxy_passwd));
	/* ************** */
	

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
	
  if (!phIsInitialized)
	  return;

#if USE_VLINES

  for(i = 0; i < PH_MAX_VLINES; i++)
    {
      phDelVline(i+1);
    }

#endif


  for(i = 0; i < PH_MAX_CALLS; i++)
    if (ph_calls[i].cid != -1)
      ph_release_call(&ph_calls[i]);


    
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
		if (ph_event_get() == -2)
		{
			return -2;
		}
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

static int ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags)
{
   int  i = 0;

   if (_is_audio_enabled(flags))
     i = eXosip_retrieve_negotiated_audio_payload(ca->did, &ca->audio_payload, ca->audio_payload_name, sizeof(ca->audio_payload_name));

   ca->video_payload = 0;
   if (ca->remote_sdp_video_ip[0] && (_is_video_enabled(flags)))
    {
     i = eXosip_retrieve_negotiated_video_payload(ca->did, &ca->video_payload, ca->video_payload_name, sizeof(ca->video_payload_name));
     ph_printf("video_ip: %s payload=%d\n", ca->remote_sdp_video_ip, ca->video_payload); 
    }

  if(!i && phcfg.cng && (flags & PH_STREAM_CNG))
     ca->cng = !eXosip_retrieve_negotiated_specific_payload(ca->did, PH_MEDIA_CN_PT_STR, strlen(PH_MEDIA_CN_PT_STR));

  return i;
}


static int ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int flags)
{
	int i = 0;
	int port_audio, port_video;
	phFrameDisplayCbk  frameDisplay = 0;

	if (phcfg.nomedia || ca->localhold || ca->remotehold) {
		return 0;
	}

	port_audio = atoi(_get_local_sdp_port());
	port_video = port_audio + 2;
    
  if ((_is_video_enabled(ca->user_mflags)) && ca->video_payload && ca->remote_sdp_video_ip[0])
  {
	ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_RX;
	ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_TX;
    frameDisplay = phcb->onFrameReady;
    ph_printf("will have video stream ip: %s payload=%d\n", ca->remote_sdp_video_ip, ca->video_payload);
	ph_printf("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags); 
  } else {
	// video is not negociated
	ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_RX;
	ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_TX;
	ph_printf("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags); 
  }

#ifdef PHAPI_VIDEO_LOCAL_HACK
    frameDisplay = phcb->onFrameReady;
    ca->video_payload = 34;
#endif

	if ((_is_audio_enabled(ca->nego_mflags) || (_is_video_enabled(ca->nego_mflags))) && (!je || je->remote_sdp_audio_ip[0] || je->remote_sdp_video_ip[0]))
	{
		if (ph_media_start(ca, port_audio, port_video, 
				   ph_wegot_dtmf, 
				   ph_stream_ended, 
				   frameDisplay, 
				 phcfg.audio_dev, phcfg.vad, ca->cng, phcfg.jitterdepth, phcfg.noaec))
		{
			i = -PH_NOMEDIA;
		}
		
	}

  return i;
}


void 
ph_call_new(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca;

  clear(info);
  if (ph_busyFlag)
    {
      eXosip_lock();
      eXosip_answer_call(je->did, 486, 0, 0, 0);
      eXosip_unlock();
      return;
    }


  if (ph_follow_me_addr[0])
    {
      eXosip_lock();
      eXosip_answer_call(je->did, 302, 0, ph_follow_me_addr, 0);
      eXosip_unlock();
      return;
    }


  info.vlid = ph_get_vline_id(je->local_uri, je->req_uri);

  if (!info.vlid)
    {
      eXosip_lock();
      eXosip_answer_call(je->did, 404, 0, 0, 0);
      eXosip_unlock();
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
      eXosip_lock();
      eXosip_answer_call(je->did, 500, 0, 0, 0);
      eXosip_unlock();
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
	phcb->callProgress(oldca->cid, &info);

	if (oldca->hasaudio) 
	  ph_media_stop(oldca);

	phAcceptCall(je->cid);


      }
  }

}



void 
ph_call_answered(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;
  int cng=0;

  clear(info);


  ca = ph_locate_call(je, 1);
  if (ca)
    rca = ph_locate_call_by_cid(ca->rcid);

  ph_call_retrieve_payloads(ca, je, -1);
  ph_call_media_start(ca, je, -1);



  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  if (ca->localhold)
    info.event = phHOLDOK;
  else if (ca->localresume)
    {
      info.event = phRESUMEOK;
      ca->localresume = 0;
    }
  else
      info.event = phCALLOK;

  info.u.remoteUri = je->remote_uri; 
  info.vlid = ca->vlid;

  info.streams = PH_STREAM_AUDIO;
  if (ca->video_payload)
    info.streams |= PH_STREAM_VIDEO_RX;


  if (!ca->localrefer)
    phcb->callProgress(ca->cid, &info);

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

  clear(info);


  ca = ph_locate_call(je, 1);
  if (ca)
    rca = ph_locate_call_by_cid(ca->rcid);


  if (!ca->localrefer)
    {
      ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);
      ph_call_media_start(ca, je, -1);

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


  ca = ph_locate_call(je, 1);
  
  if (!ca)
    return;

  clear(info);

  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  info.event = phCALLREDIRECTED;
  info.u.remoteUri = je->remote_contact;
  info.vlid = ca->vlid;
  info.newcid = 0;
  
  if (phcfg.autoredir)
    info.newcid = phLinePlaceCall(info.vlid, je->remote_contact, 0, 0);

  ph_release_call(ca);

  

  phcb->callProgress(je->cid, &info);

  


}

void 
ph_call_ringing(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;
  int cng=0;

  clear(info);

  ca = ph_locate_call(je, 1);
  if (ca)
    rca = ph_locate_call_by_cid(ca->rcid);

#ifdef AMR_OVER_G729_HACK
  if (0 && ca->audio_payload != PH_MEDIA_G729_PAYLOAD) /* HACK to HIJACK G729transport for AMR-WB delivery */
#endif

  ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);
  ph_call_media_start(ca, je, -1);

  
  info.localUri = je->local_uri;
  info.userData = je->external_reference;
  info.event = phRINGING;
  info.u.remoteUri = je->remote_uri; 
  info.vlid = ca->vlid;

  info.streams = ca->nego_mflags;

  phcb->callProgress(je->cid, &info);

  if (rca)
    ph_refer_notify(rca->rdid, 180, "Ringing", 0);


}


void 
ph_call_requestfailure(eXosip_event_t *je)
{
  phCallStateInfo_t info;
  phcall_t *ca, *rca=0;
  int i;


#if 0
  if (je->status_code == 401 || 407 == je->status_code) /* need authetication? */
    {
      eXosip_lock();
      i = eXosip_retry_call(je->cid);
      eXosip_unlock();
      return;
    }
#endif

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
  if (ca->hasaudio)
    ph_media_stop(ca);

  //  ph_media_suspend(ca);

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


  ca = ph_locate_call(je, 0);
  if (!ca)
    return;

  clear(info);

  info.vlid = ca->vlid;
  // ph_media_resume(ca);

//  if (ca->hasaudio)
//      ph_media_stop(ca);
  
 
  remhold = ca->remotehold;
  ca->remotehold = 0;


  ph_call_retrieve_payloads(ca, je, -1);
  ph_call_media_start(ca, je, -1);

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
#if USE_VLINES  

  vl =  ph_find_vline_by_rid(je->rid);
  
  if (!vl)
    return;

  if (vl->regTimeout == 0)
    mask = PH_UNREG_MASK;
  vlid = ph_vline2vlid(vl);


#else
  vlid = je->rid;
#endif


  SKIP(printf("REGPROGRESS reg=%d for vlid=%d\n", je->rid, vlid));

  if (je->type == EXOSIP_REGISTRATION_SUCCESS)
    {
      phcb->regProgress(vlid, 0 | mask);
#if USE_VLINES  
      if (vl->used == VL_DELETING)
	vline_free(vl);
#endif
    }    
  else if (je->type == EXOSIP_REGISTRATION_FAILURE)
    {
      if (je->status_code == 401 || je->status_code == 407)
	{
	  eXosip_lock();
	  i = eXosip_register(je->rid, -1);
	  eXosip_unlock();
	  SKIP(printf("Retrying reg=%d for vlid=%d i=%d\n", je->rid, vlid, i))
	  if (i == 0) return;
	}
      phcb->regProgress(vlid, mask | (je->status_code ? je->status_code : 500) );
#if USE_VLINES  
    if (vl->used == VL_DELETING)
      vline_free(vl);
#endif
    }
    
}



void ph_notify_handler(eXosip_event_t *je)
{
	if (phcb->onNotify) {
		phcb->onNotify(je->sip_event, je->remote_uri, je->msg_body);
	}
}

void ph_call_refered(eXosip_event_t *je)
{
  phcall_t *ca;
  phCallStateInfo_t info;
  int nCid;
  struct vline *vl;
  char utmp[256];

  ca = ph_locate_call_by_cid(je->cid);
  
  if (!ca)
    return;

  clear(info);

  ca->rdid = je->did;
  
  vl = ph_valid_vlid(ca->vlid);
 

  if (ca->hasaudio)
    ph_media_stop(ca);

  if (!vl)
    return;

  snprintf(utmp, sizeof(utmp), "sip:%s@%s", vl->username, vl->server);



  info.newcid = phPlaceCall2(utmp, je->refer_to,  0, je->cid);
  

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


  if (status < 200 && status >= 100)
    {
    info.event = phXFERPROGRESS;
    }
  else if (status >= 200 && status < 300)
    {
    info.event = phXFEROK;
    }
  else
    info.event = phXFERFAIL;


  phcb->callProgress(je->cid, &info);

  if (info.event == phXFEROK)
    {
    phCloseCall(ca->cid);
    if (ca->txcid)
      phCloseCall(ca->txcid);
    }
  else if (info.event == phXFERFAIL)
    {
      if (!ca->txcid)  /* blind transfer */
	{
	  phCloseCall(ca->cid);
	}
      else
	{
	  /* assisted tranfser: resume the original call */
	  phCloseCall(ca->txcid);
	  phResumeCall(ca->cid);
	}
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

int
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

#if USE_VLINES
    ph_refresh_vlines();
#endif

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
	while(1) {
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
