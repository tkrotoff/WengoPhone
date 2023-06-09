/*
 * PhApi, a voice over Internet library
 * Copyright (C) 2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _PH_NEW_API_HEADER_
#define _PH_NEW_API_HEADER_

#ifdef WIN32
	#if defined(BUILD_PHAPI_DLL)
		#define MY_DLLEXPORT __declspec(dllexport)
	#elif defined(PHAPI_DLL)
		#define MY_DLLEXPORT __declspec(dllimport)
	#endif
#endif

#ifndef MY_DLLEXPORT
	#define MY_DLLEXPORT
#endif

#include <phapi-old.h>
#include <owpl_plugin.h>
#include <owpl.h>
#include <phvline.h>
#include <stdio.h>

#define OWPL_STREAM_AUDIO		PH_STREAM_AUDIO
#define OWPL_STREAM_VIDEO_RX	PH_STREAM_VIDEO_RX
#define OWPL_STREAM_VIDEO_TX	PH_STREAM_VIDEO_TX

#define OWPL_TUNNEL_NOT_USED	0
#define OWPL_TUNNEL_USE			PH_TUNNEL_USE // 1
#define	OWPL_TUNNEL_SSL			PH_TUNNEL_SSL // 1 << 2
#define OWPL_TUNEL_AUTOCONF		PH_TUNNEL_AUTOCONF // 1 << 1

#ifdef __cplusplus
extern "C" {
#endif

extern int owStaticCallId;

typedef enum {
	OWPL_NAT_TYPE_NONE,
	OWPL_NAT_TYPE_AUTO,
	OWPL_NAT_TYPE_FCONE,
	OWPL_NAT_TYPE_RCONE,
	OWPL_NAT_TYPE_PRCONE,
	OWPL_NAT_TYPE_SYMETRIC
} OWPL_NAT_TYPE;

typedef enum {
	OWPL_TYPING_STATE_TYPING,
	OWPL_TYPING_STATE_STOP_TYPING,
	OWPL_TYPING_STATE_NOT_TYPING
} OWPL_TYPING_STATE;

typedef enum OWPL_CALL_REFUSED_REASON_
{
    OWPL_CALL_REFUSED_BUSY,      /**< Busy */
	OWPL_CALL_REFUSED_NOANSWER,
	OWPL_CALL_REFUSED_GLOBAL_FAILURE,
	OWPL_CALL_REFUSED_SERVER_FAILURE,
	OWPL_CALL_REFUSED_REQUEST_FAILURE
} OWPL_CALL_REFUSED_REASON;

/**
 * DTMF/other tone ids used with owplCallStartTone/owplCallStopTone
 */
typedef enum OWPL_TONE_ID_T
{
    ID_DTMF_0              = '0',   /**< DMTF 0 */
    ID_DTMF_1              = '1',   /**< DMTF 1 */
    ID_DTMF_2              = '2',   /**< DMTF 2 */
    ID_DTMF_3              = '3',   /**< DMTF 3 */
    ID_DTMF_4              = '4',   /**< DMTF 4 */
    ID_DTMF_5              = '5',   /**< DMTF 5 */
    ID_DTMF_6              = '6',   /**< DMTF 6 */
    ID_DTMF_7              = '7',   /**< DMTF 7 */
    ID_DTMF_8              = '8',   /**< DMTF 8 */
    ID_DTMF_9              = '9',   /**< DMTF 9 */
    ID_DTMF_STAR           = '*',   /**< DMTF * */
    ID_DTMF_POUND          = '#',   /**< DMTF # */
    ID_DTMF_FLASH          = '!',   /**< DTMF Flash */
    ID_TONE_DIALTONE  = 512,        /**< Dialtone */
    ID_TONE_BUSY,                   /**< Call-busy tone */
    ID_TONE_RINGBACK,               /**< Remote party is ringing feedback tone */
    ID_TONE_RINGTONE,               /**< Default ring/alert tone */
    ID_TONE_CALLFAILED,             /**< Fasy Busy / call failed tone */
    ID_TONE_SILENCE,                /**< Silence */
    ID_TONE_BACKSPACE,              /**< Backspace tone */
    ID_TONE_CALLWAITING,            /**< Call waiting alert tone*/
    ID_TONE_CALLHELD,               /**< Call held feedback tone */
    ID_TONE_LOUD_FAST_BUSY          /**< Off hook / fast busy tone */
} OWPL_TONE_ID ;

/**
 * Signature for log report function.  Application developers should
 * not block the calling thread.
 *
 * @param szLogMsg The log message
 */
typedef int (*OWPL_LOG_REPORT_PROC)(const char * szLogMsg);


MY_DLLEXPORT OWPL_RESULT
owplLogListenerSet(OWPL_LOG_REPORT_PROC pLogReportProc);

/**
 * Initialize the phone library
 *
 * @param	asyncCallbackMode		1: all callbacks are called from a owpl thread
									0: phPoll need to be called. TODO: implement a wrapper for phPoll
 * @param	udpPort					The local UDP port of the SIP stack
 * @param	tcpPort					The local TCP port of the SIP stack (not supported yet)
 * @param	tlsPort					The local TLS port of the SIP stack (not supported yet)
 * @param	szBindToAddr			The local address to bind to. Set to NULL to let owpl 
									choose an address itself
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplInit(	const int asyncCallbackMode,
			const int udpPort,
			const int tcpPort,
			const int tlsPort,
			const char* szBindToAddr,
			const int bUserSequentialPorts);

MY_DLLEXPORT OWPL_RESULT
owplShutdown();

/********************************************************************************************
 *								Phone CONFIG related functions								*
 ********************************************************************************************/

/**
 * Sets the local HTTP proxy parameters.
 *
 * @param	szLocalProxyAddr	the local HTTP proxy ip address
 * @param	LocalProxyPort	the local HTTP proxy port
 * @param	szLocalProxyUserName	the username for the local HTTP proxy
 * @param	szLocalProxyPasswd	the password for the local HTTP proxy
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetLocalHttpProxy(const char* szLocalProxyAddr,
					 const unsigned int LocalProxyPort,
					 const char* szLocalProxyUserName,
					 const char* szLocalProxyPasswd);

/**
 * Sets the parameters for tunnel use.
 *
 * @param	szTunnelSeverAddr	the server ip address
 * @param	TunnelServerPort	the server port
 * @param	TunnelMode	a mask of OWPL_TUNNEL_NOT_USED or OWPL_TUNNEL_USE | OWPL_TUNNEL_SSL | OWPL_TUNNEL_AUTOCONF
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetTunnel(const char* szTunnelSeverAddr,
					 const unsigned int TunnelServerPort,
					 const unsigned int TunnelMode);
/**
 * Sets the NAT configuration
 *
 * @param	eNatType	the nat type
 * @param	natRefreshTime	the nat refresh time in ms
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetNat(const OWPL_NAT_TYPE eNatType,
				 const unsigned int natRefreshTime);

/**
 * DEPRECATED
 * WARNING this function must be called before owplInit if http tunnel is used
 *
 * @param the outbound sip proxy address "ip:port"
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetOutboundProxy(const char * szProxyAddr);

MY_DLLEXPORT OWPL_RESULT
owplConfigAddAudioCodecByName(const char* szCodecName);

/**
 * Replaces any previous codec list by the one given in argument.
 * If NULL or empty string is given, resets the codec list.
 * Codec list must be less long than phcfg.audio_codecs.
 * Codec list is lost if return value is a failure.
 *
 * @param	szCodecs	a list of codecs separated by comas
 * @return	an OWPL_RESULT enum code
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetAudioCodecs(const char * szCodecs);

/**
 * Gets the whole codecs comas separated 0 terminated string.
 * Buffer should be at least the size of phcfg.audio_codecs
 *
 * @param	szCodecs	a buffer receiving the whole codecs comas separated string
 * @param	size	the size of the buffer (typically sizeof(buffer))
 * @return	an OWPL_RESULT enum code
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigGetAudioCodecs(const char * szCodecs, size_t size);

MY_DLLEXPORT OWPL_RESULT
owplConfigAddVideoCodecByName(const char* szCodecName);

/**
 * Replaces any previous codec list by the one given in argument.
 * If NULL or empty string is given, resets the codec list.
 * Codec list must be less long than phcfg.video_codecs
 * Codec list is lost if return value is a failure.
 *
 * @param	szCodecs	a list of codecs separated by comas
 * @return	an OWPL_RESULT enum code
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetVideoCodecs(const char * szCodecs);

/**
 * Gets the whole codecs comas separated 0 terminated string.
 * Buffer should be at least the size of phcfg.video_codecs
 *
 * @param	szCodecs	a buffer receiving the whole codecs comas separated string
 * @param	size	the size of the buffer (typically sizeof(buffer))
 * @return	an OWPL_RESULT enum code
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigGetVideoCodecs(const char * szCodecs, size_t size);

/**
 * Gets the local bound ip address.
 *
 * @param	szLocalAddr	a buffer receiving the local address
 * @param	size	the size of the buffer
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigGetBoundLocalAddr(char * szLocalAddr, size_t size);

/**
 * Gets the local http proxy address.
 *
 * @param	szLocalProxyAddr	a buffer receiving the local http proxy address
 * @param	size	the size of the buffer
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetAddr(char * szLocalProxyAddr, size_t size);

/**
 * Gets the local http proxy password.
 *
 * @param	szLocalProxyPasswd	a buffer receiving the local http proxy password
 * @param	size	the size of the buffer
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetPasswd(char * szLocalProxyPasswd, size_t size);

/**
 * Gets the local http proxy port
 *
 * @param	LocalProxyPort	a int pointer receiving the local http proxy port
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetPort(int * LocalProxyPort);

/**
 * Gets the local http proxy username.
 *
 * @param	szLocalProxyUserName	a buffer receiving the local http proxy username
 * @param	size	the size of the buffer
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetUserName(char * szLocalProxyUserName, size_t size);

/**
 * Enable/Disable encryption for calls
 *
 * @param	EncryptionMode	1 if calls should be encrypted, 0 otherwise
 * @return	OWPL_RESULT_SUCCESS if succeeds, an error value otherwise
 */
MY_DLLEXPORT OWPL_RESULT owplConfigSetEncryptionMode(int EncryptionMode);

/**
 * Check whether encryption is enabled for calls
 *
 * @param (out)		EncryptionMode	contains 1 if encryption is enabled, 0 otherwise
 * @return	OWPL_RESULT_SUCCESS if succeeds, an error value otherwise
 */
MY_DLLEXPORT OWPL_RESULT owplConfigGetEncryptionMode(int *EncryptionMode);

/**
 * Enable or disable presence and IM in phapi.
 *
 * @param	PimEnabled	if 1, enable presence & IM otherwise disable them
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT owplConfigEnablePIM(int PimEnabled);


/********************************************************************************************
 *								Phone AUDIO CONFIG related functions								*
 ********************************************************************************************/

/**
 * Set the audio config string
 * @param szAudioConfig		The audio configuration string
 */
MY_DLLEXPORT OWPL_RESULT owplAudioSetConfigString(const char* szAudioConfig);


/********************************************************************************************
 *								Phone LINE related functions								*
 ********************************************************************************************/

typedef enum LineOptEnum_T
{
	OWPL_LINE_OPT_REG_TIMEOUT,
	OWPL_LINE_OPT_USER_NAME,
	OWPL_LINE_OPT_DOMAIN,
	OWPL_LINE_OPT_PROXY
} LineOptEnum;

typedef enum CallOptEnum_T
{
	OWPL_CALL_OPT_CODEC_AUDIO,
	OWPL_CALL_OPT_CODEC_VIDEO,
	OWPL_CALL_OPT_REMOTE_ADDR
} CallOptEnum;



/**
 * owplLineAdd
 *
 * Add virtual line
 * The virtual line corresponds to identity/server/proxy triplet
 *
 * @param  displayname display name component of the SIP identity "displayname" <sip:username@host>
 * @param  username    username
 * @param  host        the host component of SIP identity username@host corresponding to this virtual line
 *                     if regTimeout != 0 'host' will designate the REGISTRAR server, in this case it may have form of host:port
 *                     otherwise it should be set to IP address or hostname of the local machine
 * @param  proxy       outgoing proxy URI  (all calls using this virtual line will be routed
 *                     through this proxy)
 * @param  regTimeout  registration timeout  (when 0 will NOT use registrations)
 *                     to unergister one should do phDelVline (or phUnregister -- depreciated)
 * @param  phLine	   the handle to the newly created line
 * @return             OWPL_RESULT_SUCCESS in case of success
 */
MY_DLLEXPORT OWPL_RESULT
owplLineAdd(const char* displayname,
			 const char* username,
			 const char *server,
			 const char*  proxy,
			 int regTimeout,
			 OWPL_LINE * phLine);

/**
 * Removes a virtual line
 *
 * @param	hLine	a virtual line handle
 * @param	skipUnregister	a boolean, if 1 skip the unregister and force
							the vline deletion, if 0 does the unregister
							as normal
 * @return	an OWPL_RESULT code
 */
MY_DLLEXPORT OWPL_RESULT
owplLineDelete(OWPL_LINE hLine, unsigned int skipUnregister);

/**
 * owplLineGetProxy
 *
 * Get the proxy address of the line
 *
 * @param  hLine	   handle to the virtual line
 * @param  szBuffer    The buffer to contain the result.
 * @param  nBuffer	   The size of the buffer. On return, nBuffer will contain the actual by written to the buffer.
 * @return             OWPL_RESULT_SUCCESS in case of success. On failure, nBuffer will contain
 *					   the size needed to store the result.
 */
MY_DLLEXPORT OWPL_RESULT
owplLineGetProxy(const OWPL_LINE hLine,
                        char*  szBuffer,
                        int * nBuffer);

/**
 * Get the local username associated with the given hLine
 *
 * @param	hLine	a line handle
 * @param	szLocalUserName	a buffer receiving the local username
 * @param	size	the size of the buffer
 * @return	OWPL_RESULT_SUCCESS if succeeds; an OWPL_RESULT failure code else
 */
MY_DLLEXPORT OWPL_RESULT
owplLineGetLocalUserName(const OWPL_LINE hLine, char * szLocalUserName, int * nBuffer);

/**
 * owplLineRegister
 *
 * register a virtual line
 *
 * @param  hLine	   handle to the virtual line
 * @param  bRegister   1: Register the line; 0: Unregister the line
 * @return             OWPL_RESULT_SUCCESS in case of success
 */
MY_DLLEXPORT OWPL_RESULT
owplLineRegister(const OWPL_LINE hLine, const int bRegister);

/**
 * owplLineSetOpts
 *
 * Set optional parammetters for a line
 *
 * @param  hLine		Handle to the virtual line
 * @param  Opt			The option to set
 * @param  Data			The pointer to the new data
 */
MY_DLLEXPORT OWPL_RESULT
owplLineSetOpts(const OWPL_LINE hLine, LineOptEnum Opt, const void *Data);

MY_DLLEXPORT OWPL_RESULT
owplLineGetOpts(const OWPL_LINE hLine, LineOptEnum Opt, const void *OutBuff, const int BuffSize);


/**
 * Get the Line URI for the designated line handle
 *
 * @param hLine Handle to a line appearance.  Line handles are obtained by
 *        creating a line using the owplLineAdd function or by receiving
 *        a line event notification.
 * @param szBuffer Buffer to place line URL.  A NULL value will return
 *        the amount of storage needed in nActual.
 * @param nBuffer Size of szBuffer in bytes (not to exceed)
 * @param nActual Actual number of bytes written
 */
MY_DLLEXPORT OWPL_RESULT
owplLineGetUri(const OWPL_LINE hLine,
                        char*  szBuffer,
                        int * nBuffer);

/**
 * owplLineAddCredential
 *
 * Add credential information to a line
 */
MY_DLLEXPORT OWPL_RESULT
owplLineAddCredential(const OWPL_LINE hLine,
						const char* szUserID,
						const char* szPasswd,
						const char* szRealm);

/**
 * Sets the line as busy or not.
 *
 * @param	bBusy	boolean indicating the mode busy or not
 * @return	an OWPL_RESULT code
 */
MY_DLLEXPORT OWPL_RESULT
owplLineSetBusy(OWPL_LINE hLine,
				unsigned int bBusy);

/**
 * Sets the line as busy or not.
 *
 * @param	bBusy	pointer to a boolean indicating the mode busy or not
 * @return	an OWPL_RESULT code
 */
MY_DLLEXPORT OWPL_RESULT
owplLineIsBusy(OWPL_LINE hLine,
			   unsigned int * bBusy);
               
/**
 * owplLineSetAutoKeepAlive
 *
 * Get the proxy address of the line
 * 
 * @param  hLine		handle to the virtual line
 * @param  Enabled		1: Enabled, 0: Disabled
 * @param  Period		The timeout period between 2 keep alive messages (in seconds)
 * @return				OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplLineSetAutoKeepAlive(OWPL_LINE hLine,
			   unsigned int Enabled,
			   unsigned int Period);

/********************************************************************************************
 *								Phone CALL related functions								*
 ********************************************************************************************/

/**
 * owplCallCreate
 *
 * Create a call object
 */
MY_DLLEXPORT OWPL_RESULT
owplCallCreate( const OWPL_LINE hLine,
				 OWPL_CALL * hCall);
/**
 * Connect an already created call
 *
 * @param hCall				The handle to the created call.
 * @param szAddress			The SIP URI to call
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallConnect(const OWPL_CALL hCall,
				const char* szAddress,
				int mediaStreams);

/**
 * Connect an already created call with a custom body
 *
 * @param hCall				The handle to the created call.
 * @param szAddress			The SIP URI to call
 * @param szContentType		The content type of the message
 * @param body				The body of the message
 * @param BodySize			The size of the body of the message
 */
MY_DLLEXPORT OWPL_RESULT
owplCallConnectWithBody(const OWPL_CALL hCall,
				const char* szAddress,
				const char* szContentType,
				const char* szBody,
				int BodySize);

/**
 * Accept an incoming call. The message 180 RINGING is sent *ONLY* after the call to this function
 *
 * @param hCall				The handle to the incoming call.
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAccept(const OWPL_CALL hCall, int mediaStreams);

/**
 * Answer an incoming call
 *
 * @param hCall				The handle to the incoming call.
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAnswer(const OWPL_CALL hCall, int mediaStreams);

/**
 * Answer an incoming call with the specified body
 *
 * @param hCall				The handle to the incoming call.
 * @param szContentType		The content type of the message
 * @param body				The body of the message
 * @param BodySize			The size of the body of the message
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAnswerWithBody (const OWPL_CALL hCall,
						const char* szContentType,
						const char* szBody,
						int BodySize);

/**
 * Reject an incoming call
 *
 * @param hCall				The handle to the incoming call.
 * @param errorCode			The SIP error code (eg. 486 - Busy Here, 488 - Not acceptable here)
 * @param szErrorText		The text that will be sent in SIP response with the error code
 */
MY_DLLEXPORT OWPL_RESULT
owplCallReject(const OWPL_CALL hCall,
				const int errorCode,
				const char* szErrorText);

/**
 * Reject an incoming call with a predefined reason
 *
 * @param hCall				The handle to the incoming call.
 * @param Reason			The predefined reason
 */
MY_DLLEXPORT OWPL_RESULT
owplCallRejectWithPredefinedReason(const OWPL_CALL hCall,
				OWPL_CALL_REFUSED_REASON Reason);


///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Placed the specified call on hold.
 *
 * If the ContentType of the current call is *NOT* SDP, the message INVITE will contain an
 * EVENT field with the value "ONHOLD"
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT
owplCallHold(const OWPL_CALL hCall);

///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Placed the specified call on hold. The INVITE message will be sent with the specified Body and
 * ContentType
 * If the ContentType is *NOT* SDP, the message INVITE will contain an EVENT field with the value "ONHOLD"
 *
 * @param hCall			Handle to a call.  Call handles are obtained either by
 *						invoking owplCallCreate or passed to your application through
 *						a listener interface.
 * @param szContentType The content type for the INVITE
 * @param szBody		The content of the INVITE message
 * @param BodySize		The size of the body
 */
MY_DLLEXPORT OWPL_RESULT
owplCallHoldWithBody(const OWPL_CALL hCall,
					 const char * szContentType,
					 const char * szBody,
					 int BodySize);

/**
 * Take the specified call off hold.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT owplCallUnhold(const OWPL_CALL hCall);

/**
 * Take the specified call off hold. The INVITE message will be sent with the specified Body and
 * ContentType
 * If the ContentType is *NOT* SDP, the message INVITE will contain an EVENT field with the value "RESUME"
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT owplCallUnholdWithBody(const OWPL_CALL hCall,
										const char * szContentType,
										const char * szBody,
										int BodySize);


/**
 * Drop/Destroy the specified call.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking sipxCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT owplCallDisconnect(OWPL_CALL hCall);


/**
 * Check whether the audio flow of a call is crypted or not
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param EncryptionMode:	Contain 1 if the call is crypted, 0 otherwise
 *
 */
MY_DLLEXPORT OWPL_RESULT owplCallGetEncryptionMode(const OWPL_CALL hCall,
                                           int * EncryptionMode);

/**
 * Get the SIP identity of the local connection.  The identity represents
 * either 1) who was called in the case of a inbound call, or 2) the
 * line identity used in an outbound call.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param szId Buffer to store the ID.  A zero-terminated string will be
 *        copied into this buffer on success.
 * @param iMaxLength Max length of the ID buffer.
 */
MY_DLLEXPORT OWPL_RESULT owplCallGetLocalID(const OWPL_CALL hCall,
                                            char* szId,
                                            const size_t iMaxLength) ;


/**
 * Get the SIP identity of the remote connection.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param szId Buffer to store the ID.  A zero-terminated string will be
 *        copied into this buffer on success.
 * @param iMaxLength Max length of the ID buffer.
 */
MY_DLLEXPORT OWPL_RESULT owplCallGetRemoteID(const OWPL_CALL hCall,
                                             char* szId,
                                             const size_t iMaxLength) ;

/**
 * Set a new asserted identity for the local connection
 *
 * This function is used to change the identity of the local connection
 * used in the SIP signalling for the given call.  This operation is
 * useful in applications like B2BUA where the local identity changes
 * (e.g. as a result of a local operation like bridging or transfer not
 * exposed to the signalling in this call.  From a SIP signalling
 * perspective this sets the PAssertedIdentity header field (RFC 3325).
 *
 * @param hCall the call in which the local identity or caller id is to be
 *        changed
 * @param szPAssertedId the new SIP identity to be user for the local
 *        connection in the given call.
 * @param bSignalNow signal the identity change now.  true causes a
 *        SIP reINVITE now, false will signal the identity change with
 *        the next on or off hold reINVITE
 */
MY_DLLEXPORT OWPL_RESULT owplCallSetAssertedId(const OWPL_CALL hCall,
                                               const char* szPAssertedId,
                                               const int bSignalNow);

/**
 * Get the SIP remote contact.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param szContact Buffer to store the remote contact.  A zero-terminated string will be
 *        copied into this buffer on success.
 * @param iMaxLength Max length of the remote contact buffer.
 */
MY_DLLEXPORT OWPL_RESULT owplCallGetRemoteContact(const OWPL_CALL hCall,
                                                  char* szContact,
                                                  const size_t iMaxLength) ;


/**
 * Play a tone (DTMF, dialtone, ring back, etc) to the local and/or
 * remote party.  See the DTMF_ constants for built-in tones.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param toneId ID of the tone to play
 * @param bLocal Should the tone be played locally?
 * @param bRemote Should the tone be played to the remote party?
 */
MY_DLLEXPORT OWPL_RESULT owplCallToneStart(const OWPL_CALL hCall,
                                           const OWPL_TONE_ID toneId,
                                           const int bLocal,
                                           const int bRemote);

/**
 * Stop playing a tone (DTMF, dialtone, ring back, etc). to local
 * and remote parties.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking sipxCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT owplCallToneStop(const OWPL_CALL hCall);

/**
 * Play a tone (DTMF, dialtone, ring back, etc). to local and remote parties
 * during a specified duration
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 */
MY_DLLEXPORT OWPL_RESULT owplCallTonePlay(const OWPL_CALL hCall, unsigned long Miliseconds) ;


/**
 * Play the designed file.  The file may be a raw 16 bit signed PCM at
 * 8000 samples/sec, mono, little endian or a .WAV file.
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.  Audio files can only be played in the
 *        context of a call.
 * @param szFile Filename for the audio file to be played.
 * @param bRepeat True if the file is supposed to be played repeatedly
 * @param bLocal True if the audio file is to be rendered locally.
 * @param bRemote True if the audio file is to be rendered by the remote
 *                endpoint.
 */
MY_DLLEXPORT OWPL_RESULT owplCallAudioPlayFileStart(const OWPL_CALL hCall,
                                               const char* szFile,
                                               const int bRepeat,
                                               const int bLocal,
                                               const int bRemote) ;

/**
 * Stop playing a file started with owplCallAudioPlayFileStart
 *
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking sipxCallCreate or passed to your application through
 *        a listener interface.  Audio files can only be played and stopped
 *        in the context of a call.
 */
MY_DLLEXPORT OWPL_RESULT owplCallAudioPlayFileStop(const OWPL_CALL hCall) ;

/**
 * Sends an INFO event to the other end-point(s) on a Call.
 *
 * @param phInfo Pointer to an INFO message handle, whose value is set by this method.
 * @param hCall Handle to a call.  Call handles are obtained either by
 *        invoking owplCallCreate or passed to your application through
 *        a listener interface.
 * @param szContentType String representation of the INFO content type
 * @param szContent Pointer to the INFO messasge's content
 * @param nContentLength Size of the INFO content
 */
MY_DLLEXPORT OWPL_RESULT owplCallSendInfo(const OWPL_CALL hCall,
                                          const char* szContentType,
                                          const char* szContent,
                                          const int nContentLength);

/********************************************************************************************
 *								Presence related functions								*
 ********************************************************************************************/

/**
 * Sends a SUBSCRIBE message to a sip URI.
 *
 * @param hLine		The handle to the line.
 * @param szUri		Sip address of the person to subscribe to
 * @param winfo		0: Subscribe for presence
					1: Subscribe for watcher info
 * @param hSub		The handle to this subscription

 */
MY_DLLEXPORT OWPL_RESULT owplPresenceSubscribe(OWPL_LINE  hLine,
                                          const char* szUri,
                                          const int winfo,
										  OWPL_SUB *hSub);

/**
 * Unsubscribes to presence of the SUBSCRIBE of id hSub
 *
 * @param	hSub	the SIP URI from which we want to unsubscribe
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplPresenceUnsubscribe(const char * szRemoteUri);

/**
 * Sends a PUBLISH message to change the presence of the current user.
 *
 * @param hLine			The handle to the line.
 * @param Online		0: Offline
						1: Online
 * @param szStatus		The status message if Online is 1
 * @param hPub			The handle to this publish
 */
MY_DLLEXPORT OWPL_RESULT owplPresencePublish(OWPL_LINE  hLine,
                                          const int Online,
                                          const char * szStatus,
										  OWPL_PUB *hPub);

/********************************************************************************************
 *								Message related functions								*
 ********************************************************************************************/

/**
 * Sends a custom message
 *
 * @param	hLine	a virtual line handle
 * @param	szRemoteUri	a remote uri "sip:user@domain"
 * @param	szContent	the content of the message
 * @param	szMIME	the MIME type "content_type/sub_content_type"
 * @param	messageId	a buffer receiving the id of the sent message
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplMessageSend(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szContent,
				const char * szMIME, 
				int * messageId);

/**
 * Sends a plain text message (chat purpose)
 *
 * @param	hLine	a virtual line handle
 * @param	szRemoteUri	a remote uri "sip:user@domain"
 * @param	szContent	the content of the message
 * @param	messageId	a buffer receiving the id of the sent message
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplMessageSendPlainText(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szContent, 
				int * messageId);

/**
 * Sends a message to inform the other party of the typing state
 *
 * @param	hLine	a virtual line handle
 * @param	szRemoteUri	a remote uri "sip:user@domain"
 * @param	state	the typing state
 * @param	messageId	a buffer receiving the id of the sent message
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplMessageSendTypingState(OWPL_LINE hLine,
				const char * szRemoteUri,
				OWPL_TYPING_STATE state, 
				int * messageId);

/**
 * Sends a message to inform the other party that the user changed his icon
 *
 * @param	hLine	a virtual line handle
 * @param	szRemoteUri	a remote uri "sip:user@domain"
 * @param	szIconFileName	the icon file name
 * @param	messageId	a buffer receiving the id of the sent message
 * @return	an OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplMessageSendIcon(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szIconFileName, 
				int * messageId);


#ifdef __cplusplus
}
#endif

#endif  //_PH_NEW_API_HEADER_
