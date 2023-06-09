#ifndef __PHAPI_H__
#define __PHAPI_H__ 1
/*
  The phapi module implements simple interface for eXosip stack
  Copyright (C) 2004  Vadim Lebedev  <vadim@mbdsys.com>
  
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

/**
 * @file phoneapi.h
 * @brief softphone  API
 *
 * phoneapi is a library providing simplified api to create VOIP sessions
 * using eXosip library oSIP stack and oRTP stack 
 * <P>
 */


/**
 * @defgroup phAPI  Phone API
 * @{
 *  
 *  From the perspecitve of the phApi client the call can be in the following states:
 *   
 *   [INCOMING], [ACCEPTING], [OUTGOING], [ESTABLISHED], [ONHOLD], [CLOSED]
 *
 *                
 *                                              V
 *                           +--(INCALL)--------+----(phPlaceCall)-------------------+
 *                           |                                                       |
 *                           |                                                       |
 *                           v                                                       v           
 *  +--(phRejectCall)--<-[INCOMING]<----<------- +                               [OUTGOING]--(DIALING/RINGING)-->-+
 *  |                        v                   ^                                v   v  ^                        |
 *  |                        |                   |                                |   |  |                        |
 *  |                        +--(phRingingCall)--+                                |   |  |                        |
 *  |                        |                                                    |   |  |                        |
 *  |                        v                                                    |   |  |                        |
 *  |                   (phAcceptCall)            +--------+                      |   |  +------------------------+
 *  |                        |                 (DTMF)      |                      |   |
 *  |                        |                    ^        |                  (CALLOK)|
 *  |                        v                    |        v                      v   |
 *  |                  [ACCEPTING]->--(ret==0)-->[ESTABLISHED]<--+---------+------+   +-------->+
 *  |                        |                    v        v               ^                    v
 *  |                        |                    |        |               |                    |
 *  |                (CALLCLOSED/CALLERROR)       +   (CALLHELD/HOLDOK)    |    (CALLCLOSED/CALLERROR/CALLBUSY/NOANSWER/REDIRECTED)
 *  |                        |                    |        |               |                    |
 *  |                        |             (CALLCLOSED)    |               ^                    |
 *  |                        v                    |        +->[ONHOLD]->(CALLRESUMED/RESUMEOK)  |
 *  |                        |                    |             v                               |
 *  |                        |                    |             |                               |
 *  |                        |                    +<-----(CALLCLOSED)                           |
 *  |                        |                    |                                             |
 *  v                        |                    v                                             v
 *  +------------------------+------->[CLOSED]<---+---------------------------------------------+
 *
 *
 *   Blind transfer operation
 *
 *   Suppose we have 2 parties A and B  and a call 'cid'  established between them.
 *   On the A's side the call is identified bu cidA  and on the B's side the call is identfied by cidB.
 *   Suppose the user A  want to transfer the call to a 3rd party  P.
 *   So A does :
 *     phBlindTransferCall(cidA, "P");
 *   this will cause following sequence of events:
 *     1. B  will receive a CALLHELD event
 *     2. B  will receive a XFERREQ  event for cidB containingg "P" as remoteUri and newcid field will 
 *           contain a callid for an automatically generated call to the new destination
 *   A will receive a XFERPROGRESS event
 *   P will receive a INCALL event
 *   P will do:
 *     phAcceptCall
 *   B will receive CALLOK
 *   A will receive XFEROK
 *   A will get CALLCLOSED for cidA
 *   B will get CALLCLOSED  for cidB
 *
 *  In case of failure transfer for whatever reason A will receive an
 *   XFERFAIL event with status field containing SIP status code 
 * 
 *   the file ../miniua/minua.c contains the code demonstrating the usage of blind transfer
 *
 *
 *  Assisted Transfer operation:
 *
 *   Suppose we have 2 parties A and B  and a call 'cid'  established between them.
 *   On the A's side the call is identified bu cidA  and on the B's side the call is identfied by cidB.
 *   Suppose the user A  want to transfer the call to a 3rd party  P.
 *   So A does :
 *     phHoldCall(cidA);
 *   B will receive  CALLHELD event
 *   A does:
 *       newcid = phPlaceCall2("A", "P");
 *   P Gets INCALL event with cidP0 and accepts it
 *   A talks to B and then does
 *     phHoldCall(newcid)
 *   P gets CALLHELD event
 *   A does
 *     phTransferCall(cidA, newcid)
 *   B  will receive a XFERREQ  event for cidB containing "P" as remoteUri and newcid field will 
 *      contain a callid for an automatically generated call to the new destination
 *   A gets XFERPROGRESS events
 *   P will get CALLREPLACED for cidP0 with newcid cidP1
 *   A gets XFEROK event
 *   A will get CALLCLOSED for cidA
 *   B will get CALLCLOSED for cidB
 *   P will get CALLCLOSED for cidP0  
 *        
 */

#ifdef WIN32  
#ifdef MY_DLLEXPORTS
#define MY_DLLEXPORT __declspec(dllexport)
#else
#define MY_DLLEXPORT __declspec(dllimport)
#endif
#else //WIN32
#define MY_DLLEXPORT
#endif

#ifndef PHAPI_VERSION 
#define PHAPI_VERSION "0.0.3"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

enum phErrors {
  PH_ERROR=1,    /* generic error */
  PH_HOLDERR,     /* HOLD/RESUME error */
  PH_BADID,      /* bad identity -  usually ther is no virtual line which correpond to 'from' or userid parameter */
  PH_BADVLID,   /*  Bad virual line ID  */
  PH_BADCID,     /* Bad callid */
  PH_NOMEDIA,     /* No media stream avalable */
  PH_NOTUNNEL,    /* Unable to create tunnel */
  PH_NORESOURCES, /* No resources for operation */
  PH_RPCERR,     /* RPC error */
  PH_BADARG,      /* BAD argument */
};



#ifdef WIN32
#include <windows.h>
#endif

#define PH_REFRESH_INTERVAL		30  /* 30 seconds */

#define PH_SOCK_MODE_UDP 0
#define PH_SOCK_MODE_HTTP_TUNNEL 1
#define PH_STREAM_AUDIO (1 << 0)
#define PH_STREAM_VIDEO_RX (1 << 1)
#define PH_STREAM_VIDEO_TX (1 << 2)


/**
 * Place an outgoing call
 * 
 * @param from         call originator address
 * @param uri          call destination address
 * @param userData     application specific data
 * @return             if positive the call id else error indication
 */
MY_DLLEXPORT int phPlaceCall(const char *from, const char *uri, void *userData);



/**
 * Place an outgoing call using given virtual line
 * 
 * @param vlid         virtual line id to use for the call
 * @param uri          call destination address
 * @param userData     application specific data
 * @param rcid         call id of the original call (MUST BE ZERO) 
 * @return             if positive the call id else error indication
 */
MY_DLLEXPORT int phLinePlaceCall(int vlid, const char *uri, void *userData, int rcid);
MY_DLLEXPORT int phLinePlaceCall2(int vlid, const char *uri, void *userData, int rcid, int streams);


/**
 * Place an outgoing call as a result of transfer reqeust
 * 
 * @param from         call originator address
 * @param uri          call destination address
 * @param userData     application specific data
 * @param rcid         call id of the original call 
 * @return             if positive the call id else error indication
 */
MY_DLLEXPORT int phPlaceCall2(const char *from, const char *uri, void *userData, int rcid);
MY_DLLEXPORT int phPlaceCall3(const char *from, const char *uri, void *userData, int rcid, int streams);

/**
 * Accept an incoming a call.
 * 
 * @param cid          call id of call.
 * @return             0 in case of success
 */
MY_DLLEXPORT int phAcceptCall2(int cid, void *userData);
MY_DLLEXPORT int phAcceptCall3(int cid, void *userData, int streams);

#define phAcceptCall(cid) phAcceptCall2(cid, 0)

/**
 * Reject the incoming call.
 * 
 * @param cid          call id of call.
 * @param reason       SIP reason code for the rejection
 *                     suggested values: 486 - Busy Here, 488 - Not acceptable here
 * @return             0 in case of success
 */
MY_DLLEXPORT int phRejectCall(int cid, int reason);


/**
 * Signal reining event to the remote side.
 * 
 * @param cid          call id of call.
 * @return             0 in case of success
 */
MY_DLLEXPORT int phRingingCall(int cid);



/**
 * Terminate a call.
 * 
 * @param cid          call id of call.
 * @return             0 in case of success
 */
MY_DLLEXPORT int phCloseCall(int cid);


/**
 * Perform a blind call transfer
 * 
 * @param cid          call id of call.
 * @param uri          call transfer destination address
 * @return             txid  used in the subsequent transferProgress callback
 */
MY_DLLEXPORT int phBlindTransferCall(int cid, const char *uri);


/**
 * Perform an assisted  call transfer
 * 
 * @param cid          call id of call.
 * @param targetCid    call id of the destination call
 * @return             txid  used in the subsequent transferProgress callback
 */
MY_DLLEXPORT int phTransferCall(int cid, int targetCid);



/**
 * Resume previously held call
 * 
 * @param cid          call id of call.
 * @return             0 in case of success
 */
MY_DLLEXPORT int phResumeCall(int cid);


/**
 * Put a call on hold
 * 
 * @param cid          call id of call.
 * @return             0 in case of success
 */
MY_DLLEXPORT int phHoldCall(int cid);


/**
 * Send an OPTIONS packet
 *  
 * @param from  --  url to put in the From: header
 * @param to    --  uri to put in the To: header
*/   
MY_DLLEXPORT int phSendOptions(const char *from, const char *to);

/**
 * Configure tunnel parameters:		This funtion should be call just before phInit, when the global variable phcfg 
 *									is properly set up.
 *  
 * @param http_proxy			    IP address of http proxy in local network, set to 0 if there is no local proxy
 * @param http_proxy				Port of local http proxy
 * @param httpt_server				IP address of the HTTP TUNNEL SERVER that do the http to udp conversion
 * @param httpt_server_port			Port of the HTTP TUNNEL SERVER that do the http to udp conversion
 * @param proxy_user				User name used for authentication purpose
 * @param proxy_passwd				Password for proxy
 * @param autoconf	                If this is set to 1, this funtion will try to detect network configuration when it can't 
									setup a tunnel with given parameters.
*/   
MY_DLLEXPORT int phTunnelConfig(const char* http_proxy, const int http_proxy_port,
								const char* httpt_server, const int httpt_server_port, 
								const char *proxy_user, const char* proxy_passwd,
								int autotoconf);


/**
 * Send a MESSAGE request
 *  
 * @param from  --  url to put in the From: header
 * @param to    --  uri to put in the To: header
 * @param message -- message to send
 * @return --  if positive msgid
*/
MY_DLLEXPORT int phSendMessage(const char *from, const char *uri, const char *buff);


/* 
   winfo = 0 -> subscribe with event = presence
   winfo = 1 -> subscribe with event = presence.winfo
*/
MY_DLLEXPORT int phSubscribe(const char *from, const char *to, const int winfo); 

MY_DLLEXPORT int phPublish(const char *from, const char *to, const int winfo, const char * content_type, const char * content);

MY_DLLEXPORT int phPing(char* host, int port, int ttl);

MY_DLLEXPORT void phRefresh();



/**
 * Send a DTMF to remote party
 * 
 * @param cid          call id of call.
 * @param dtmfChar     DTMF event to send 
 *                     ('0','1','2','3','4','5','6','7','8','9','0','#','A','B','C','D','!')
 * @mode               bitmask specifying DTMF geneartion mode 
 *                     INBAND - the DTMF signal is mixed in the outgoing
 *                     RTPPAYLOAD - the DTMF signal will be sent using telephone_event RTP payload 
 * @return             0 in case of success
 */
#define PH_DTMF_MODE_INBAND 1
#define PH_DTMF_MODE_RTPPAYLOAD 2
#define PH_DTMF_MODE_ALL 3  
MY_DLLEXPORT int phSendDtmf(int cid, int dtmfChar, int mode);





/**
 * Play a sound file
 * 
 * @param fileName     file to play (the file externsion will determine the codec to use
 *                     .sw - 16bit signed PCM, .ul - uLaw, .al - aLaw, .gsm - GSM, (.wav on Windows)
 * @param loop         when TRUE play the file in loop
 * @return             0 in case of success
 */
MY_DLLEXPORT int phPlaySoundFile(const char *fileName , int loop);

/**
 * Stop playing a sound file
 * 
 * @return             0 in case of success
 */
MY_DLLEXPORT int phStopSoundFile( void );



/**
 * Mix a sound file into the outgoing network audio stream
 * 
 * @param cid          call id
 * @param fileName     file to play - for the moment only 
 *                      RAW audio files containing 16Bit signed PCM sampled at 16KHZ are supported 
 * @return             0 in case of success
 */
MY_DLLEXPORT int phSendSoundFile(int cid, const char *fileName);



/**
 * Set speaker volume  
 * 
 * @param      cid       call id (-1 for general volume, -2 for playing sounds)
 * @param      volume    0 - 100
 * @return             0 in case of success
 */
MY_DLLEXPORT int phSetSpeakerVolume(int cid,  int volume);


/**
 * Set recording level  
 * 
 * @param      cid - call id (-1 for general recording level)
 * @param      level    0 - 100
 * @return             0 in case of success
 */
MY_DLLEXPORT int phSetRecLevel(int cid,  int level);



/**
 * Configure follow me address. All 
 * incoming call will be redirected to this address
 * 
 * @param uri          destination of the forwarding
 * @return             0 in case of success
 */
MY_DLLEXPORT int phSetFollowMe(const char *uri);


/**
 * Set busy mode
 * When activated all incoming calls will be answerd by busy signal
 * 
 * @param busyFlag          when 0 busy mode is deactivated else activated
 * @return             0 in case of success
 */
MY_DLLEXPORT int phSetBusy(int busyFlag);




/**
 * Add authentication info
 * the given info will be to send as authentication information 
 * when server request it.
 * 
 * @param  username    username which will figure in the From: headers 
 *                     (usually the same as userid)
 * @param  userid      userid field value
 * @param  realm       authentication realm
 * @param  passwd      password correspoinding to the userid
 * @return             0 in case of success
 */
MY_DLLEXPORT int phAddAuthInfo(const char *username, const char *userid,
	      const char *passwd, const char *ha1,
	      const char *realm);

/**
 * Add virtual line
 * The virtual line corresponds to identity/server/proxy triplet 
 * 
 * @param  displayname display name component of the SIP identity "displayname" <sip:username@host> 
 * @param  username    username    
 * @param  host        the host component of SIP identity username@host corresponding to this virtual ligne
 *                     if regTimeout != 0 'host' will designate the REGISTRAR server, in this case it may have form of host:port
 *                     otherwise it should be set to IP address or hostname of the local machine
 * @param  proxy       outgoing proxy URI  (all calls using this virtual ligne will be routed
 *                     through this proxy) 
 * @param  regTimeout  registration timeout  (when 0 will NOT use registrations)
 *                     to unergister one should do phDelVline (or phUnregister -- depreciated)
 * @return             -1 in case of error vlid  in case of success
 */

MY_DLLEXPORT int phAddVline(const char* username, const char *host, const char*  proxy,  int regTimeout);      
MY_DLLEXPORT int phAddVline2(const char* displayname, const char* username, const char *host, const char*  proxy,  int regTimeout);      


/**
 * Delete virtual line 
 *  This will cause REGISTER request with timeout=0 to be sent to server if needed
 *
 * @param  vlid        Virual line id to remove 
 * @return             0 in case of success
 */
MY_DLLEXPORT int phDelVline(int vlid);

/**
 * Change Audio devices
 * change the audio devices used by phApi. The change will take effect for
 * the new calls only.
 * 
 * @param  devstr    the same value as in phcfg.audio_dev
 * @return           0 on success, or error code
 * 
 */

MY_DLLEXPORT int phChangeAudioDevices(const char *devstr);

/**
 * Register to the server  (**DEPRECIATED**)
 * Try to refister the given user name at the server
 * 
 * @param  server    the name or ip adderss of the registration server
 * @param  username  username to be used in the From: header
 * @param  timeout   registration timeout in seconds (0 correspond to de-registration request)
 * @return           if positive registration id which will be used in regProgress callback
 * 
 */
MY_DLLEXPORT int phRegister2(const char *username, const char *server, int timeout);

#define phRegister(u, s) phRegister2(u, s, 3600)
#define phUnregister(u, s) phRegister2(u, s, 0)







/**
 * @enum phCallStateEvent
 * @brief call progress events.
 *
 */ 
enum  phCallStateEvent { 
  phDIALING, phRINGING, phNOANSWER, phCALLBUSY, phCALLREDIRECTED, phCALLOK, 
  phCALLHELD, phCALLRESUMED, phHOLDOK, phRESUMEOK, 
  phINCALL, phCALLCLOSED, phCALLERROR, phDTMF, phXFERPROGRESS, phXFEROK, phXFERFAIL, phXFERREQ, phCALLREPLACED
};

/**
 * @struct phCallStateInfo
 */
struct phCallStateInfo {
  enum phCallStateEvent event;
  void *userData;              /*!< used to match placeCall with callbacks */ 
  const char *localUri;        /*!< valid for all events execpt CALLCLOSED and DTMF */
  int   newcid;                /*!< valid for CALLREPLACED and XFERREQ */
  int   vlid;                  /*! virtual line id */
  int   streams;               /*!  proposed (for phINCALL) and active (for other events) streams for the call */
  union {
    const char  *remoteUri;    /*!< valid for all events execpt CALLCLOSED, DTMF and CALLERROR */
    int   errorCode;           /*!< valid for CALLERROR */
    int   dtmfDigit;           /*!< valid for DTMF */  
  } u;
};

enum phMsgEvent
  {
    phMsgNew, phMsgOk, phMsgError
  };

 
struct phMsgStateInfo {
  enum phMsgEvent event;
  int   status;
  const char *from;
  const char *to;
  const char *ctype;
  const char *subtype;
  const char *content;
};

enum phSubscriptionEvent {
	phSubscriptionOk, phSubscriptionErrNotFound, phSubscriptionError
};

struct phSubscriptionStateInfo  {
	enum phSubscriptionEvent event;
	int status;
	const char *from;
	const char *to;
};

struct phVideoFrameReceivedEvent {
	struct webcam *cam;
	void *frame_remote;
	int frame_remote_width;
	int frame_remote_height;

	void *frame_local;
	int frame_local_width;
	int frame_local_height;
};

typedef struct phCallStateInfo phCallStateInfo_t;
typedef struct phTransferStateInfo phTransferStateInfo_t;
typedef struct phConfStateInfo phConfStateInfo_t;
typedef struct phRegStateInfo phRegStateInfo_t; 
typedef struct phMsgStateInfo  phMsgStateInfo_t;
typedef struct phSubscriptionStateInfo  phSubscriptionStateInfo_t;
typedef struct phVideoFrameReceivedEvent  phVideoFrameReceivedEvent_t;

#define PH_UNREG_MASK               0x8000            /* this mask is ored with regStatus to distingush REGISTER from UNREGISTER */

struct webcam;
    
typedef void (*phFrameDisplayCbk)(int cid, phVideoFrameReceivedEvent_t *ev);

/**
 * @struct phCallbacks
 * @brief  callbacks to the MMI
 */  
struct phCallbacks
{
  void  (*callProgress)(int cid, const phCallStateInfo_t *info);       /*!< call progress callback routine */
  void  (*transferProgress)(int cid, const phTransferStateInfo_t *info); /*!< transfer progress callback routine */
  void  (*confProgress)(int cfid, const phConfStateInfo_t *info);        /*!< conference progress callback routine */
  void  (*regProgress)(int regid, int regStatus);                       /*!< registration status (0 - OK, else SIP error code */
  void  (*msgProgress)(int mid,  const phMsgStateInfo_t *info);
  void  (*onNotify) (const char* event, const char* from, const char* content);
  void  (*subscriptionProgress)(int sid,  const phSubscriptionStateInfo_t *info);
  phFrameDisplayCbk onFrameReady;
  void  (*errorNotify) (enum phErrors error);
};


typedef struct phCallbacks phCallbacks_t;

/**
 * @var phcb
 * @brief pointer to callback structure
 * 
 */

MY_DLLEXPORT extern phCallbacks_t *phcb;


/**
  @var phIsInitialize 
  @brief 1 : if phInit has been called and phTerminate has not been called
	 0 : Otherwise
*/
MY_DLLEXPORT extern int phIsInitialized;


/**
 * Initilize phApi
 *
 * @param cbk          pointer to callback descriptor
 * @param server       string containing an ip address of the phApi server
 *                     (ignored when in direct link mode)
 * @param asyncmode    when != 0 a thread will be created to deliver
 *                     callbacks asyncronously, othewise the client
 *                     is supposed to call phPoll periodically to get
 *                     phApi events delivered
 *                     In DIRECT link mode this parameter is copied to the phcfg.asyncmode structure
 *                     in client/server mode this parameter has client local meaning.
 */
MY_DLLEXPORT int phInit(phCallbacks_t *cbk, char *server, int asyncmode);




/**
 * Get the version of the phAPI module
 *
 * @return  encoded value corresponding to Version.Subversion.Release
 *
 */
MY_DLLEXPORT int phGetVersion(void);


#define phRelease(v) ((v) & 0xff)
#define phSubversion(v) ((v >> 8) & 0xff)
#define phVersion(v) ((v >> 16) & 0xff)


/**
 * Return the NAT router type and it's ip address
 *
 * @param   natType  string buffer to receive nat type string (possible "fcone,rcone,prcone,sym,open")
 * @param   ntlen  sizeof natType buffer
 * @param   fwip   firewall ip address buffer
 * @param   sizeof of fwip buffer
 * @return  error or success code
 *
 */
MY_DLLEXPORT int phGetNatInfo(char *natType, int ntlen, char *fwip, int fwiplen);



/**
 * poll for phApi events
 */
MY_DLLEXPORT int phPoll( void );


/**
 *  Terminate phApi
 */
MY_DLLEXPORT void phTerminate( void );     

struct phVideoConfig {

#define PHAPI_VIDEO_LINE_128KBPS	1
#define PHAPI_VIDEO_LINE_256KBPS	2
#define PHAPI_VIDEO_LINE_512KBPS	3
#define PHAPI_VIDEO_LINE_1024KBPS	4
#define PHAPI_VIDEO_LINE_2048KBPS	5
#define PHAPI_VIDEO_LINE_AUTOMATIC	6

	int video_fps;
	int video_camera_flip_frame;
	int video_max_frame_size;
	int video_webcam_capture_width;
	int video_webcam_capture_height;
	int video_line_configuration;
	int video_codec_max_bitrate;
	int video_codec_min_bitrate;
	char video_device[256];
};

/*
 * Temporary structure that holds codec config, to be set from GUI
 *
 */

struct phVideoCodecConfig {
	int minrate;
	int maxrate;
	int gopsize;
	int qmin;
	int qmax;
	float b_offset;
	float b_factor;
	float i_offset;
	float i_factor;
	int compress;
	int max_b_frame;
	int f_quality;
};

/**
 * @struct phConfig
 * @brief ph API configuration info
 */
#define VAD_VALID_MASK 0x80000000
#define VAD_THRESHOLD_MASK 0x7fffffff

#ifdef EMBED
struct phConfig
{
  char local_rtp_port[16]; /*!< port number used for RTP data */ /* range 1st - last */
  char local_rtcp_port[16]; /*!< port number used for RTCP data */
  char sipport[16];         /*!< sip port number */
  char identity[256];       /*!< DEPRECIATED! Use Virtual Lines instead.  my sip address (this field is temporary hack) */
  char proxy[256];          /*!< DEPRECIATED! User Virtual Lines instead. proxy address (this field is temporary hack)  */ 
  char nattype[16];         /*!< nat type (auto,none,fcone,rcone,prcone,sym)  */
  char audio_codecs[128];         /*!< comma separate list of codecs in order of priority */
  char video_codecs[128];         /*!< comma separate list of codecs in order of priority */
                            /* example: PCMU,PCMA,GSM,ILBC,SPEEX   */
  int  force_proxy;         /*!< DEPRECIATED... (Use virtual lines instead) when set to 1 causes all SIP packet go through specifed proxy */
  int  asyncmode;           /*!< when true phApi creates a separate eXosip polling thread... in client/server mode MUST be TRUE */
  char audio_dev[64];       /*!< audio device identifier */
			    /* example: IN=2 OUT=1 ; 2 is input device and 1 is ouput device */
  int softboost;            /* to be removed */
  int nomedia;
  int noaec;		    /* when non-zero - disable aec */
  unsigned int vad;         /* if bit31=1  DTX/VAD features activated and bits0-30 contains the power threshold */
  int cng;                  /* if 1,  CNG feature will be negotiated */
  int nat_refresh_time;       /* timeout for sip address/port refresh (when 0 no-refresh) */
  int jitterdepth;           /* jitter buffer depth in miliseconds (if 0 default of 60 msecs is used) */ 
  int nodefaultline;         /* temporary hack for implementing backward compatibility... Don't touch it */
  int autoredir;            /*!< when NONZERO the redirect requests will be automatically executed by phApi 
			      the new CID will be deliverd in newcid field  in the CALLREDIRECTED event */
  char stunserver[128];      /*!< stun server address:port or name:port */

#define PH_TUNNEL_AUTOCONF 2
#define PH_TUNNEL_USE  1
  int use_tunnel;


  char httpt_server[128];
  int  httpt_server_port;
  char http_proxy[128];
  int  http_proxy_port;
  char http_proxy_user[128];
  char http_proxy_passwd[128];

 char video_codecs[128];         /*!< comma separate list of codecs in order of priority */
 char local_video_rtp_port[16]; /*!< port number used for video RTP data */ 
 char local_video_rtcp_port[16]; /*!< port number used for video RTCP data */ 
 

};
#else
struct phConfig
{
  char local_rtp_port[16]; /*!< port number used for RTP data */ 
  char local_audio_rtcp_port[16]; /*!< port number used for RTCP data */
  char local_video_rtp_port[16]; /*!< port number used for video RTP data */ 
  char local_video_rtcp_port[16]; /*!< port number used for video RTCP data */ 

  char sipport[16];              /*!< sip port number */
  char identity[256];       /*!< DEPRECIATED! Use Virtual Lines instead.  my sip address (this field is temporary hack) */
  char proxy[256];          /*!< DEPRECIATED! User Virtual Lines instead. proxy address (this field is temporary hack)  */ 
  char nattype[16];         /*!< nat type (auto,none,fcone,rcone,prcone,sym)  */
  char audio_codecs[128];         /*!< comma separate list of codecs in order of priority */
  char video_codecs[128];         /*!< comma separate list of codecs in order of priority */
                            /* example: PCMU,PCMA,GSM,ILBC,SPEEX   */

  int  force_proxy;         /*!< DEPRECIATED... (Use virtual lines instead) when set to 1 causes all SIP packet go through specifed proxy */
  int  asyncmode;           /*!< when true phApi creates a separate eXosip polling thread... in client/server mode MUST be TRUE */
  char audio_dev[64];       /*!< audio device identifier */
			    /* example: IN=2 OUT=1 ; 2 is input device and 1 is ouput device */
  int softboost;            /* to be removed */
  int nomedia;
  int noaec;				/* when non-zero - disable aec */
  unsigned int vad;         /* if bit31=1  DTX/VAD features activated and bits0-30 contains the power threshold */
  int cng;                  /* if 1,  CNG feature will be negotiated */
  int nat_refresh_time;       /* timeout for sip address/port refresh (when 0 no-refresh) */
  int jitterdepth;           /* jitter buffer depth in miliseconds (if 0 default of 60 msecs is used) */
  int nodefaultline;         /* temporary hack for implementing backward compatibility... Don't touch it */
  int autoredir;            /*!< when NONZERO the redirect requests will be automatically executed by phApi 
			      the new CID will be deliverd in newcid field  in the CALLREDIRECTED event */
  char stunserver[128]; /*!< stun server address:port or name:port */

#define PH_TUNNEL_AUTOCONF 2
#define PH_TUNNEL_USE  1
  int  use_tunnel; 


  char httpt_server[128];
  int  httpt_server_port;
  char http_proxy[128];
  int  http_proxy_port;
  char http_proxy_user[128];
  char http_proxy_passwd[128];
#ifdef WIN32
  HWND videoHandle;
#endif

  struct phVideoConfig video_config;
};
#endif



typedef struct phConfig phConfig_t;

/**
 * @var phconfig
 * @brief variable storing the ph API configuration
 */
MY_DLLEXPORT extern phConfig_t phcfg;

/**
 * variable storing the name of the log file
 */ 
MY_DLLEXPORT extern char *phLogFileName;


/**
 * Debugging level (between 0 and 9)
 */ 
MY_DLLEXPORT extern int phDebugLevel;

MY_DLLEXPORT extern unsigned short phServerPort;

MY_DLLEXPORT phConfig_t *phGetConfig();


MY_DLLEXPORT int phVideoControlChangeFps(int callid, int fps);
MY_DLLEXPORT int phVideoControlChangeQuality(int callid, int quality);
MY_DLLEXPORT int phVideoControlSetCameraFlip(int flip);
MY_DLLEXPORT int phVideoControlSetWebcamCaptureResolution(int width, int height);
MY_DLLEXPORT int phVideoControlSetBitrate(int callid, int maxrate, int minrate);
MY_DLLEXPORT void phVideoControlCodecSet(int, struct phVideoCodecConfig *);
MY_DLLEXPORT void phVideoControlCodecGet(int, struct phVideoCodecConfig *);

/* JT */
#ifdef QOS_DEBUG_ENABLE
MY_DLLEXPORT void phrtcp_QoS_enable_rtcp_report(int ToF);
MY_DLLEXPORT void phrtcp_report_set_cb(jt_rtcpCallbacks_t *cbk);
MY_DLLEXPORT int phrtcp_report_begin();
MY_DLLEXPORT int phrtcp_report_end();
#endif /* QOS_DEBUG_ENABLE */
#ifdef __cplusplus
}
#endif

/** @} */

#endif
