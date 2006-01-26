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
 * Checks if a specific TCP port is opened.
 *
 * Blocking function. 3 seconds maximum before returning false
 *
 * @param host host to test the connection
 * @param port port number to test
 * @return true if the port is opened; false otherwise
 */
NETLIB_BOOLEAN is_tcp_port_open(const char * host, int port);

/**
 * Checks if a specific UDP port is opened.
 *
 * Blocking function. 3 seconds maximum before returning false
 *
 * @param stun_server STUN server IP address
 * @param port port number to test
 * @return true if the port is opened; false otherwise
 */
NETLIB_BOOLEAN is_udp_port_open(const char * stun_server, int port);

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

/**
 * Gets the local HTTP proxy IP address.
 *
 * @return local HTTP proxy address or NULL
 */
char * get_local_http_proxy_address();

/**
 * Gets the local HTTP proxy port number.
 *
 * @return local HTTP proxy port or -1
 */
int get_local_http_proxy_port();

#ifdef __cplusplus
}
#endif

#endif	//NETLIB_H
