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

#ifndef NETLIB_H
#define NETLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define NETLIB_BOOLEAN int
#define NETLIB_TRUE 1
#define NETLIB_FALSE 0

/**
 * @file netlib.h
 *
 * Detects the network configuration of a computer.
 *
 * @author Julien Bossart
 */

#define HTTP_PORT 80
#define HTTPS_PORT 443
#define SIP_PORT 5060

/**
 * Checks if a specific UDP port is opened.
 *
 * @param stun_server STUN server IP address
 * @param port port number to test
 * @return true if the port is opened; false otherwise
 */
NETLIB_BOOLEAN is_udp_port_opened(const char *stun_server, int port);

/**
 * Try to send a SIP request and wait for a response.
 *
 * @param sip_server Sip server address we try to SIPping
 * @param sip_port Sip server port we try to SIPping
 * @param timeout in milliseconds
 * @return true if a response is received; false otherwise
 */
NETLIB_BOOLEAN udp_sip_ping(const char *sip_server, int sip_port, int ping_timeout);

/**
 * Checks if a specific UDP local port is used.
 *
 * @param itf IP address of an interface. If NULL, default address is used.
 * @param port port number to test
 * @return true if the port is used; false otherwise
 */
NETLIB_BOOLEAN is_local_udp_port_used(const char *itf, int port);


/**
 * Gets the local HTTP proxy IP address.
 *
 * @return local HTTP proxy address or NULL
 */
char *get_local_http_proxy_address();


/**
 * Gets the local HTTP proxy port number.
 *
 * @return local HTTP proxy port or 0
 */
int get_local_http_proxy_port();


/**
 * Checks if an authentication with proxy is needed 
 *
 * @param itf IP address of an interface. If NULL, default address is used.
 * @param port port number to test
 * @return true if the port is used; false otherwise
 */
NETLIB_BOOLEAN is_proxy_auth_needed(const char *proxy_addr, int proxy_port);


/**
 * is_http_conn_allowed() or is_tunnel_conn_allowed() return.
 */
typedef enum 
{
	HTTP_OK = 0,
	HTTP_NOK,
	HTTP_AUTH
}	HttpRet;

/**
 * Checks if a http request can be sent.
 *
 * @param remote_addr host address we want to connect to
 * @param remote_port host port we want to connect to
 * @param proxy_addr proxy address we want to connect through
 * @param proxy_port proxy port we want to connect through
 * @param proxy_login if proxy authentication needed
 * @param proxy_passwd if proxy authentication needed
 * @param ssl use ssl or not
 * @return State of HTTP response
 */
 HttpRet is_http_conn_allowed(const char *remote_addr, int remote_port, 
							  const char *proxy_addr, int proxy_port, 
							  const char *proxy_login, const char *proxy_passwd,
							  NETLIB_BOOLEAN ssl);

/**
 * Checks if a http tunnel can be created.
 *
 * @param http_gate_addr http gateway address
 * @param http_gate_port http gateway port
 * @param sip_addr sip server address we want to join
 * @param sip_port sip server port we want to join
 * @param proxy_addr proxy address we want to connect through
 * @param proxy_port proxy port we want to connect through
 * @param proxy_login if proxy authentication needed
 * @param proxy_passwd if proxy authentication needed
 * @param ssl use ssl or not
 * @param sip_ping try to send a SIPping
 * @param ping_timeout in milliseconds
 * @return State of HTTP response or SIPping result if it's enabled
 */
 HttpRet is_tunnel_conn_allowed(const char *http_gate_addr, int http_gate_port,
								const char *sip_addr, int sip_port,
								const char *proxy_addr, int proxy_port, 
								const char *proxy_login, const char *proxy_passwd,
								NETLIB_BOOLEAN ssl, NETLIB_BOOLEAN sip_ping,
								int ping_timeout);
					

/**
 * NAT type.
 */
typedef enum  {
	StunTypeUnknown=0,
	StunTypeOpen,
	StunTypeConeNat,
	StunTypeRestrictedNat,
	StunTypePortRestrictedNat,
	StunTypeSymNat,
	StunTypeSymFirewall,
	StunTypeBlocked,
	StunTypeFailure
} NatType;

/**
 * Gets the NAT type of the computer.
 *
 * @param stun_server STUN server IP address used to detect the NAT type
 * @return NAT type or -1 if the STUN server is incorrect
 */
NatType get_nat_type(const char * stun_server);



#ifdef __cplusplus
}
#endif

#endif	//NETLIB_H
