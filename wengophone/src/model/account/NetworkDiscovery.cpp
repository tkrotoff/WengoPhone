/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "NetworkDiscovery.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <thread/Thread.h>
#include <util/Logger.h>
#include <util/StringList.h>

#include <netlib.h>

using namespace std;

NetworkDiscovery::NetworkDiscovery() {
	_proxySettingsSet = false;
}

NetworkDiscovery::~NetworkDiscovery() {
}

bool NetworkDiscovery::testHTTP(const std::string & url, bool ssl) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	NETLIB_BOOLEAN sslActivated = ((ssl) ? NETLIB_TRUE : NETLIB_FALSE);
	bool isProxyDetected = config.getNetworkProxyDetected();
	unsigned retryCount = 2;

	while (retryCount != 0) {
		--retryCount;

		if (!isProxyDetected) {
			LOG_DEBUG("proxy not yet detected. Testing http connection without proxy");
			if (is_http_conn_allowed(url.c_str(),
				NULL, 0, NULL, NULL, sslActivated, HTTP_TIMEOUT) == HTTP_OK) {

				setProxySettings(String::null, 0, String::null, String::null);
				return true;
			} else {
				discoverProxySettings();
			}
		}

		string proxyAddress = config.getNetworkProxyServer();
		int proxyPort = config.getNetworkProxyPort();
		string proxyLogin = config.getNetworkProxyLogin();
		string proxyPassword = config.getNetworkProxyPassword();

		LOG_DEBUG("proxy detected. Testing http connection");
		if (is_http_conn_allowed(url.c_str(),
			proxyAddress.c_str(), proxyPort,
			proxyLogin.c_str(), proxyPassword.c_str(), sslActivated, HTTP_TIMEOUT) == HTTP_OK) {

			return true;
		}

		// If we cannot connect, we try again one time.
		// We try again because at first launch of the software,
		// the proxy has maybe been detected (and no proxy has been found)
		// the last time the application was running however
		// the software should maybe connect through a proxy this time.
		if (isProxyDetected && (retryCount != 0)) {
			config.set(Config::NETWORK_PROXY_DETECTED_KEY, false);
		}
	}

	LOG_DEBUG("cannot connect to " + url + (ssl ? " with" : " without") + " ssl");
	return false;
}

bool NetworkDiscovery::testUDP(const string & stunServer) {
	LOG_DEBUG("testing UDP connection and discovering NAT type with STUN server " + stunServer);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	bool isProxyDetected = config.getNetworkProxyDetected();
	string proxyAddress = config.getNetworkProxyServer();

	if (!isProxyDetected || (isProxyDetected && proxyAddress.empty())) {
		NatType natType;
		bool opened = (is_udp_port_opened(stunServer.c_str(), SIP_PORT, &natType) == NETLIB_TRUE ? true : false);

		EnumNatType::NatType nat = EnumNatType::NatTypeUnknown;

		switch(natType) {
		case StunTypeUnknown:
			nat = EnumNatType::NatTypeUnknown;
			break;
		case StunTypeOpen:
			nat = EnumNatType::NatTypeOpen;
			break;
		case StunTypeConeNat:
			nat = EnumNatType::NatTypeFullCone;
			break;
		case StunTypeRestrictedNat:
			nat = EnumNatType::NatTypeRestrictedCone;
			break;
		case StunTypePortRestrictedNat:
			nat = EnumNatType::NatTypePortRestrictedCone;
			break;
		case StunTypeSymNat:
			nat = EnumNatType::NatTypeSymmetric;
			break;
		case StunTypeSymFirewall:
			nat = EnumNatType::NatTypeSymmetricFirewall;
			break;
		case StunTypeBlocked:
			nat = EnumNatType::NatTypeBlocked;
			break;
		case StunTypeFailure:
			nat = EnumNatType::NatTypeFailure;
			break;
		default:
			LOG_FATAL("unknown NAT type=" + String::fromNumber(natType));
		}

		setNatConfig(nat);
		return opened;
	} else {
		return false;
	}
}

bool NetworkDiscovery::testSIP(const string & server, unsigned port) {
	LOG_DEBUG("pinging SIP server " + server + " on port " + String::fromNumber(port));
	return (udp_sip_ping(server.c_str(), port, PING_TIMEOUT) == NETLIB_TRUE ? true : false);
}

bool NetworkDiscovery::testSIPHTTPTunnel(const string & tunnelServer, unsigned tunnelPort, bool ssl,
	const string & sipServer, unsigned sipServerPort) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	bool isProxyDetected = config.getNetworkProxyDetected();

	if (!isProxyDetected) {
		LOG_DEBUG("proxy not yet detected. Testing SIP tunnel connection without proxy");
		if (is_tunnel_conn_allowed(tunnelServer.c_str(), tunnelPort,
			sipServer.c_str(), sipServerPort,
			NULL, 0, NULL, NULL,
			(ssl ? NETLIB_TRUE : NETLIB_FALSE), HTTP_TIMEOUT, NETLIB_TRUE, PING_TIMEOUT) == HTTP_OK) {

			setProxySettings(String::null, 0, String::null, String::null);
			return true;
		} else {
			discoverProxySettings();
		}
	}

	string proxyAddress = config.getNetworkProxyServer();
	int proxyPort = config.getNetworkProxyPort();
	string proxyLogin = config.getNetworkProxyLogin();
	string proxyPassword = config.getNetworkProxyPassword();

	LOG_DEBUG("proxy detected. Testing SIP tunnel connection");
	if (is_tunnel_conn_allowed(tunnelServer.c_str(), tunnelPort,
		sipServer.c_str(), sipServerPort,
		proxyAddress.c_str(), proxyPort, proxyLogin.c_str(), proxyPassword.c_str(),
		(ssl ? NETLIB_TRUE : NETLIB_FALSE), HTTP_TIMEOUT, NETLIB_TRUE, PING_TIMEOUT) == HTTP_OK) {

		return true;
	}

	LOG_DEBUG("cannot create a tunnel to " + tunnelServer + ":" + String::fromNumber(tunnelPort)
		+ " for SIP server " + sipServer + ":" + String::fromNumber(sipServerPort)
		+ (ssl ? " with " : " without ") + " ssl");

	return false;
}

unsigned NetworkDiscovery::getFreeLocalPort() {
	//TODO: change this code to choose a random code
	if (!is_local_udp_port_used(NULL, SIP_PORT)) {
		LOG_DEBUG("udp port 5060 is free");
		return SIP_PORT;
	} else if (!is_local_udp_port_used(NULL, SIP_PORT + 1)) {
		LOG_DEBUG("udp port 5060 is busy, will use 5061");
		return SIP_PORT + 1;
	} else {
		LOG_DEBUG("udp port 5061 is busy, will use random port");
		//TODO: should we use 0 for random port?
		return 0;
	}
}

void NetworkDiscovery::discoverProxySettings() {
	LOG_DEBUG("proxy test");
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	string proxyAddress = config.getNetworkProxyServer();
	int proxyPort = config.getNetworkProxyPort();

	bool isProxyDetected = config.getNetworkProxyDetected();

	if (!isProxyDetected) {
		LOG_DEBUG("searching for proxy");

		char * localProxyUrl = get_local_http_proxy_address();
		int localProxyPort = get_local_http_proxy_port();
		setProxySettings((localProxyUrl ? string(localProxyUrl) : string(String::null)), localProxyPort, String::null, String::null);

		if (localProxyUrl) {
			LOG_DEBUG("proxy found");
			config.set(Config::NETWORK_PROXY_SERVER_KEY, string(localProxyUrl));
			config.set(Config::NETWORK_PROXY_PORT_KEY, localProxyPort);
			proxyAddress = localProxyUrl;
			proxyPort = localProxyPort;
		} else {
			LOG_DEBUG("no proxy found");
			return;
		}
	}

	if (is_proxy_auth_needed(proxyAddress.c_str(), proxyPort, HTTP_TIMEOUT)) {
		LOG_DEBUG("proxy authentication needed");

		string proxyLogin = config.getNetworkProxyLogin();
		string proxyPassword = config.getNetworkProxyPassword();

		if (proxyLogin.empty()) {
			LOG_DEBUG("proxy needs login/password");
			_proxySettingsSet = false;

			proxyNeedsAuthenticationEvent(*this, proxyAddress, proxyPort);

			LOG_DEBUG("waiting for a login/password");
			while (!_proxySettingsSet) {
				Thread::msleep(100);
			}
		}

		proxyLogin = config.getNetworkProxyLogin();
		proxyPassword = config.getNetworkProxyPassword();
		proxyAddress = config.getNetworkProxyServer();
		proxyPort = config.getNetworkProxyPort();

		while (!is_proxy_auth_ok(proxyAddress.c_str(), proxyPort, proxyLogin.c_str(), proxyPassword.c_str(), HTTP_TIMEOUT)) {
			LOG_DEBUG("proxy needs valid login/password");
			_proxySettingsSet = false;

			wrongProxyAuthenticationEvent(*this, proxyAddress, proxyPort, proxyLogin, proxyPassword);

			LOG_DEBUG("waiting for a valid login/password");
			while (!_proxySettingsSet) {
				Thread::msleep(100);
			}

			proxyLogin = config.getNetworkProxyLogin();
			proxyPassword = config.getNetworkProxyPassword();
			proxyAddress = config.getNetworkProxyServer();
			proxyPort = config.getNetworkProxyPort();
		}
	}
}

void NetworkDiscovery::setProxySettings(const std::string & proxyServer, unsigned proxyPort,
	const std::string & proxyLogin, const std::string & proxyPassword) {

 	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NETWORK_PROXY_DETECTED_KEY, true);
	config.set(Config::NETWORK_PROXY_SERVER_KEY, proxyServer);
	config.set(Config::NETWORK_PROXY_PORT_KEY, (int) proxyPort);
	config.set(Config::NETWORK_PROXY_LOGIN_KEY, proxyLogin);
	config.set(Config::NETWORK_PROXY_PASSWORD_KEY, proxyPassword);

	_proxySettingsSet = true;
}

void NetworkDiscovery::setNatConfig(EnumNatType::NatType natType) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	std::string tmp = EnumNatType::toString(natType);
	LOG_DEBUG("NAT type=" + tmp);
	config.set(Config::NETWORK_NAT_TYPE_KEY, tmp);
}
