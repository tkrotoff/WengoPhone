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
* indicateurs de transfer
* timeouts de transfer
* code strengthen
* on est limite a la taille d'un unsigned long pour la taille maxi de transfer de fichier
* unregister doit nettoyer les structures allouées
*/

#include <sfp-plugin/sfp-commons.h>
#include <sfp-plugin/sfp-plugin.h>
#include <sfp-plugin/sfp-error.h>

#include <phapi-util/mystdio.h>
#include <phapi-util/util.h>
#include <phapi-util/phapi-globals.h>
#include <phapi-util/mystring.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>


#define READ_WRITE_BUFFER_SIZE					1024

#define SFP_TRANSFER_UDP_ENDING_STRING			"OK"
#define SFP_TRANSFER_UDP_ENDING_STRING_LENGTH	2

#define SFP_REUSABLE_SOCKET						1
#define SFP_NOT_REUSABLE_SOCKET					0

#define SFP_MAX_RETRIES							5
#define SFP_WAIT_TIME_BASE						1
#define SFP_TIMEOUT_SEC							25
#define SFP_MAX_PORT							65535

#define SFP_TRANSFER_ACTIVE						1
#define SFP_TRANSFER_PASSIVE					2

#define SFP_TRANSFER_TCP						SOCK_STREAM
#define SFP_TRANSFER_UDP						SOCK_DGRAM

#define WAIT_PAUSE_DELAY						25000 // microseconds


// ----- PRIVATE FUNCTION DECLARATION -----
static sfp_returncode_t sfp_transfer_send_file2(char * filename, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_receive_file2(char * filename, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_send_switch(FILE * stream, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_receive_switch(FILE * stream, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session);
static sfp_returncode_t init_connection(struct sockaddr_in * address, SOCKET * sckt, unsigned int ip_protocol, const char * ip, unsigned short port);
static void finalize_connection(SOCKET sckt);
static sfp_returncode_t sfp_transfer_send_active(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_send_passive(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_receive_active(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session);
static sfp_returncode_t sfp_transfer_receive_passive(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session);
static unsigned int send_udp(FILE * stream, SOCKET sckt, struct sockaddr_in address);
static unsigned int receive_udp(FILE * stream, SOCKET sckt, struct sockaddr_in address);
static void notify_progress(sfp_session_info_t * session, unsigned long actual, unsigned long final, unsigned int * increase);
// -----

// ----- PRIVATE FUNCTION DEFINITION -----

/**
* Sends a file using the information provided in the session info
* TODO mutualize initialization part
*
* @param	[in][out]	session : a session info
* @return	TRUE if the sending succeeded; FALSE else
*/
unsigned int sfp_transfer_send_file(sfp_session_info_t * session){
	unsigned int mode, protocol;
	unsigned short port;
	sfp_returncode_t res = SUCCESS;

	// mark the session as running
	session->state = SFP_SESSION_RUNNING;

	// TODO add checks
	if(strequals(session->local_mode, SFP_MODE_ACTIVE)){
		mode = SFP_TRANSFER_ACTIVE;
	}else if(strequals(session->local_mode, SFP_MODE_PASSIVE)){
		mode = SFP_TRANSFER_PASSIVE;
	}
	if(strequals(session->ip_protocol, SFP_IP_PROTOCOL_TCP)){
		protocol = SFP_TRANSFER_TCP;
	}else if(strequals(session->local_mode, SFP_IP_PROTOCOL_UDP)){
		protocol = SFP_TRANSFER_UDP;
	}

	// check the mode first
	if(strequals(session->local_mode, SFP_MODE_ACTIVE)){
		// TODO add checks
		port = (unsigned short)atoi(session->remote_port);

		// TODO add checks
		res = sfp_transfer_send_file2(session->filename, protocol, mode, session->remote_ip, port, session);
	}else{
		// TODO add checks
		port = (unsigned short)atoi(session->local_port);

		res = sfp_transfer_send_file2(session->filename, protocol, mode, session->local_ip, port, session);
	}

	// tell the main thread that the tranfser ended, via a callback
	if(session->terminaisonCallback != NULL) session->terminaisonCallback(session, res);

	return res;
}

/**
* Receives a file using the information provided in the session info
* TODO mutualize initialization part
*
* @param	[in][out]	session
* @return	TRUE if the receiving succeeded; FALSE else
*/
unsigned int sfp_transfer_receive_file(sfp_session_info_t * session){
	unsigned int mode, protocol;
	unsigned short port;
	sfp_returncode_t res = SUCCESS;

	// mark the session as running
	session->state = SFP_SESSION_RUNNING;

	// TODO add checks
	if(strequals(session->local_mode, SFP_MODE_ACTIVE)){
		mode = SFP_TRANSFER_ACTIVE;
	}else if(strequals(session->local_mode, SFP_MODE_PASSIVE)){
		mode = SFP_TRANSFER_PASSIVE;
	}
	if(strequals(session->ip_protocol, SFP_IP_PROTOCOL_TCP)){
		protocol = SFP_TRANSFER_TCP;
	}else if(strequals(session->local_mode, SFP_IP_PROTOCOL_UDP)){
		protocol = SFP_TRANSFER_UDP;
	}

	// check the mode first
	if(strequals(session->local_mode, SFP_MODE_ACTIVE)){
		// TODO add checks
		port = (unsigned short)atoi(session->remote_port);

		// TODO add checks
		res = sfp_transfer_receive_file2(session->filename, protocol, mode, session->remote_ip, port, session);
	}else{
		// TODO add checks
		port = (unsigned short)atoi(session->local_port);

		res = sfp_transfer_receive_file2(session->filename, protocol, mode, session->local_ip, port, session);
	}

	// tell the main thread that the tranfser ended, via a callback
	if(session->terminaisonCallback != NULL) session->terminaisonCallback(session, res);

	return res;
}

/**
* Sends a file using the information provided in the session info
* TODO fusion this function with the "sfp_transfer_send_file(sfp_session_info_t * session)"
*
* @param	[in]	filename : the "full" file name (including the whole path)
* @param	[in]	ip_protocol : the IP protocol, can be SFP_TRANSFER_TCP or SFP_TRANSFER_UDP
* @param	[in]	mode : the transfer mode, can be SFP_TRANSFER_ACTIVE or SFP_TRANSFER_PASSIVE
* @param	[in]	ip : the public ip 
* @param	[in]	port : the public port
* @param	[in][out]	session : a session info
* @return	TRUE if the sending succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_send_file2(char * filename, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session){
	FILE * stream = NULL;
	unsigned int success = SUCCESS;

	if((stream = fopen(filename, "rb")) == NULL){
		m_log_error("Could not open file in read mode", "sfp_transfer_send_file2");
		return CANT_READ_FILE; // fail
	}else{
		// success
	}

	if(sfp_transfer_send_switch(stream, ip_protocol, mode, ip, port, session) != SUCCESS){
		success = FILE_SEND_FAILED; // fail
	}else{
		// success
	}

	if(fclose(stream) != 0){
		success = CANT_CLOSE_FILE; // fail
	}else{
		// success
	}

	return success; // TODO
}

/**
* Receives a file using the information provided in the session info
* TODO fusion this function with the "sfp_transfer_receive_file(sfp_session_info_t * session)"
*
* @param	[in]	filename : the "full" file name (including the whole path)
* @param	[in]	ip_protocol : the IP protocol, can be SFP_TRANSFER_TCP or SFP_TRANSFER_UDP
* @param	[in]	mode : the transfer mode, can be SFP_TRANSFER_ACTIVE or SFP_TRANSFER_PASSIVE
* @param	[in]	ip : the public ip 
* @param	[in]	port : the public port
* @param	[in][out]	session : a session info
* @return	TRUE if the receiving succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_receive_file2(char * filename, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session){
	FILE * stream = NULL;
	unsigned int success = SUCCESS;

	if((stream = fopen(filename, "wb")) == NULL){
		m_log_error("Could not open file in write mode", "sfp_transfer_receive_file2");
		return CANT_WRITE_FILE; // fail
	}else{
		// success
	}

	if(sfp_transfer_receive_switch(stream, ip_protocol, mode, ip, port, session) != SUCCESS){
		success = FILE_RECEIVE_FAILED; // fail
	}else{
		// success
	}

	if(fclose(stream) != 0){
		success = CANT_CLOSE_FILE; // fail
	}else{
		// success
	}

	return success; // TODO
}

/**
* Selector function that chooses the right send function, concerning the mode and ip protocol
*
* @param	[in]	stream : the file stream from which to read
* @param	[in]	ip_protocol : the IP protocol, can be SFP_TRANSFER_TCP or SFP_TRANSFER_UDP
* @param	[in]	mode : the transfer mode, can be SFP_TRANSFER_ACTIVE or SFP_TRANSFER_PASSIVE
* @param	[in]	ip : the public ip 
* @param	[in]	port : the public port
* @param	[in][out]	session : a session info
* @return	TRUE if the sending succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_send_switch(FILE * stream, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session){
	struct sockaddr_in address;
	SOCKET sckt = 0;
	sfp_returncode_t success = SUCCESS;
	
	if(ip_protocol == SOCK_STREAM){ // TCP
		if(mode == SFP_TRANSFER_ACTIVE){
			if(init_connection(&address, &sckt, ip_protocol, ip, port) == SUCCESS){
				success = sfp_transfer_send_active(stream, sckt, address, session);
				finalize_connection(sckt);
			}else{
				return NETWORK_INITIALIZATION_FAILED;
			}
		}else if(mode == SFP_TRANSFER_PASSIVE){
			success = sfp_transfer_send_passive(stream, session->local_socket, session->local_address, session);
			finalize_connection(session->local_socket);
		}
	}else if(ip_protocol == SOCK_DGRAM){ // UDP
		if(init_connection(&address, &sckt, ip_protocol, ip, port) == SUCCESS){
			success = send_udp(stream, sckt, address);
			finalize_connection(sckt);
		}else{
			return NETWORK_INITIALIZATION_FAILED;
		}
	}

	return success; // TODO
}

/**
* Selector function that chooses the right receive function, concerning the mode and ip protocol
*
* @param	[in]	stream : the file stream into which to write
* @param	[in]	ip_protocol : the IP protocol, can be SFP_TRANSFER_TCP or SFP_TRANSFER_UDP
* @param	[in]	mode : the transfer mode, can be SFP_TRANSFER_ACTIVE or SFP_TRANSFER_PASSIVE
* @param	[in]	ip : the public ip 
* @param	[in]	port : the public port
* @param	[in][out]	session : a session info
* @return	TRUE if the receiving succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_receive_switch(FILE * stream, unsigned int ip_protocol, unsigned int mode, const char * ip, unsigned short port, sfp_session_info_t * session){
	struct sockaddr_in address;
	SOCKET sckt = 0;
	sfp_returncode_t success = SUCCESS;
	
	if(ip_protocol == SOCK_STREAM){ // TCP
		if(mode == SFP_TRANSFER_ACTIVE){
			if(init_connection(&address, &sckt, ip_protocol, ip, port) == SUCCESS){
				success = sfp_transfer_receive_active(stream, sckt, address, session);
				finalize_connection(sckt);
			}else{
				return NETWORK_INITIALIZATION_FAILED;
			}
		}else if(mode == SFP_TRANSFER_PASSIVE){
			success = sfp_transfer_receive_passive(stream, session->local_socket, session->local_address, session);
			finalize_connection(session->local_socket);
		}
	}else if(ip_protocol == SOCK_DGRAM){ // UDP
		if(init_connection(&address, &sckt, ip_protocol, ip, port) == SUCCESS){
			success = receive_udp(stream, sckt, address);
			finalize_connection(sckt);
		}else{
			return NETWORK_INITIALIZATION_FAILED;
		}
	}

	return success; // TODO
}

/* JULIEN */

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

int sfp_transfer_send_connect_id(SOCKET sckt, char * connect_id, int size)
{
	char buff[24];
	int bsize;
	int retry = 3;
	int ret = 0;

	memset(buff, 0, sizeof(buff));
	snprintf(buff, sizeof(buff), "%s\n", connect_id);
	for (ret = 0, bsize = (size + 1); bsize && retry; retry--){
		ret = send(sckt, buff, bsize, MSG_NOSIGNAL);
		if (ret <= 0){
			return -1;
		}

		bsize -= ret;
	}

	return (retry == 0 ? -1 : 0);
}
/* ****** */

/**
* Sends the file read from a stream, in TCP and in active mode (means that it does a "connect").
*
* @param	[in]	stream : the file stream from which to read
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @param	[in][out]	session : a session info
* @return	TRUE if the sending succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_send_active(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session){
	char message[256];
	char buffer[READ_WRITE_BUFFER_SIZE];
	size_t read = 0;
	int retries = SFP_MAX_RETRIES;
	int wait_time = SFP_WAIT_TIME_BASE;
	int res_connect = -1;
	int sent = 0;
	int tmp_sent = 0;
	long total_sent = 0;
	long total_to_send = (unsigned long)atol(session->file_size);
	unsigned int increase = 0;
	fd_set sckts;
	struct timeval timeout = {SFP_TIMEOUT_SEC, 0};
	int max_sckt;

	while((res_connect = connect(sckt, (struct sockaddr *)&address, sizeof(address))) < 0 && retries-- > 0){
		sprintf(message, "Waiting for %d ms", wait_time);
		m_log(message, "sfp_transfer_send_active");
		if(session->state == SFP_SESSION_CLOSED_BY_PEER || session->state == SFP_SESSION_CANCELLED){
			break;
		}
		sleep(wait_time);
		wait_time = wait_time * 2;
	}
	if(res_connect < 0){
		m_log_error("Could not connect to peer", "sfp_transfer_send_active");
		return CANT_CONNECT; // fail
	}

	/* JULIEN */
	if (!session->connection_id || session->connection_id[0] == 0)
		return TRANSFER_CORRUPTION;
	if (sfp_transfer_send_connect_id(sckt, session->connection_id, strlen(session->connection_id)) < 0){
		m_log_error("Could not send connection ID", "sfp_transfer_send_active");
		return TRANSFER_CORRUPTION;
	}
	/* ****** */

	memset(buffer, 0, sizeof(buffer));
	while((read = fread(buffer, sizeof(char), READ_WRITE_BUFFER_SIZE, stream)) > 0){
		// if the transfer has been paused, wait
		while(session->state == SFP_SESSION_PAUSED){
			usleep(WAIT_PAUSE_DELAY);
		}
		// if the transfer has been cancelled stop sending
		if(session->state == SFP_SESSION_CLOSED_BY_PEER){
			return SUCCESS;
		}else if(session->state == SFP_SESSION_CANCELLED){
			return SUCCESS;
		}
		sent = 0;
		while(sent < (int)read){
			FD_ZERO(&sckts);
			FD_SET(sckt, &sckts);
			max_sckt = sckt+1;
			timeout.tv_sec = SFP_TIMEOUT_SEC;
			timeout.tv_usec = 0;
			if(select(max_sckt, NULL, &sckts, NULL, &timeout) > 0){
				if((tmp_sent = send(sckt, buffer, (int)read, 0)) >= 0){
					sent += tmp_sent;
				}else{
					m_log_error("Send failed", "sfp_transfer_send_active");
					return TRANSFER_CORRUPTION; // fail
				}
			}else{
				FD_CLR(sckt, &sckts);
				m_log_error("Connection timed out", "sfp_transfer_send_active");
				return CONNECTION_TIMED_OUT; // fail
			}
		}

		total_sent += sent;

		if(total_sent > total_to_send){
			m_log_error("Sent more bytes than declared", "sfp_transfer_send_active");
			return TRANSFER_CORRUPTION; // fail
		}

		// notify the progession of the transfer
		notify_progress(session, total_sent, total_to_send, &increase);

		memset(buffer, 0, sizeof(buffer));
	}

	// check that we have sent it all
	if(total_sent < total_to_send){ // TODO verify
		session->state = SFP_SESSION_SENT_INCOMPLETE;
		return TRANSFER_CORRUPTION;
	}else if(total_sent == total_to_send){
		session->state = SFP_SESSION_SENT_COMPLETE;
	}

	return SUCCESS; // TODO
}

/**
* Sends the file read from a stream, in TCP and in passive mode (means that it does a "select and accept").
*
* @param	[in]	stream : the file stream from which to read
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @param	[in][out]	session : a session info
* @return	TRUE if the sending succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_send_passive(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session){
	char buffer[READ_WRITE_BUFFER_SIZE];
	socklen_t addrlen;
	size_t read = 0;
	SOCKET tmp = -1;
	fd_set sckts;
	struct timeval timeout = {SFP_TIMEOUT_SEC, 0};
	int max_sckt;
	int sent = 0;
	int tmp_sent = 0;
	long total_sent = 0;
	long total_to_send = (unsigned long)atol(session->file_size);
	unsigned int increase = 0;

	/* JULIEN */
	int ret = 0;
	int cread = 0;
	int total = 0;
	char cid_buff[32];
	/* ****** */

	addrlen = (socklen_t)sizeof(address);

	if(listen(sckt, 5) < 0){
		return CANT_LISTEN_ON_SOCKET; // fail
	}
	
	// use select to do a timeout in order not to stay blocked if peer cannot send file
	memset(cid_buff, 0, sizeof(cid_buff));
	while (1){
		
		FD_ZERO(&sckts);
		FD_SET(sckt, &sckts);
		if (tmp > 0)
			FD_SET(tmp, &sckts);

		max_sckt = (sckt > tmp ? sckt : tmp) + 1;
		
		ret = select(max_sckt, &sckts, NULL, NULL, &timeout);
		if (ret <= 0){
			// no connection received
			FD_CLR(sckt, &sckts);
			return CONNECTION_TIMED_OUT; // fail
		}
		if(FD_ISSET(sckt, &sckts)){
			tmp = accept(sckt, (struct sockaddr *)&address, &addrlen); // should block until someone connects
			if(tmp < 0){
				return CANT_ACCEPT_CONNECTION; // fail
			}
		}
		else if (FD_ISSET(tmp, &sckts)){
			if (total < (sizeof(cid_buff) -1)){
				if ((cread = recv(tmp, &cid_buff[total], 1, 0)) > 0)
					total += cread;
				else
					return CANT_ACCEPT_CONNECTION; // fail

				if (cid_buff[total] == '\n') {
					cid_buff[total] = '\0';
					break;
				}
			} 
			else {
				return CANT_ACCEPT_CONNECTION; // fail
			}
		}
		else
			return CANT_ACCEPT_CONNECTION; // fail
	}
	// CHECK CONNECTION ID
	if (strcmp(cid_buff, session->connection_id) != 0)
		return CANT_ACCEPT_CONNECTION; // fail
	/* ****** */

	FD_CLR(sckt, &sckts);

	memset(buffer, 0, sizeof(buffer));
	while((read = fread(buffer, sizeof(char), READ_WRITE_BUFFER_SIZE, stream)) > 0){
		// if the transfer has been paused, wait
		while(session->state == SFP_SESSION_PAUSED){
			usleep(WAIT_PAUSE_DELAY);
		}
		// if the transfer has been cancelled stop sending
		if(session->state == SFP_SESSION_CLOSED_BY_PEER){
			finalize_connection(tmp);
			return SUCCESS;
		}else if(session->state == SFP_SESSION_CANCELLED){
			finalize_connection(tmp);
			return SUCCESS;
		}
		sent = 0;
		while(sent < (int)read){
			FD_ZERO(&sckts);
			FD_SET(tmp, &sckts);
			max_sckt = tmp+1;
			timeout.tv_sec = SFP_TIMEOUT_SEC;
			timeout.tv_usec = 0;
			if(select(max_sckt, NULL, &sckts, NULL, &timeout) > 0){
				if((tmp_sent = send(tmp, buffer, (int)read, 0)) >= 0){
					sent += tmp_sent;
				}else{
					m_log_error("Send failed", "sfp_transfer_send_active");
					return TRANSFER_CORRUPTION; // fail
				}
			}else{
				FD_CLR(tmp, &sckts);
				m_log_error("Connection timed out", "sfp_transfer_send_active");
				return CONNECTION_TIMED_OUT; // fail
			}
		}
		
		total_sent += sent;

		if(total_sent > total_to_send){
			m_log_error("Sent more bytes than declared", "sfp_transfer_send_passive");
			return TRANSFER_CORRUPTION; // TODO errorcode
		}

		// notify the progession of the transfer
		notify_progress(session, total_sent, total_to_send, &increase);
		
		memset(buffer, 0, sizeof(buffer));
	}

	finalize_connection(tmp);

	// check that we have sent it all
	if(total_sent < total_to_send){ // TODO verify
		session->state = SFP_SESSION_SENT_INCOMPLETE;
		return TRANSFER_CORRUPTION;
	}else if(total_sent == total_to_send){
		session->state = SFP_SESSION_SENT_COMPLETE;
	}

	return SUCCESS; // TODO
}

/**
* Receives the file read from network and writes it into a strean, in TCP and in active mode (means that it does a "connect").
*
* @param	[in]	stream : the file stream into which to write
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @param	[in][out]	session : a session info
* @return	TRUE if the receiving succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_receive_active(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session){
	char buffer[READ_WRITE_BUFFER_SIZE];
	int received = 0;
	char message[256];
	int retries = SFP_MAX_RETRIES;
	int wait_time = SFP_WAIT_TIME_BASE;
	int res_connect = -1;
	long total_received = 0;
	long total_to_receive = (unsigned long)atol(session->file_size);
	unsigned int increase = 0;

	while((res_connect = connect(sckt, (struct sockaddr *)&address, sizeof(address))) < 0 && retries-- > 0){
		sprintf(message, "Waiting for %d ms", wait_time);
		m_log(message, "sfp_transfer_receive_active");
		if(session->state == SFP_SESSION_CLOSED_BY_PEER || session->state == SFP_SESSION_CANCELLED){
			break;
		}
		sleep(wait_time);
		wait_time = wait_time * 2;
	}
	if(res_connect < 0){
		m_log_error("Could not connect to peer", "sfp_transfer_send_active");
		return CANT_CONNECT; // fail
	}

	/* JULIEN */
	if (!session->connection_id || session->connection_id[0] == 0)
		return TRANSFER_CORRUPTION;
	if (sfp_transfer_send_connect_id(sckt, session->connection_id, strlen(session->connection_id)) < 0){
		m_log_error("Could not send connection ID", "sfp_transfer_send_active");
		return TRANSFER_CORRUPTION;
	}
	/* ****** */

	memset(buffer, 0, sizeof(buffer));
	while((received = recv(sckt, buffer, READ_WRITE_BUFFER_SIZE, 0)) > 0){
		total_received += (unsigned long)received;

		if(total_received > total_to_receive){
			m_log_error("Received more bytes than declared", "sfp_transfer_receive_active");
			return TRANSFER_CORRUPTION; // TODO errorcode
		}

		// notify the progession of the transfer
		notify_progress(session, total_received, total_to_receive, &increase);

		//sprintf(message, "Received %d char", received);
		m_log(message,"sfp_transfer_receive_active");
		// if the transfer has been cancelled stop sending
		if(session->state == SFP_SESSION_CLOSED_BY_PEER){
			return SUCCESS;
		}else if(session->state == SFP_SESSION_CANCELLED){
			return SUCCESS;
		}
		if((int)fwrite(buffer, sizeof(char), received, stream) < received){
			m_log_error("Wrote less char than what's been received", "sfp_transfer_receive_active");
			return WRITE_ERROR; // fail
		}else{
			// success
		}
		memset(buffer, 0, sizeof(buffer));
	}

	// check that we have received it all
	if(total_received < total_to_receive){ // TODO verify
		session->state = SFP_SESSION_RECEIVED_INCOMPLETE;
		return TRANSFER_CORRUPTION;
	}else if(total_received == total_to_receive){
		session->state = SFP_SESSION_RECEIVED_COMPLETE;
	}

	return SUCCESS;
}

/**
* Receives the file read from network and writes it into a strean, in TCP and in active mode (means that it does a "select and connect").
*
* @param	[in]	stream : the file stream into which to write
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @param	[in][out]	session : a session info
* @return	TRUE if the receiving succeeded; FALSE else
*/
static sfp_returncode_t sfp_transfer_receive_passive(FILE * stream, SOCKET sckt, struct sockaddr_in address, sfp_session_info_t * session){
	char buffer[READ_WRITE_BUFFER_SIZE];
	socklen_t addrlen;
	unsigned int received = 0;
	SOCKET tmp = -1;
	fd_set sckts;
	struct timeval timeout = {SFP_TIMEOUT_SEC, 0};
	int max_sckt;
	long total_received = 0;
	long total_to_receive = (unsigned long)atol(session->file_size);
	unsigned int increase = 0;

	/* JULIEN */
	int ret = 0;
	int cread = 0;
	int total = 0;
	char cid_buff[32];
	/* ****** */
	
	addrlen = (socklen_t)sizeof(address);

	if(listen(sckt, 5) < 0){
		return CANT_LISTEN_ON_SOCKET; // fail
	}

	/* JULIEN */
	// use select to do a timeout in order not to stay blocked if peer cannot send file
	memset(cid_buff, 0, sizeof(cid_buff));
	while (1){
		
		FD_ZERO(&sckts);
		FD_SET(sckt, &sckts);
		if (tmp > 0)
			FD_SET(tmp, &sckts);

		max_sckt = (sckt > tmp ? sckt : tmp) + 1;
		
		ret = select(max_sckt, &sckts, NULL, NULL, &timeout);
		if (ret <= 0){
			// no connection received
			FD_CLR(sckt, &sckts);
			return CONNECTION_TIMED_OUT; // fail
		}
		if(FD_ISSET(sckt, &sckts)){
			tmp = accept(sckt, (struct sockaddr *)&address, &addrlen); // should block until someone connects
			if(tmp < 0){
				return CANT_ACCEPT_CONNECTION; // fail
			}
		}
		else if (FD_ISSET(tmp, &sckts)){
			if (total < (sizeof(cid_buff) -1)){
				if ((cread = recv(tmp, &cid_buff[total], 1, 0)) > 0)
					total += cread;
				else
					return CANT_ACCEPT_CONNECTION; // fail

				if (cid_buff[total - 1] == '\n') {
					cid_buff[total - 1] = '\0';
					break;
				}
			} 
			else {
				return CANT_ACCEPT_CONNECTION; // fail
			}
		}
		else
			return CANT_ACCEPT_CONNECTION; // fail
	}
	// CHECK CONNECTION ID
	if (strcmp(cid_buff, session->connection_id) != 0)
		return CANT_ACCEPT_CONNECTION; // fail
	/* ****** */

	FD_CLR(sckt, &sckts);

	memset(buffer, 0, sizeof(buffer));
	while((received = recv(tmp, buffer, READ_WRITE_BUFFER_SIZE, 0)) > 0){
		total_received += (unsigned long)received;

		if(total_received > total_to_receive){
			m_log_error("Received more bytes than declared", "sfp_transfer_receive_passive");
			return TRANSFER_CORRUPTION; // TODO errorcode
		}

		// notify the progession of the transfer
		notify_progress(session, total_received, total_to_receive, &increase);

		// if the transfer has been cancelled stop sending
		if(session->state == SFP_SESSION_CLOSED_BY_PEER){
			finalize_connection(tmp);
			return SUCCESS;
		}else if(session->state == SFP_SESSION_CANCELLED){
			finalize_connection(tmp);
			return SUCCESS;
		}
		if(fwrite(buffer, sizeof(char), received, stream) < received){
			m_log_error("Wrote less char than what's been received", "sfp_transfer_receive_passive");
			return WRITE_ERROR; // fail
		}
		memset(buffer, 0, sizeof(buffer));
	}

	finalize_connection(tmp);

	// check that we have received it all
	if(total_received < total_to_receive){ // TODO verify
		session->state = SFP_SESSION_RECEIVED_INCOMPLETE;
		return TRANSFER_CORRUPTION;
	}else if(total_received == total_to_receive){
		session->state = SFP_SESSION_RECEIVED_COMPLETE;
	}

	return SUCCESS; // TODO
}


/**
* WARNING Needs to be implemented fully.
*
* Sends the file read from a stream, in UDP.
*
* @param	[in]	stream : the file stream from which to read
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @return	TRUE if the sending succeeded; FALSE else
*/
static unsigned int send_udp(FILE * stream, SOCKET sckt, struct sockaddr_in address){
	char buffer[READ_WRITE_BUFFER_SIZE];
	size_t read;

	memset(buffer, 0, sizeof(buffer));
	while((read = fread(buffer, sizeof(char), READ_WRITE_BUFFER_SIZE, stream)) > 0){
		if(sendto(sckt, buffer, (int)read, 0, (struct sockaddr *)&address, sizeof(address)) < (int)read ){
			return FALSE; // fail
		}else{
			// success
		}
		memset(buffer, 0, sizeof(buffer));
	}

	return TRUE; // TODO
}

/**
* WARNING Needs to be implemented fully.
*
* Receives the file read from a stream, in UDP.
*
* @param	[in]	stream : the file stream into which to write
* @param	[in]	sckt : the socket used for transfer
* @param	[in]	sockaddr_in : the adress used for transfer
* @return	TRUE if the receiving succeeded; FALSE else
*/
static unsigned int receive_udp(FILE * stream, SOCKET sckt, struct sockaddr_in address){
	char buffer[READ_WRITE_BUFFER_SIZE];
	socklen_t fromlen;
	int received;

	fromlen = (socklen_t)sizeof(address);

	memset(buffer, 0, sizeof(buffer));
	while((received = recvfrom(sckt, buffer, READ_WRITE_BUFFER_SIZE, 0, (struct sockaddr *)&address, &fromlen)) > 0){
		if((int)fwrite(buffer, sizeof(char), received, stream) < received){
			return FALSE; // fail
		}else{
			// success
		}
		memset(buffer, 0, sizeof(buffer));
	}

	// TODO indicateur de fin de transfer
	//finalize_connection(sckt);

	return TRUE; // TODO
}

/**
* Initializes a connection by filling in a sockaddr_in struct and opening a socket.
*
* @param	[out]	address : the address to initialize
* @param	[out]	socket : the socket to open
* @param	[in]	ip_protocol : the protocol to use, can be SFP_TRANSFER_TCP or SFP_TRANSFER_UDP
* @param	[in]	ip : the public ip
* @param	[in]	port : the public port
* @return	TRUE if the initialization succeeded; FALSE else
*/
static sfp_returncode_t init_connection(struct sockaddr_in * address, SOCKET * sckt, unsigned int ip_protocol, const char * ip, unsigned short port){
#ifdef WIN32  // TODO other OSes
	int opt;
#endif /* WIN32 */

	memset(address, 0, sizeof(struct sockaddr_in));
	address->sin_family = AF_INET;
	address->sin_port = htons(port);
	address->sin_addr.s_addr = inet_addr(ip);

	*sckt = socket(AF_INET, ip_protocol, 0);
	if(*sckt < 0){
		m_log_error("Could not get a socket", "init_connection");
		return CANT_GET_SOCKET; // fail
	}

#ifdef WIN32  // TODO other OSes
	opt = SFP_REUSABLE_SOCKET;
	if(setsockopt(*sckt, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
		m_log("Could not ste the socket in reusable mode","init_connection");
		return CANT_SET_SOCKET_REUSABLE;
	}
#endif /* WIN32 */

	return SUCCESS; // TODO
}

/**
* Closes a socket.
* TODO turn it into a void func
*
* @return	TRUE
*/
static void finalize_connection(SOCKET sckt){
	close(sckt);
}

/**
* Notifies of the progress of a file transfer
*
* @param	[in]	session : a session info providing the callback to call in order to notify of the transfer progress
* @param	[in]	actual : the actual size, in bytes
* @param	[in]	final : the size to reach, in bytes
* @param	[in][out]	increase : the progress step / next step to reach
*/
static void notify_progress(sfp_session_info_t * session, unsigned long actual, unsigned long final, unsigned int * increase){
	double percentage = 0;
	double pg = 49000000/((double)final+1000000)+1;
	int progression = (int)ceil(pg);

	if(actual == final) {
		if(session->progressionCallback != NULL) session->progressionCallback(session, 100);
		*increase = 100;
	} else {
		percentage = ((double)actual / (double)final) * 100;
		if(*increase == 0){
			*increase = progression;
		}
		if(percentage >= (double)(*increase)){
			if(session->progressionCallback != NULL) session->progressionCallback(session, *increase);
			while(percentage >= (double)(*increase)){
				*increase += progression;
			}
		}
	}
}

sfp_returncode_t sfp_transfer_get_free_port(sfp_session_info_t * session){
//#ifdef WIN32  // TODO other OSes
//	int opt;
//#endif /* WIN32 */
	int res_bind = -1;
	unsigned short port;
	char temp[33];
	int ip_protocol;


	if(strequals(session->ip_protocol, SFP_IP_PROTOCOL_TCP)){
		ip_protocol = SFP_TRANSFER_TCP;
	}else if(strequals(session->local_mode, SFP_IP_PROTOCOL_UDP)){
		ip_protocol = SFP_TRANSFER_UDP;
	}

	session->local_socket = socket(AF_INET, ip_protocol, 0);
	if(session->local_socket < 0){
		m_log_error("Could not get a socket", "sfp_transfer_get_free_port");
		return CANT_GET_SOCKET; // fail
	}

//#ifdef WIN32  // TODO other OSes
//	opt = SFP_REUSABLE_SOCKET;
//	if(setsockopt(session->local_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
//		m_log("Could not ste the socket in reusable mode","init_connection");
//		return FALSE;
//	}
//#endif /* WIN32 */

	port = (unsigned short)atoi(session->local_port);

	memset(&(session->local_address), 0, sizeof(struct sockaddr_in));
	session->local_address.sin_family = AF_INET;
	session->local_address.sin_port = htons(port);
	session->local_address.sin_addr.s_addr = inet_addr(session->local_ip);

	while((res_bind = bind(session->local_socket, (struct sockaddr *)&(session->local_address), sizeof(session->local_address))) < 0 && port < SFP_MAX_PORT){
		port += 1;
		session->local_address.sin_port = htons(port);
	}

	if(port >= SFP_MAX_PORT){
		m_log_error("Could not get a free transfer port", "sfp_transfer_get_free_port");
		finalize_connection(session->local_socket);
		return NO_AVAILABLE_PORT; // fail
	}

	if(session->local_port != NULL){
		free(session->local_port);
	}
	itostr((int)port, temp, sizeof(temp), 10);
	session->local_port = strdup(temp);

	return SUCCESS;
}
