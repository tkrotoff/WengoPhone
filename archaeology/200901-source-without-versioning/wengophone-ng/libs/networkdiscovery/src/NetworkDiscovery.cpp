/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <networkdiscovery/NetworkDiscovery.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/Logger.h>

#include <netlib.h>
#include <owsl.h>

NetworkDiscovery::NetworkDiscovery() {
	_owslInitialized = false;
}

void NetworkDiscovery::owsl_init() {
	if (!_owslInitialized) {
		if (::owsl_initialize() == 0) {
			_owslInitialized = true;
		} else {
			LOG_ERROR("owsl_initialize() failed");
		}
	}
}

void NetworkDiscovery::owsl_terminate() {
	if (_owslInitialized) {
		if (::owsl_terminate() == 0) {
			_owslInitialized = false;
		} else {
			LOG_ERROR("owsl_terminate() failed");
		}
	} else {
		LOG_ERROR("owsl not initialized");
	}
}

void NetworkDiscovery::setNetworkProxy(NetworkProxy networkProxy) {
	_networkProxy = networkProxy;
}

bool NetworkDiscovery::testHTTP(const std::string & url, bool ssl) {
	owsl_init();

	NETLIB_BOOLEAN sslActivated = ((ssl) ? NETLIB_TRUE : NETLIB_FALSE);
	bool result = false;

	if (is_http_conn_allowed(url.c_str(),
		_networkProxy.getServer().c_str(), _networkProxy.getServerPort(),
		_networkProxy.getLogin().c_str(), _networkProxy.getPassword().c_str(),
		sslActivated, HTTP_TIMEOUT) == HTTP_OK) {

		result = true;
	} else {
		LOG_DEBUG("cannot connect to " + url + (ssl ? " with" : " without") + " SSL");
	}

	owsl_terminate();

	return result;
}

bool NetworkDiscovery::testNAT(const std::string & stunServer, EnumNatType::NatType & natType) {
	owsl_init();

	LOG_DEBUG("testing UDP connection and discovering NAT type with STUN server");

	NatType nat;
	bool opened = (is_udp_port_opened(stunServer.c_str(), SIP_PORT, &nat) == NETLIB_TRUE ? true : false);

	switch(nat) {
	case StunTypeUnknown:
		natType = EnumNatType::NatTypeUnknown;
		break;
	case StunTypeOpen:
		natType = EnumNatType::NatTypeOpen;
		break;
	case StunTypeConeNat:
		natType = EnumNatType::NatTypeFullCone;
		break;
	case StunTypeRestrictedNat:
		natType = EnumNatType::NatTypeRestrictedCone;
		break;
	case StunTypePortRestrictedNat:
		natType = EnumNatType::NatTypePortRestrictedCone;
		break;
	case StunTypeSymNat:
		natType = EnumNatType::NatTypeSymmetric;
		break;
	case StunTypeSymFirewall:
		natType = EnumNatType::NatTypeSymmetricFirewall;
		break;
	case StunTypeBlocked:
		natType = EnumNatType::NatTypeBlocked;
		break;
	case StunTypeFailure:
		natType = EnumNatType::NatTypeFailure;
			break;
	default:
		LOG_FATAL("unknown NAT type=" + QString::number(natType).toStdString());
	}

	owsl_terminate();

	return opened;
}

bool NetworkDiscovery::testSIP(const std::string & server, int port,
	int localPort, const std::string & realm) {

	owsl_init();

	LOG_DEBUG("pinging SIP server " + server + " on port " + QString::number(port).toStdString()
		+ " from port " + QString::number(localPort).toStdString() + " with realm " + realm);
	return (udp_sip_ping(server.c_str(), port, localPort, PING_TIMEOUT,
		"nobody", realm.c_str()) == NETLIB_TRUE ? true : false);

	owsl_terminate();
}

bool NetworkDiscovery::testSIPHTTPTunnel(const std::string & tunnelServer, int tunnelPort, bool ssl,
	const std::string & sipServer, int sipServerPort, const std::string & realm) {

	owsl_init();

	bool result = false;

	LOG_DEBUG("testing SIP tunnel connection");
	if (is_tunnel_conn_allowed(tunnelServer.c_str(), tunnelPort,
		sipServer.c_str(), sipServerPort,
		_networkProxy.getServer().c_str(), _networkProxy.getServerPort(),
		_networkProxy.getLogin().c_str(), _networkProxy.getPassword().c_str(),
		(ssl ? NETLIB_TRUE : NETLIB_FALSE), HTTP_TIMEOUT,
		realm.c_str(), NETLIB_TRUE, PING_TIMEOUT) == HTTP_OK) {

		result = true;
	} else {
		LOG_DEBUG("cannot create a tunnel to " + tunnelServer + ":" + QString::number(tunnelPort).toStdString()
			+ " for SIP server " + sipServer + ":" + QString::number(sipServerPort).toStdString()
			+ (ssl ? " with" : " without") + " SSL");
	}

	owsl_terminate();

	return result;
}

int NetworkDiscovery::getFreeLocalPort() {
	owsl_init();

	int localPort = SIP_PORT;

	if (!is_local_udp_port_used(NULL, localPort)) {
		LOG_DEBUG("UDP port 5060 is free");
		return localPort;
	} else if (!is_local_udp_port_used(NULL, localPort + 1)) {
		LOG_DEBUG("UDP port 5060 is busy, will use 5061");
		return localPort + 1;
	} else {
		localPort = get_local_free_udp_port(NULL);
		if (localPort == -1) {
			LOG_DEBUG("cannot get a free local port");
			localPort = 0;
		}
		LOG_DEBUG("UDP port 5061 is busy, will use random port number : " + QString::number(localPort).toStdString());
		return localPort;
	}

	owsl_terminate();
}
