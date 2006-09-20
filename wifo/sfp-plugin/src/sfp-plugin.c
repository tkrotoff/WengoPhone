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

/*
* TODO
*
* qu'arrive t'il sur les REINVITE?
* change required_bandwith to max_bandwidth_use
* pause et resume
* trouver une solution pour virer le call_id de la session
* unstore the session infos on call closed
* suppress the created file on CANCEL
*/

#include <sfp-plugin/sfp-plugin.h>
#include <sfp-plugin/sfp-error.h>
#include <sfp-plugin/sfp-parser.h>
#include <sfp-plugin/sfp-transfer.h>

#include "phapi.h"
#include "phplugin.h"

#include <phapi-util/util.h>
#include <phapi-util/mappinglist.h>
#include <phapi-util/phapi-globals.h>
#include <phapi-util/mystdio.h>
#include <phapi-util/mystring.h>

#include <pthread.h>

#include <eXosip/eXosip.h>

#include <stdlib.h>
#include <time.h>

// ----- CONSTANTS -----

#define SFP_INCOMING_INFO		1
#define SFP_OUTGOING_INFO		2

#define SFP_SENDER				1
#define SFP_RECEIVER			2

// ----- GLOBALS -----
/**
* A mapping list used to keep track of the file transfer sessions
*/
mappinglist_t * sfp_sessions_by_call_ids = NULL;

/**
* The base transfer port, from which an available port is used
* TODO dynamic usable port discovering?
* TODO rename to sfp_base_transfer_port
*/
static char * sfp_file_transfer_port = NULL;

/**
* Default IP protocol to use when sending an INVITE to send a file
*/
static char * sfp_default_ip_protocol = NULL;

/**
* Default UDP packet size to use when sending an INVITE to send a file
* TODO rename to sfp_default_udp_packet_size
*/
static char * sfp_default_packet_size = NULL;

/**
* Used for singleton (only one instance of the plugin at a time
*/
static phplugin_t * plugin = NULL;

/**
* Callbacks used by the plugin and given by the top level program
*/
static sfp_callbacks_t * sfp_cbks = NULL;
// -----

// ----- PRIVATE FUNCTIONS DECLARATION -----
static void sfp_on_EXOSIP_CALL_NEW(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_ANSWERED(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_PROCEEDING(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_RINGING(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_REQUESTFAILURE(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_SERVERFAILURE(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_GLOBALFAILURE(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_NOANSWER(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_CLOSED(eXosip_event_t * event);
static void sfp_on_EXOSIP_CALL_HOLD(eXosip_event_t *);
static void sfp_on_EXOSIP_CALL_OFFHOLD(eXosip_event_t * event);
static void sfp_generate_tranfer_id(char * buffer, const size_t sizeof_buffer);
static sfp_session_info_t * create_sfp_session_info();
static void free_sfp_session_info(sfp_session_info_t ** session);
static unsigned int sfp_add_property(char ** dst, const char * src);
static unsigned int sfp_add_session_info(int call_id, sfp_session_info_t * info);
static unsigned int sfp_remove_session_info(int call_id);
static sfp_session_info_t * sfp_get_session_info(int call_id);
static phplugin_t * sfp_get_plugin();
static void sfp_free_plugin();
static sfp_session_info_t * sfp_make_session(int vlid);
static sfp_session_info_t * sfp_make_session_for_invite(int vlid, char * filename, char * short_filename, char * file_type, char * file_size, char * bandwidth);
static sfp_session_info_t * sfp_make_session_info_from_body_info(int call_id, sfp_session_info_t * existing_session, sfp_info_t * info, const unsigned int in_or_out);
static sfp_info_t * sfp_make_body_info_from_session_info(sfp_session_info_t * session);
static void sfp_get_file_transfer_port(char buf[]);
static char * sfp_get_default_ip_protocol();
static char * sfp_get_default_packet_size();
static unsigned int sfp_can_do_tcp();
static unsigned int sfp_can_do_udp();
static void sfp_receive_terminaison(sfp_session_info_t * session, sfp_returncode_t code);
static void sfp_send_terminaison(sfp_session_info_t * session, sfp_returncode_t code);
static void sfp_progressionCallback(sfp_session_info_t * session, int percentage);
// ------

/*************** PLUGIN SERVICES ****************/

/**
* Registers the plugin into PhApi.
*
* @return	TRUE if the registering succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS unsigned int sfp_register_plugin(){
	return phplugin_register(sfp_get_plugin());
}

/**
* Unregisters the plugin from PhApi.
*
* @return	TRUE if the registering succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS unsigned int sfp_unregister_plugin(){

	// cannot unregister if there still are sessions (transfers)
	if(sfp_sessions_by_call_ids != NULL && sfp_sessions_by_call_ids->count > 0){
		return FALSE;
	}
	
	free_mappinglist(&sfp_sessions_by_call_ids);

	sfp_free_plugin(phplugin_unregister(sfp_get_plugin()));

	return TRUE;
}

/**
* Adds callbacks to the plugin, so that it can notify a top level program of some events
*
* @param	[in]	cbks : the callbacks
*/
SFP_PLUGIN_EXPORTS void sfp_set_plugin_callbacks(const sfp_callbacks_t * cbks){
	sfp_cbks = cbks;
}

/**
* Sets the base file transfer port
*
* @param	[in]	port : the base file transfer port
*/
SFP_PLUGIN_EXPORTS void sfp_set_file_transfer_port(const char * port){
	if(sfp_file_transfer_port != NULL){
		free(sfp_file_transfer_port);
	}
	sfp_file_transfer_port = strdup(port);
}

/**
* Sets the protocol to use
*
* @param	[in]	protocol : the protocol to use, can be SFP_IP_PROTOCOL_TCP or SFP_IP_PROTOCOL_UDP
* @param	[in]	packet_size : the max packet size for UDP transfers (in bytes)
*/
SFP_PLUGIN_EXPORTS void sfp_set_protocol(const char * protocol, char * packet_size){
	char * _protocol = NULL;
	char * _packet_size = NULL;

	if(strfilled(protocol) && (strequals(protocol, SFP_IP_PROTOCOL_TCP) || strequals(protocol, SFP_IP_PROTOCOL_UDP))){
		_protocol = strdup(protocol);
	}else{
		_protocol = strdup(SFP_IP_PROTOCOL_TCP); // defaults to TCP
	}
	if(strfilled(protocol) && strequals(protocol, SFP_IP_PROTOCOL_UDP)){
		if(strfilled(packet_size)){
			_packet_size = strdup(packet_size);
		}else{
			_packet_size = strdup(SFP_PACKET_SIZE_DEFAULT);
		}
	}else{
		sfp_default_packet_size = strdup("0");
	}

	// here _packet_size and _protocol are filled. no other checks necessitated
	if(sfp_default_ip_protocol != NULL){
		free(sfp_default_ip_protocol);
		sfp_default_ip_protocol = NULL;
	}
	if(sfp_default_ip_protocol == NULL){
		sfp_default_ip_protocol = _protocol;
	}
	if(sfp_default_packet_size != NULL){
		free(sfp_default_packet_size);
		sfp_default_packet_size = NULL;
	}
	if(sfp_default_packet_size == NULL){
		sfp_default_packet_size = _packet_size;
	}
}

/**
* Sends a file (send an INVITE to the transfer)
*
* @param	[in]	vlid : a virtual line id
* @param	[in]	userdata : ?
* @param	[in]	filename : the "full" file name (the whole path) to send
* @param	[in]	short_filename : the "short" file name (without the path) to send
* @param	[in]	file_type : the file type
* @param	[in]	file_size : the exact file size in bytes
* @return	the call id if the transfer initiation succeeded
*/
SFP_PLUGIN_EXPORTS int sfp_send_file(int vlid, void *userdata, char * uri, char * filename, char * short_filename, char * file_type, char * file_size){
	char * body;
	int call_id;
	sfp_session_info_t * session;
	sfp_info_t * body_info;
	int return_value;

	if((session = sfp_make_session_for_invite(vlid, filename, short_filename, file_type, file_size, SFP_REQUIRED_BANDWIDTH_DEFAULT)) == NULL){ // TODO bouchon pour la bandwidth a virer
		m_log_error("Could not create session","sfp_send_file");
		return FALSE;
	}

	// get an available file transfer port
	if(sfp_transfer_get_free_port(session) != SUCCESS){
		m_log_error("Could not find a free transfer port","sfp_send_file");
		free_sfp_session_info(&session);
		return FALSE;
	}

	if((body_info = sfp_make_body_info_from_session_info(session)) == NULL){
		m_log_error("Could not create sfp body info from session","sfp_send_file");
		// free the session
		free_sfp_session_info(&session);
		return FALSE;
	}

	body = sfp_make_message_body_from_sfp_info(body_info);
	if(!strfilled(body)){
		m_log_error("Could not create sfp body from sfp info","sfp_send_file");
		// free the body info
		sfp_free_sfp_info(&body_info);
		// free the session
		free_sfp_session_info(&session);
		return FALSE;
	}

	// we can now free the sent infos
	sfp_free_sfp_info(&body_info);

	// send the INVITE message
	return_value = phInvite(vlid, userdata, uri, "application/sfp", body, &call_id); // TODO check

	// we can now free the body
	free(body);

	// set the call_id
	session->call_id = call_id;

	sfp_add_session_info(call_id, session);

	// notify GUI
	if(sfp_cbks != NULL && sfp_cbks->inviteToTransfer) sfp_cbks->inviteToTransfer(call_id, uri, short_filename, file_type, file_size);

	return call_id;
}

/**
* Receives a file (sends a 200OK and starts a thread to receive the file)
*
* @param	[in]	cid : the call id
* @param	[in]	filename : the "full" file name (the whole path) where to write what has been received
* @return	TRUE if succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS int sfp_receive_file(int cid, const char * filename){
	sfp_info_t * to_send_info;
	char * to_send_body;
	sfp_session_info_t * session = NULL;
	pthread_t thread;

	if((session = sfp_get_session_info(cid)) == NULL){
		m_log_error("Could not retrieve the session","sfp_receive_file");
		return FALSE;
	}

	// make the sfp body info from the session
	if((to_send_info = sfp_make_body_info_from_session_info(session)) == NULL){
		m_log_error("Could not create infos to send from the session","sfp_receive_file");
		// free the session
		sfp_remove_session_info(cid);
		return FALSE; // TODO notify GUI
	}
	
	// make the sfp body to send
	if((to_send_body = sfp_make_message_body_from_sfp_info(to_send_info)) == NULL){
		m_log_error("Could not make the sfp body to send from the sfp body info","sfp_receive_file");
		// free the body info
		sfp_free_sfp_info(&to_send_info);
		// free the session
		sfp_remove_session_info(cid);
		return FALSE; // TODO notify GUI
	}

	// now that the body is done we can free the sfp body info
	sfp_free_sfp_info(&to_send_info);

	// add the place wher we want the file to be stored
	sfp_add_property(&(session->filename), filename);

	// install the terminaison callback
	session->terminaisonCallback = &sfp_receive_terminaison;

	// create the receive thread
	if(pthread_create(&thread, NULL, (void *)sfp_transfer_receive_file, (void *)session) != 0){
		// TODO free le thread
		m_log_error("Could not create receive thread", "sfp_receive_file");
		phCancel(cid);
		// TODO notify GUI
		return FALSE;
	}else{
		// notify GUI of the begining of the send
		if(sfp_cbks != NULL && sfp_cbks->receivingFileBegin) sfp_cbks->receivingFileBegin(cid, session->remote_username, session->short_filename, session->file_type, session->file_size);	
	}

	// send the accept answer
	phAccept(cid, "application/sfp", to_send_body); // TODO check

	// we can now free the body
	free(to_send_body);

	// TODO notify GUI
	return FALSE;
}	

/**
* Cancels a file transfer 
* (sends a CANCEL if still in the INVITE phase and that we are the sender)
* (sends a DECLINE if still in the INVITE phase and that we are the receiver)
* (tells the transfer thread to stop if already transferring)
*
* @param	[in]	call_id : the call id
* @return	TRUE if succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS int sfp_cancel_transfer(int call_id){
	//int res;
	sfp_session_info_t * session = NULL;

	// retrieve the session corresponding to the transfer to stop
	if((session = sfp_get_session_info(call_id)) == NULL){
		m_log_error("Could not retrieve the session","sfp_stop_transfer");
		return FALSE;
	}

	// if the session isn't yet ended
	if(session->state == SFP_SESSION_RUNNING){ // the receive / send thread is active yet; INVITE has been answered by a 200OK
		// set the session in a cancelled transfer state
		session->state = SFP_SESSION_CANCELLED;
	}else if(session->state == SFP_SESSION_INITIATED){ // the receive / send thread is not active yet; still in INVITE phase
		// send a BYE
		phBye(call_id);

		// notify the GUI
		if(sfp_cbks != NULL && sfp_cbks->transferCancelled) sfp_cbks->transferCancelled(call_id, session->short_filename, session->file_type, session->file_size);

		// remove the session
		sfp_remove_session_info(call_id);
	}

	return TRUE;
}

/**
* Pauses a file transfer 
* (sends an INVITE containing "holdon")
*
* @param	[in]	call_id : the call id
* @return	TRUE if succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS int sfp_pause_transfer(int call_id){
	sfp_session_info_t * session = NULL;

	if((session = sfp_get_session_info(call_id)) == NULL){
		m_log_error("Could not retrieve the session","sfp_pause_transfer");
		return FALSE;
	}

	if(phHoldOn(call_id, "application/sfp") == TRUE){
		session->state = SFP_SESSION_PAUSED;
		if(sfp_cbks != NULL && sfp_cbks->transferPaused) sfp_cbks->transferPaused(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);
		return TRUE;
	}

	return FALSE;
}

/**
* Resumes a file transfer 
* (sends an INVITE containing "holdoff"))
*
* @param	[in]	call_id : the call id
* @return	TRUE if succeeded; FALSE else
*/
SFP_PLUGIN_EXPORTS int sfp_resume_transfer(int call_id){
	sfp_session_info_t * session = NULL;

	if((session = sfp_get_session_info(call_id)) == NULL){
		m_log_error("Could not retrieve the session","sfp_resume_transfer");
		return FALSE;
	}

	if(phHoldOff(call_id, "application/sfp") == TRUE){
		session->state = SFP_SESSION_RESUMED;		
		return TRUE;
	}

	return FALSE;
}

/**************** SESSION INFO ****************/

/**
* Creates a new session info
*
* @return	a new allocated session info
*/
static sfp_session_info_t * create_sfp_session_info(){
	sfp_session_info_t * session = (sfp_session_info_t *)malloc(sizeof(sfp_session_info_t));
	memset(session, 0, sizeof(sfp_session_info_t));

	return session;
}

/**
* Frees a session info
* Example
* <pre>
*	sfp_session_info_t * session;
*	free_sfp_session_info(&session);
* </pre>
*
* @param	[in][out]	session : the session info to free
*/
static void free_sfp_session_info(sfp_session_info_t ** session){

	if(!session || !*session) return;

	free((*session)->session_id);
	free((*session)->local_username);
	free((*session)->local_mode);
	free((*session)->local_ip_address_type);
	free((*session)->local_ip);
	free((*session)->local_port);
	free((*session)->remote_username);
	free((*session)->remote_ip_address_type);
	free((*session)->remote_ip);
	free((*session)->remote_port);
	free((*session)->ip_protocol);
	free((*session)->required_bandwidth);
	free((*session)->packet_size);
	free((*session)->key_info);
	free((*session)->crypted_key);
	free((*session)->filename);
	free((*session)->short_filename);
	free((*session)->file_type);
	free((*session)->file_size);
	free(*session);
	*session = NULL;
}

/**
* Adds cleanly textual properties to a sfp sessiçon info
*
* @param	[in][out]	dst : the property receiving the new value
* @param	[in]	src : the new value to be set
* @return	TRUE if the new value could be added; FALSE else
*/
static unsigned int sfp_add_property(char ** dst, const char * src){
	if(strfilled(src)){
		if(strfilled(*dst)){
			free(*dst);
		}
		*dst = strdup(src);
		return TRUE;
	}
	return FALSE;
}

/************ SESSION INFO STORAGE ************/

/**
* Adds a session to the list of created sessions, and associates it to the corresponding call_id
*
* @param	[in]	call_id : the call id
* @param	[in]	info : the session to store
* @return	TRUE if the new session could be added; FALSE else
*/
static unsigned int sfp_add_session_info(int call_id, sfp_session_info_t * info){
	if(sfp_sessions_by_call_ids == NULL){
		sfp_sessions_by_call_ids = create_mappinglist();
	}
	return mappinglist_put_with_int_key(sfp_sessions_by_call_ids, call_id, info);
}

/**
* Removes the session associated to the given call_id
*
* @param	[in]	call_id : the call id
* @return	TRUE if the session could be removed; FALSE else
*/
static unsigned int sfp_remove_session_info(int call_id){
	sfp_session_info_t * session;
	if((session = mappinglist_remove_with_int_key(sfp_sessions_by_call_ids, call_id)) != NULL){
		free_sfp_session_info(&session);
		return TRUE;
	}
	return FALSE;
}

/**
* Retrieves the session associated to the given call_id
*
* @param	[in]	call_id : the call id
* @return	the session info associated to the given call_id; NULL else
*/
static sfp_session_info_t * sfp_get_session_info(int call_id){
	return (sfp_session_info_t *)mappinglist_get_with_int_key(sfp_sessions_by_call_ids, call_id);
}
/************* PLUGIN MANIPULATION ***********/

/**
* Creates a new plugin it it hasn't been yet instanciated,
* or returns the one that already exists
*
* @return	a new initialized sfp plugin
*/
static phplugin_t * sfp_get_plugin(){
	if(plugin == NULL){
		plugin = (phplugin_t *)malloc(sizeof(phplugin_t));
		memset(plugin, 0, sizeof(phplugin_t));

		strncpy(plugin->content_type.type, "application", sizeof(plugin->content_type.type));
		strncpy(plugin->content_type.subtype, "sfp", sizeof(plugin->content_type.subtype));

		plugin->plg_callbacks.on_EXOSIP_CALL_ANSWERED				=	&sfp_on_EXOSIP_CALL_ANSWERED;
		plugin->plg_callbacks.on_EXOSIP_CALL_CLOSED					=	&sfp_on_EXOSIP_CALL_CLOSED;
		plugin->plg_callbacks.on_EXOSIP_CALL_GLOBALFAILURE			=	&sfp_on_EXOSIP_CALL_GLOBALFAILURE;
		plugin->plg_callbacks.on_EXOSIP_CALL_NEW					=	&sfp_on_EXOSIP_CALL_NEW;
		plugin->plg_callbacks.on_EXOSIP_CALL_NOANSWER				=	&sfp_on_EXOSIP_CALL_NOANSWER;
		plugin->plg_callbacks.on_EXOSIP_CALL_PROCEEDING				=	&sfp_on_EXOSIP_CALL_PROCEEDING;
		plugin->plg_callbacks.on_EXOSIP_CALL_REQUESTFAILURE			=	&sfp_on_EXOSIP_CALL_REQUESTFAILURE;
		plugin->plg_callbacks.on_EXOSIP_CALL_RINGING				=	&sfp_on_EXOSIP_CALL_RINGING;
		plugin->plg_callbacks.on_EXOSIP_CALL_SERVERFAILURE			=	&sfp_on_EXOSIP_CALL_SERVERFAILURE;
		plugin->plg_callbacks.on_EXOSIP_CALL_HOLD					=	&sfp_on_EXOSIP_CALL_HOLD;
		plugin->plg_callbacks.on_EXOSIP_CALL_OFFHOLD				=	&sfp_on_EXOSIP_CALL_OFFHOLD;
	}

	return plugin;
}

/**
* Frees a plugin
*/
static void sfp_free_plugin(){
	if(plugin != NULL){
		memset(plugin, 0, sizeof(phplugin_t));
	}

	free(sfp_file_transfer_port);
	free(sfp_default_ip_protocol);
	free(sfp_default_packet_size);

	free(plugin);

	plugin = NULL;
}

/**
* Generates a "supposed unique" random id
*
* @param	[out]	buffer : a "supposed unique" random id
*/
static void sfp_generate_tranfer_id(char * buffer, const size_t sizeof_buffer){ // TODO rename to generate_session_id
	int radix = 10;
	//char * buffer = (char *)malloc(sizeof(int)*8+1); // for radix = 2

	srand((unsigned)time(NULL));

	itostr(rand(), buffer, sizeof_buffer, radix);

	//return buffer;
}

/**
* Makes a session with every info available locally
* Not for use on creation of a session to do an INVITE, use sfp_make_session_for_invite(...) instead.
*
* @param	[in]	vlid : a virtual line id
* @return	a new session initialized with every info available locally
*/
static sfp_session_info_t * sfp_make_session(int vlid){
	char * username;
	char port[16];
	char local_ip[16];

	sfp_session_info_t * session = NULL;

	username = ph_get_username(vlid);
	ph_get_local_ip(local_ip);
	sfp_get_file_transfer_port(port);

	if((session = create_sfp_session_info()) == NULL){
		m_log_error("Could not create sfp_session_info_t", "sfp_make_session");
		return NULL;
	}

	sfp_add_property(&(session->local_username), username);
	sfp_add_property(&(session->local_ip_address_type), SFP_ADDRESS_TYPE_IPV4); // TODO fixed for the moment
	sfp_add_property(&(session->local_ip), local_ip);
	sfp_add_property(&(session->local_port), port);
	//sfp_add_property(&(session->local_mode), SFP_MODE_PASSIVE); // mode defaults to passive
	sfp_add_property(&(session->local_mode), SFP_MODE_ACTIVE); // mode defaults to active
	sfp_add_property(&(session->ip_protocol), sfp_get_default_ip_protocol());
	sfp_add_property(&(session->packet_size), sfp_get_default_packet_size());
	session->state = SFP_SESSION_INITIATED;
	session->progressionCallback = &sfp_progressionCallback;

	return session;
}
/**
* Creates a file transfer session to use before an INVITE, adding all the file information
*
* @param	[in]	vlid : a virtual line id
* @param	[in]	filename : the "full" file name (the whole path)
* @param	[in]	short_filename : the "short" file name (without the path)
* @param	[in]	file_type : the file type
* @param	[in]	file_size : the file size
* @param	[in]	bandwidth : the required bandwidth for the transfer
* @return	a session info that can be used to make an INVITE
*/
static sfp_session_info_t * sfp_make_session_for_invite(int vlid, char * filename, char * short_filename, char * file_type, char * file_size, char * bandwidth){
	char file_sending_id[12];
	char connection_id[16];
	sfp_session_info_t * session = NULL;

	// args check
	if(!strfilled(filename)) return NULL;
	if(!strfilled(short_filename)) return NULL;
	if(!strfilled(file_type)) return NULL;
	if(!strfilled(file_size)) return NULL;

	//username = ph_get_username(vlid);
	//sfp_get_file_transfer_port(port);
	sfp_generate_tranfer_id(file_sending_id, sizeof(file_sending_id));

	if((session = sfp_make_session(vlid)) == NULL){
		m_log_error("Could not create sfp_session_info_t", "sfp_make_session");
		return NULL;
	}

	/* JULIEN */
	sfp_generate_tranfer_id(connection_id, sizeof(connection_id));
	sfp_add_property(&(session->connection_id), connection_id);
	/* ****** */

	sfp_add_property(&(session->session_id), file_sending_id);
	if(strfilled(bandwidth)){
		sfp_add_property(&(session->required_bandwidth), bandwidth);
	}else{
		sfp_add_property(&(session->required_bandwidth), SFP_REQUIRED_BANDWIDTH_DEFAULT);
	}
	sfp_add_property(&(session->filename), filename);
	sfp_add_property(&(session->short_filename), short_filename);
	sfp_add_property(&(session->file_type), file_type);
	sfp_add_property(&(session->file_size), file_size);
	
	return session;
}
/**
* Extracs information useful for a session from a sfp body info.
* Can be used to get info from incoming messages
*
* @param	[in]	call_id : the call_id
* @param	[in][out]	existing_session : a session inf if yet existing
* @param	[in]	info : the info extracted from an SFP body
* @param	[in]	in_or_out : flag indicating wether we are in the case of an incoming or outgoing case, can be SFP_INCOMING_INFO or SFP_OUTGOING_INFO
* @return	a session info containing the proper extracted infos from and sfp body info
*/
static sfp_session_info_t * sfp_make_session_info_from_body_info(int call_id, sfp_session_info_t * existing_session, sfp_info_t * info, const unsigned int in_or_out){
	sfp_session_info_t * session = NULL;

	// try to retrieve an existing session (if exists)
	if(existing_session != NULL){
		session = existing_session;
	}else if( (call_id <= 0) || (call_id > 0 && (session = sfp_get_session_info(call_id)) == NULL) ){
		// else create one
		session = create_sfp_session_info();
	}
	// then if we have a session, fill it
	if(session != NULL && in_or_out == SFP_INCOMING_INFO){ // we are the receiver of the SFP message

		// SESSION ID // TODO necessary ?
		if(strfilled(info->file_sending_id) && !strfilled(session->session_id)){
			sfp_add_property(&(session->session_id), info->file_sending_id);
		}

		// PEER
		if(strfilled(info->username) && !strfilled(session->remote_username))
			sfp_add_property(&(session->remote_username), info->username);

		if (strfilled(info->ip_address) && !strfilled(session->remote_ip)){
			sfp_add_property(&(session->remote_ip), info->ip_address);
		}

		if(strfilled(info->ip_address_type) && !strfilled(session->remote_ip_address_type)){
			sfp_add_property(&(session->remote_ip_address_type), info->ip_address_type);
		}

		if(strfilled(info->address_port) && !strfilled(session->remote_port)){
			sfp_add_property(&(session->remote_port), info->address_port);
		}

		// NETWORK REQUIREMENTS
		// on receive of INVITE fill in what is possible considering the INVITE protocol request
		// on receive of 200OK must accept protocol or if can't, must send a CANCEL
		if(strfilled(info->ip_protocol) && strfilled(session->ip_protocol)){ // receive of 200OK case
			if(strequals(info->ip_protocol, SFP_IP_PROTOCOL_TCP)){
				if(sfp_can_do_tcp()){
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_TCP);
				}else{
					// TODO send a cancel + nettoyage
					phCancel(call_id);
				}
			}else if(strequals(info->ip_protocol, SFP_IP_PROTOCOL_UDP)){
				if(sfp_can_do_udp()){
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_UDP);
				}else{
					// TODO send a cancel + nettoyage
					phCancel(call_id);
				}
			}
		}else if(strfilled(info->ip_protocol) && !strfilled(session->ip_protocol)){ // receive of INVITE case
			if(strequals(info->ip_protocol, SFP_IP_PROTOCOL_TCP)){
				if(sfp_can_do_tcp()){
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_TCP);
				}else{
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_UDP);
				}
			}else if(strequals(info->ip_protocol, SFP_IP_PROTOCOL_UDP)){
				if(sfp_can_do_udp()){
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_UDP);
				}else{
					sfp_add_property(&(session->ip_protocol), SFP_IP_PROTOCOL_TCP);
				}
			}
		}

		if(strfilled(info->required_bandwidth) && !strfilled(session->required_bandwidth)){
			sfp_add_property(&(session->required_bandwidth), info->required_bandwidth);
		}else if(strfilled(info->required_bandwidth) && strfilled(session->required_bandwidth)){
			// TODO change to min because required_bandwith will become max_bandwidth_use
			// take the max
			sfp_add_property(&(session->required_bandwidth), str_int_max(session->required_bandwidth, info->required_bandwidth));
		}

		if(strfilled(info->packet_size) && !strfilled(session->packet_size)){
			sfp_add_property(&(session->packet_size), info->packet_size);
		}else if(strfilled(info->packet_size) && strfilled(session->packet_size)){
			// take the min
			sfp_add_property(&(session->packet_size), str_int_min(session->packet_size, info->packet_size));
		}

		// FILE CONCERNS
		if(strfilled(info->filename) && !strfilled(session->short_filename))
			sfp_add_property(&(session->short_filename), info->filename);

		if(strfilled(info->file_type) && !strfilled(session->file_type))
			sfp_add_property(&(session->file_type), info->file_type);

		if(strfilled(info->file_size) && !strfilled(session->file_size))
			sfp_add_property(&(session->file_size), info->file_size);

		// MODE
		if(strfilled(info->mode)){
			if(strequals(info->mode, SFP_MODE_ACTIVE)){
				sfp_add_property(&(session->local_mode), SFP_MODE_PASSIVE);
			}else if(strequals(info->mode, SFP_MODE_PASSIVE)){
				sfp_add_property(&(session->local_mode), SFP_MODE_ACTIVE);
			}
		}

		// CONNECTION_ID
		if(strfilled(info->connection_id))
			sfp_add_property(&(session->connection_id), info->connection_id);
		

		// TODO key and uri
	}
	else if(session != NULL && in_or_out == SFP_OUTGOING_INFO){ // we are the sender of the SFP message

		// LOCAL INFO
		if(strfilled(info->username) && !strfilled(session->local_username))
			sfp_add_property(&(session->local_username), info->username);

		if(strfilled(info->ip_address_type) && !strfilled(session->local_ip_address_type))
			sfp_add_property(&(session->local_ip_address_type), info->ip_address_type);

		if(strfilled(info->ip_address) && !strfilled(session->local_ip))
			sfp_add_property(&(session->local_ip), info->ip_address);

		if(strfilled(info->address_port) && !strfilled(session->local_port))
			sfp_add_property(&(session->local_port), info->address_port);

		// NETWORK REQUIREMENTS
		// for sending INVITE set what the top level program gave
		// for sending 200OK it's been done when on receiving INVITE, non need to do anything more
		if(strfilled(info->ip_protocol) && !strfilled(session->ip_protocol)) // sending INVITE case (set by top level program)
			sfp_add_property(&(session->ip_protocol), info->ip_protocol);

		if(strfilled(info->required_bandwidth) && !strfilled(session->required_bandwidth)){ // sending INVITE case
			sfp_add_property(&(session->required_bandwidth), info->required_bandwidth);
		}else if(strfilled(info->required_bandwidth) && strfilled(session->required_bandwidth)){ // sending 200OK case
			// TODO change to min because required_bandwith will become max_bandwidth_use
			// take the max
			sfp_add_property(&(session->required_bandwidth), str_int_max(session->required_bandwidth, info->required_bandwidth));
		}

		if(strfilled(info->packet_size) && !strfilled(session->packet_size)){ // sending INVITE case
			sfp_add_property(&(session->packet_size), info->packet_size);
		}else if(strfilled(info->packet_size) && strfilled(session->packet_size)){ // sending 200OK case
			// take the min
			sfp_add_property(&(session->packet_size), str_int_min(session->packet_size, info->packet_size));
		}

		// FILE CONCERNS
		if(strfilled(info->filename) && !strfilled(session->short_filename))
			sfp_add_property(&(session->short_filename), info->filename);

		if(strfilled(info->file_type) && !strfilled(session->file_type))
			sfp_add_property(&(session->file_type), info->file_type);

		if(strfilled(info->file_size) && !strfilled(session->file_size))
			sfp_add_property(&(session->file_size), info->file_size);

		// CONNECTION_ID
		if(strfilled(info->connection_id))
			sfp_add_property(&(session->connection_id), info->connection_id);		

		// TODO key and uri
	}

	return session;
}


/**
* Extracs information useful for a sfp body info from a session.
* Can be used to make outgoing messages.
*
* @param	[in]	session : a session info
* @return	a sfp body info ready to be used for an outgoing message
*/
static sfp_info_t * sfp_make_body_info_from_session_info(sfp_session_info_t * session){
	sfp_info_t * body_info = NULL;

	if((body_info = sfp_create_empty_sfp_info()) == NULL){
		m_log_error("Could not create sfp_info_t", "sfp_make_body_info_from_session_info");
		return NULL;
	}

	sfp_add_version_info(body_info, SFP_PROTOCOL_VERSION);
	if(strfilled(session->session_id) &&
		strfilled(session->local_username) &&
		strfilled(session->local_ip_address_type) &&
		strfilled(session->local_ip) &&		
		strfilled(session->local_port) &&
		strfilled(session->connection_id))
		// TODO network type fixed for the moment
		sfp_add_origin_info(body_info, session->session_id, session->local_username, SFP_NETWORK_TYPE_INTERNET, session->local_ip_address_type, session->local_ip, session->local_port, session->connection_id);

	if(strfilled(session->local_mode))
		sfp_add_mode_info(body_info, session->local_mode);

	if(strfilled(session->ip_protocol) &&
		strfilled(session->required_bandwidth) &&
		strfilled(session->packet_size))
		sfp_add_transfer_info(body_info, session->ip_protocol, session->required_bandwidth, session->packet_size);

	if(strfilled(session->short_filename) &&
		strfilled(session->file_type) &&
		strfilled(session->file_size))
		sfp_add_file_info(body_info, session->short_filename, session->file_type, session->file_size);

	return body_info;
}
/**
* Gives an available port for the file transfer.
* WARNING always give the same port; function not doing its work yet
* TODO really try to find an available port by trying to bind, and store the socket for later use
*
* @param	[out]	buf : the buffer that will contain the port
*/
static void sfp_get_file_transfer_port(char buf[]){
	int port = atoi(sfp_file_transfer_port);
	//int i = 0;

	buf[0] = 0;

	//while(1){
	//	if (!ph_port_inuse(port)){ // TODO remplacer avec une fonction qui check parmi les sessions si le port n'est pas deja utilise
			sprintf(buf, "%d", port);
			return;
	//	}else{
	//		port += 2;    /* try next pair */
	//	}
	//}
}

/**
* Gets the default protocol to use for file transfers
*
* @return	the default protocol to be used : either SFP_IP_PROTOCOL_TCP or SFP_IP_PROTOCOL_UDP
*/
static char * sfp_get_default_ip_protocol(){
	if(strfilled(sfp_default_ip_protocol)){
		return sfp_default_ip_protocol;
	}
	// else default to TCP
	return SFP_IP_PROTOCOL_TCP;
}
/**
* Gets the default packet size for UDP transfers
*
* @return	the default packet size for UDP transfers
*/
static char * sfp_get_default_packet_size(){ // TODO rename to sfp_get_default_udp_packet_size
	if(strfilled(sfp_default_packet_size)){
		return sfp_default_packet_size;
	}
	// else default to TCP, so packet_size="0"
	return "0";
}

/**
* Indicates wether the client can do TCP file transfers or not
* TODO for the moment, always says TRUE
*
* @return	TRUE if TCP file transfers can be done; FALSE else
*/
static unsigned int sfp_can_do_tcp(){
	return TRUE; // TODO bouchon; set par le prog top level, a proposer en service du plugin
}

/**
* Indicates wether the client can do UDP file transfers or not
* TODO for the moment, always says FALSE
*
* @return	TRUE if UDP file transfers can be done; FALSE else
*/
static unsigned int sfp_can_do_udp(){
	return FALSE; // TODO bouchon; set par le prog top level, a proposer en service du plugin
}
/**
* Callback that get called when the receive thread ends.
* Notifies the top level program, and does some cleanup.
*
* @param	[in][out]	session : the session info
* @param	[in]	code : the return code of the thread (TRUE or FALSE for the moment)
*/
static void sfp_receive_terminaison(sfp_session_info_t * session, sfp_returncode_t code){
	int call_id = -1;

	if(session != NULL){
		call_id = session->call_id;
	}else{
		m_log_error("No given session","sfp_receive_terminaison");
		// TODO notify GUI
		return;
	}

	if(code != SUCCESS){
		if(session->state == SFP_SESSION_CANCELLED){
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferCancelled) sfp_cbks->transferCancelled(call_id, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			phBye(call_id);
		}else if(session->state == SFP_SESSION_CLOSED_BY_PEER){			
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferClosedByPeer) sfp_cbks->transferClosedByPeer(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// TODO send a BYE ?
			//phCancel(call_id); // TODO can we do an CANCEL even if we do not make an INVITE
		}else if(session->state == SFP_SESSION_RECEIVED_INCOMPLETE){			
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferFromPeerStopped) sfp_cbks->transferFromPeerStopped(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			//phBye(call_id);
		}else{ // local errors
			// transfer failed
			if(sfp_cbks != NULL && sfp_cbks->transferFromPeerFailed) sfp_cbks->transferFromPeerFailed(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			phBye(call_id);
		}		
	}else if(code == SUCCESS){
		if(session->state == SFP_SESSION_RECEIVED_COMPLETE){
			// notify GUI of the end of the receive
			if(sfp_cbks != NULL && sfp_cbks->transferFromPeerFinished) sfp_cbks->transferFromPeerFinished(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			session->state = SFP_SESSION_FINISHED;

			// close the call
			phBye(call_id);
		}
		// TODO else what?
	}

	// free the session
	sfp_remove_session_info(session->call_id);
}

/**
* Callback that get called when the send thread ends.
* Notifies the top level program, and does some cleanup.
*
* @param	[in][out]	session : the session info
* @param	[in]	code : the return code of the thread (TRUE or FALSE for the moment)
*/
static void sfp_send_terminaison(sfp_session_info_t * session, sfp_returncode_t code){
	int call_id = -1;

	if(session != NULL){
		call_id = session->call_id;
	}else{
		m_log_error("No given session","sfp_send_terminaison");
		// TODO notify GUI
		return;
	}

	if(code != SUCCESS){
		if(session->state == SFP_SESSION_CANCELLED){			
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferCancelled) sfp_cbks->transferCancelled(call_id, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			phBye(call_id);
		}else if(session->state == SFP_SESSION_CLOSED_BY_PEER){ // received a BYE while still sending
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferClosedByPeer) sfp_cbks->transferClosedByPeer(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// free the session
			sfp_remove_session_info(session->call_id);
		}else if(session->state == SFP_SESSION_SENT_INCOMPLETE){			
			// notify GUI
			if(sfp_cbks != NULL && sfp_cbks->transferToPeerStopped) sfp_cbks->transferToPeerStopped(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			//phBye(call_id);
		}else{ // local errors
			// transfer failed
			if(sfp_cbks != NULL && sfp_cbks->transferToPeerFailed) sfp_cbks->transferToPeerFailed(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			// send a BYE
			phBye(call_id);
		}
	}else if(code == SUCCESS){
		if(session->state == SFP_SESSION_SENT_COMPLETE){
			// notify GUI of the end of the send
			if(sfp_cbks != NULL && sfp_cbks->transferToPeerFinished) sfp_cbks->transferToPeerFinished(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);

			session->state = SFP_SESSION_FINISHED;
		}
		// TODO else what?
	}

	// do not free the session here, but at the receive of a BYE from the receiver
}
/**
* Callback that get called by the transfer thread to notify top level program of the progression of the transfer
*
* @param	[in]	session : the session info
* @param	[in]	percentage : the percentage of transfer progress
*/
static void sfp_progressionCallback(sfp_session_info_t * session, int percentage){
	if(sfp_cbks != NULL && sfp_cbks->transferProgression != NULL) sfp_cbks->transferProgression(session->call_id, percentage);
}

/************* PLUGIN CALLBACKS **************/

/**
* Callback that get called by PhApi when an INVITE to a file transfer arrives.
* Parses the SFP message. Creates a transfer session. Notifies the top level program that an
* invite to a file transfer arrived.
*
* @param	[in]	event : an event containing the SFP body
*/
static void sfp_on_EXOSIP_CALL_NEW(eXosip_event_t * event){
	sfp_info_t * received_info;
	char * body;
	sfp_session_info_t * session = NULL;
	int call_id;

	if(event == NULL){
		m_log_error("Received (null) event","sfp_on_EXOSIP_CALL_NEW");
		return; // TODO notify GUI
	}

	// get the body and parse it
	if(!strfilled(event->msg_body)){
		m_log_error("No body in the event","sfp_on_EXOSIP_CALL_NEW");
		return; // TODO notify GUI
	}
	body = event->msg_body;
	if((received_info = sfp_parse_message(body)) == NULL){
		m_log_error("Could not parse sfp body","sfp_on_EXOSIP_CALL_NEW");
		return; // TODO notify GUI
	}

	// create a new session
	if((session = sfp_make_session(ph_get_vlid(event->local_uri, event->req_uri))) == NULL){
		m_log_error("Could not create session","sfp_on_EXOSIP_CALL_NEW");
		// free the received infos
		sfp_free_sfp_info(&received_info);
		return; // TODO notify GUI
	}

	// set the call_id
	session->call_id = event->cid;

	// set the local usable port
	if(sfp_transfer_get_free_port(session) != SUCCESS){
		m_log_error("Could not find a free port","sfp_on_EXOSIP_CALL_NEW");
		// free the received infos
		sfp_free_sfp_info(&received_info);
		free_sfp_session_info(&session);
		return; // TODO notify GUI
	}

	// store the session
	sfp_add_session_info(event->cid, session);

	// store incoming body infos into a session (which will normally be the one we created and stored upper)
	if((session = sfp_make_session_info_from_body_info(event->cid, session, received_info, SFP_INCOMING_INFO)) == NULL){
		m_log_error("Could not add incoming infos to the session","sfp_on_EXOSIP_CALL_NEW");
		return; // TODO notify GUI
	}

	// the sfp incoming infos are stored, we can free info
	sfp_free_sfp_info(&received_info);

	// create the call
	if((call_id = phNewCall(event->cid, event->did, event->local_uri, event->req_uri)) <= 0){
		m_log_error("Could not create the call", "sfp_on_EXOSIP_CALL_NEW");
		// remove the session
		sfp_remove_session_info(session->call_id);
		return;
	}

	// TODO notify GUI
	if(sfp_cbks != NULL && sfp_cbks->newIncomingFile) sfp_cbks->newIncomingFile(call_id, session->remote_username, session->short_filename, session->file_type, session->file_size);
}
/**
* Callback called when a 200OK has been received.
* Parses the SFP message. Adds information to the session. Starts the receive thread. Notifies the top level program
*
* @param	[in]	event : an event containing the SFP body
*/
static void sfp_on_EXOSIP_CALL_ANSWERED(eXosip_event_t * event){
	sfp_info_t * received_info;
	char * body;
	sfp_session_info_t * session = NULL;
	pthread_t thread;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		// free the received info
		sfp_free_sfp_info(&received_info);
		return; // TODO notify GUI
	}

	if(session->state != SFP_SESSION_PAUSED && session->state != SFP_SESSION_RESUMED){
		if(event == NULL){
			m_log_error("Received (null) event","sfp_on_EXOSIP_CALL_ANSWERED");
			return; // TODO notify GUI
		}

		// get the body and parse it
		if(!strfilled(event->msg_body)){
			m_log_error("No body in the event","sfp_on_EXOSIP_CALL_ANSWERED");
			return; // TODO notify GUI
		}
		body = event->msg_body;
		if((received_info = sfp_parse_message(body)) == NULL){
			m_log_error("Could not parse sfp body","sfp_on_EXOSIP_CALL_ANSWERED");
			return; // TODO notify GUI
		}

		// store incoming body infos into the session we retrieved before
		if((session = sfp_make_session_info_from_body_info(event->cid, session, received_info, SFP_INCOMING_INFO)) == NULL){
			m_log_error("Could not add incoming infos to the session","sfp_on_EXOSIP_CALL_NEW");
			// free the received info
			sfp_free_sfp_info(&received_info);
			// remove the session
			sfp_remove_session_info(event->cid);
			return; // TODO notify GUI
		}

		// the sfp incoming infos are stored, we can free info
		sfp_free_sfp_info(&received_info);

		phStopRinging(event->cid);
		phCallAnswered(event->cid, event->did, event->status_code);

		// install the terminaison callback
		session->terminaisonCallback = &sfp_send_terminaison;

		// create the send thread
		if(pthread_create(&thread, NULL, (void *)sfp_transfer_send_file, (void *)session) != 0){
			// TODO free le thread
			m_log_error("Could not create send thread", "sfp_on_EXOSIP_CALL_ANSWERED");
			phCancel(event->cid);
			// TODO notify GUI
			return;
		}else{
			// notify GUI of the begining of the send
			if(sfp_cbks != NULL && sfp_cbks->sendingFileBegin) sfp_cbks->sendingFileBegin(event->cid, session->remote_username, session->short_filename, session->file_type, session->file_size);	
		}
	}

	if(session->state == SFP_SESSION_RESUMED){
		session->state = SFP_SESSION_RUNNING;
		if(sfp_cbks != NULL && sfp_cbks->transferResumed) sfp_cbks->transferResumed(event->cid, session->remote_username, session->short_filename, session->file_type, session->file_size);
	}
}

/**
* Callback called when receiving a TRYING and the other party has not yet been found
*
* @param	[in]	event : an event
*/
static void sfp_on_EXOSIP_CALL_PROCEEDING(eXosip_event_t * event){ // TRYING received (destination to be found)
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	if(session->state != SFP_SESSION_PAUSED){
		phProceeding(event->cid, event->status_code);
		if(sfp_cbks != NULL && sfp_cbks->waitingForAnswer) sfp_cbks->waitingForAnswer(event->cid, event->remote_uri);
	}
}

/**
* Callback called when receiving a TRYING and the other party has been found
*
* @param	[in]	event : an event
*/
static void sfp_on_EXOSIP_CALL_RINGING(eXosip_event_t * event){ // destination found and prepare for transfer
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	if(session->state != SFP_SESSION_PAUSED){
		phRinging(event->cid);
		if(sfp_cbks != NULL && sfp_cbks->waitingForAnswer) sfp_cbks->waitingForAnswer(event->cid, event->remote_uri);

	}
}

/**
* TODO doc
*/
static void sfp_on_EXOSIP_CALL_REQUESTFAILURE(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	sfp_remove_session_info(event->cid);

	phRequestFailure(event->cid, event->status_code);

	// TODO notify GUI
}

/**
* TODO doc
*/
static void sfp_on_EXOSIP_CALL_SERVERFAILURE(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	sfp_remove_session_info(event->cid);

	phServerFailure(event->cid, event->status_code);

	// TODO notify GUI
}

/**
* TODO doc
*/
static void sfp_on_EXOSIP_CALL_GLOBALFAILURE(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	sfp_remove_session_info(event->cid);

	phGlobalFailure(event->cid, event->status_code);
}

/**
* TODO doc
*/
static void sfp_on_EXOSIP_CALL_NOANSWER(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	sfp_remove_session_info(event->cid);

	phNoAnswer(event->cid, event->status_code);
}

/**
* Callback called on receive of a CANCEL or a BYE.
* Notifies the thread if running. Makes some clean up.
*
* @param	[in]	event : an event
*/
static void sfp_on_EXOSIP_CALL_CLOSED(eXosip_event_t * event){ // BYE received
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	if(session->state != SFP_SESSION_FINISHED){
		session->state = SFP_SESSION_CLOSED_BY_PEER;
	}

	if(sfp_cbks != NULL && sfp_cbks->transferClosedByPeer) sfp_cbks->transferClosedByPeer(event->cid, session->remote_username, session->short_filename, session->file_type, session->file_size);

	if(session->state == SFP_SESSION_FINISHED){
		// suppress the session
		sfp_remove_session_info(event->cid);
	}

	phEndCall(event->cid, event->status_code);
}


static void sfp_on_EXOSIP_CALL_HOLD(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	session->state = SFP_SESSION_PAUSED;

	if(sfp_cbks != NULL && sfp_cbks->transferPausedByPeer) sfp_cbks->transferPausedByPeer(event->cid, session->remote_username, session->short_filename, session->file_type, session->file_size);
}


static void sfp_on_EXOSIP_CALL_OFFHOLD(eXosip_event_t * event){
	sfp_session_info_t * session = NULL;

	// retrieve the corresponding session
	if((event->cid <= 0) || (event->cid > 0 && (session = sfp_get_session_info(event->cid)) == NULL)){
		m_log_error("Could not retrieve session","sfp_on_EXOSIP_CALL_ANSWERED");
		return; // TODO notify GUI
	}

	session->state = SFP_SESSION_RUNNING;

	if(sfp_cbks != NULL && sfp_cbks->transferResumedByPeer) sfp_cbks->transferResumedByPeer(event->cid, session->remote_username, session->short_filename, session->file_type, session->file_size);
}
