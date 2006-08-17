/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef __SFP_PLUGIN_H__
#define __SFP_PLUGIN_H__

#ifdef WIN32
#if defined(BUILD_SFP_PLUGIN_DLL)
#define SFP_PLUGIN_EXPORTS __declspec(dllexport) 
#elif defined(USE_SFP_PLUGIN_DLL)
#define SFP_PLUGIN_EXPORTS __declspec(dllimport) 
#endif
#endif

#ifndef SFP_PLUGIN_EXPORTS
#define SFP_PLUGIN_EXPORTS 
#endif 

#include <sfp-plugin/sfp-error.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <phapi-util/phapi-globals.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/**
	* An enumeration of the states of a session
	*
	* TODO pause / resume
	*/
	typedef enum sfp_session_state {
		SFP_SESSION_INITIATED,
		SFP_SESSION_RUNNING,
		SFP_SESSION_CANCELLED,
		SFP_SESSION_CANCELLED_BY_PEER,
		SFP_SESSION_PAUSED,
		SFP_SESSION_RESUMED,
		SFP_SESSION_RECEIVED_COMPLETE,
		SFP_SESSION_RECEIVED_INCOMPLETE,
		SFP_SESSION_SENT_INCOMPLETE,
		SFP_SESSION_SENT_COMPLETE,
		SFP_SESSION_CLOSED_BY_PEER,
		SFP_SESSION_FINISHED
	} sfp_session_state_t;

	typedef struct sfp_session_info_s sfp_session_info_t;

	/**
	* Structure to store all the information about a file transfer
	*/
	typedef struct sfp_session_info_s{
		char * session_id;
		char * local_username;
		char * local_mode; // if both are natted i.e. bridge info is filled, mode must be active for both
		char * local_ip_address_type;
		char * local_ip;
		char * local_port;
		char * remote_username;
		char * remote_ip_address_type;
		char * remote_ip;
		char * remote_port;
		char * ip_protocol;
		char * required_bandwidth; /* bytes per second */
		char * packet_size; /* bytes */ // TODO rename to udp_packet_size
		char * key_info;
		char * crypted_key;
		char * filename;
		char * short_filename;
		char * file_type;
		char * file_size; /* bytes */
		// TODO uri?
		struct sockaddr_in local_address;
		SOCKET local_socket;
		sfp_session_state_t state;
		int call_id;
		void (*terminaisonCallback)(sfp_session_info_t * session, sfp_returncode_t code);
		void (*progressionCallback)(sfp_session_info_t * session, int percentage);
	};

	/**
	* Callbacks for the plugin to notify a top level program
	*/
	struct sfp_callbacks{
		void (*inviteToTransfer)(int cid, char * uri, char * short_filename, char * file_type, char * file_size);
		void (*newIncomingFile)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*waitingForAnswer)(int cid, char * uri);
		void (*transferCancelled)(int cid, char * short_filename, char * file_type, char * file_size);
		void (*transferCancelledByPeer)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*sendingFileBegin)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferClosedByPeer)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferClosed)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferFromPeerFinished)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferToPeerFinished)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferFromPeerFailed)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferToPeerFailed)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferFromPeerStopped)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferToPeerStopped)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferProgression)(int cid, int percentage);
		void (*transferPausedByPeer)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferPaused)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferResumedByPeer)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
		void (*transferResumed)(int cid, char * username, char * short_filename, char * file_type, char * file_size);
	};
	typedef struct sfp_callbacks sfp_callbacks_t;

	/************* PLUGIN SERVICES *************/

	/**
	* Registers the plugin into PhApi.
	*
	* @return	TRUE if the registering succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS unsigned int sfp_register_plugin();

	/**
	* Unregisters the plugin from PhApi.
	*
	* @return	TRUE if the registering succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS unsigned int sfp_unregister_plugin();

	/**
	* Adds callbacks to the plugin, so that it can notify a top level program of some events
	*
	* @param	[in]	cbks : the callbacks
	*/
	SFP_PLUGIN_EXPORTS void sfp_set_plugin_callbacks(sfp_callbacks_t * cbks);

	/**
	* Sends a file (send an INVITE to the transfer)
	*
	* @param	[in]	vlid : a virtual line id
	* @param	[in]	userdata : ?
	* @param	[in]	filename : the "full" file name (the whole path) to send
	* @param	[in]	short_filename : the "short" file name (without the path) to send
	* @param	[in]	file_type : the file type
	* @param	[in]	file_size : the exact file size in bytes
	* @return	TRUE if the transfer initiation succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS int sfp_send_file(int vlid, void *userdata, char * uri, char * filename, char * short_filename, char * file_type, char * file_size);

	/**
	* Sets the base file transfer port
	*
	* @param	[in]	port : the base file transfer port
	*/
	SFP_PLUGIN_EXPORTS void sfp_set_file_transfer_port(const char * port);

	/**
	* Sets the protocol to use
	*
	* @param	[in]	protocol : the protocol to use, can be SFP_IP_PROTOCOL_TCP or SFP_IP_PROTOCOL_UDP
	* @param	[in]	packet_size : the max packet size for UDP transfers (in bytes)
	*/
	SFP_PLUGIN_EXPORTS void sfp_set_protocol(const char * protocol, char * packet_size);

	/**
	* Receives a file (sends a 200OK and starts a thread to receive the file)
	*
	* @param	[in]	cid : the call id
	* @param	[in]	filename : the "full" file name (the whole path) where to write what has been received
	* @return	TRUE if succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS int sfp_receive_file(int cid, const char * filename);

	/**
	* Cancels a file transfer 
	* (sends a CANCEL if still in the INVITE phase and that we are the sender)
	* (sends a DECLINE if still in the INVITE phase and that we are the receiver)
	* (tells the transfer thread to stop if already transferring)
	*
	* @param	[in]	call_id : the call id
	* @return	TRUE if succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS int sfp_cancel_transfer(int call_id);

	/**
	* Pauses a file transfer 
	* (sends an INVITE containing "holdon")
	*
	* @param	[in]	call_id : the call id
	* @return	TRUE if succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS int sfp_pause_transfer(int call_id);

	/**
	* Resumes a file transfer 
	* (sends an INVITE containing "holdoff"))
	*
	* @param	[in]	call_id : the call id
	* @return	TRUE if succeeded; FALSE else
	*/
	SFP_PLUGIN_EXPORTS int sfp_resume_transfer(int call_id);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __SFP_PLUGIN_H__ */

// </ncouturier>