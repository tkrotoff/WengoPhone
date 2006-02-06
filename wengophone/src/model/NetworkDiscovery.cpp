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

#include <Logger.h>
#include <StringList.h>

using namespace std;

const string NetworkDiscovery::_stunAddress = "stun.wengo.fr";
const string NetworkDiscovery::_ssoAddress = "ws.wengo.fr";
const string NetworkDiscovery::_ssoURL = "/softphone-sso/sso.php";
const unsigned NetworkDiscovery::_pingTimeout = 3000;

NetworkDiscovery::NetworkDiscovery() {
}

NetworkDiscovery::~NetworkDiscovery() {
}

void NetworkDiscovery::discoverForSSO() {
	//Test HTTPS connection
	string url = _ssoAddress + ":443" + _ssoURL;
	
	if (is_http_conn_allowed(url.c_str(),
		NULL, 0, NULL, NULL, NETLIB_TRUE) == HTTP_OK) {
			LOG_DEBUG("SSO connection with SSL");
			setSSOConfig(true);
			discoveryDoneEvent(*this, DiscoveryResultSSOCanConnect);
			return;
	}

	if (!discoverProxySettings()) {
		return;
	}

	//Get proxy information from Settings
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	string proxyAddress = config.get(Config::NETWORK_PROXY_SERVER_KEY, string(""));
	int proxyPort = config.get(Config::NETWORK_PROXY_PORT_KEY, 0);
	string proxyLogin = config.get(Config::NETWORK_PROXY_LOGIN_KEY, string(""));
	string proxyPassword = config.get(Config::NETWORK_PROXY_PASSWORD_KEY, string(""));

	//Test HTTPS connection via proxy
	if (!proxyAddress.empty()) {
		if (is_http_conn_allowed(url.c_str(),
			proxyAddress.c_str(), proxyPort,
			proxyLogin.c_str(), proxyPassword.c_str(), NETLIB_TRUE) == HTTP_OK) {
				LOG_DEBUG("SSO connection with SSL via a proxy");
				setSSOConfig(true);
				setProxyConfig(proxyAddress.c_str(), proxyPort,
					proxyLogin.c_str(), proxyPassword.c_str());
				discoveryDoneEvent(*this, DiscoveryResultSSOCanConnect);
				return;
		}
	}

	//Test HTTP connection
	url = _ssoAddress + ":80" + _ssoURL;
	
	if (is_http_conn_allowed(url.c_str(),
		proxyAddress.c_str(), proxyPort,
		proxyLogin.c_str(), proxyPassword.c_str(), NETLIB_FALSE) == HTTP_OK) {
			LOG_DEBUG("SSO connection without SSL");
			setSSOConfig(false);
			discoveryDoneEvent(*this, DiscoveryResultSSOCanConnect);
			return;
	}

	LOG_DEBUG("cannot connect to SSO");
	discoveryDoneEvent(*this, DiscoveryResultSSOCannotConnect);
}

void NetworkDiscovery::discoverForSIP(const string & sipServer, int sipPort) {
	if (udpTest(sipServer, sipPort)) {
		discoveryDoneEvent(*this, DiscoveryResultSIPCanConnect);
		return;	
	}

	//Get Tunnel server from SSO config.
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	string tunnelServer = config.get(Config::NETWORK_TUNNEL_SERVER_KEY, string(""));

	//TODO: we should test the port given by SSO
	if (tunnelTest(sipServer.c_str(), sipPort,
		tunnelServer.c_str(),
		NULL, 0, NULL, NULL)) {
		
		discoveryDoneEvent(*this, DiscoveryResultSIPCanConnect);
		return;
	}

	if (!discoverProxySettings()) {
		return;	
	}

	//Get proxy information from Settings
	string proxyAddress = config.get(Config::NETWORK_PROXY_SERVER_KEY, string(""));
	int proxyPort = config.get(Config::NETWORK_PROXY_PORT_KEY, 0);
	string proxyLogin = config.get(Config::NETWORK_PROXY_LOGIN_KEY, string(""));
	string proxyPassword = config.get(Config::NETWORK_PROXY_PASSWORD_KEY, string(""));

	// Restart tunnel test with proxy settings if proxy found
	if (!proxyAddress.empty()) {
		if (tunnelTest(sipServer.c_str(), sipPort,
			tunnelServer.c_str(),
			proxyAddress.c_str(), proxyPort,
			proxyLogin.c_str(), proxyPassword.c_str())) {
			
			discoveryDoneEvent(*this, DiscoveryResultSIPCanConnect);
			return;
		}
	}

	discoveryDoneEvent(*this, DiscoveryResultSIPCannotConnect);
}

bool NetworkDiscovery::udpTest(const std::string sipServer, unsigned sipPort) {
	//STUN test
	NatType natType = get_nat_type(_stunAddress.c_str());
	setNatConfig(natType);

	if (!is_local_udp_port_used(NULL, SIP_PORT)) {
		LOG_DEBUG("udp port 5060 is free");
		setSIPConfig(SIP_PORT);
	} else if (!is_local_udp_port_used(NULL, SIP_PORT + 1)) {
		LOG_DEBUG("udp port 5060 is busy, will use 5061");
		setSIPConfig(SIP_PORT + 1);
	} else {
		LOG_DEBUG("udp port 5061 is busy, will use random port");
		//TODO: should we use 0 for random port?
		setSIPConfig(0);
	}

	if (is_udp_port_opened(_stunAddress.c_str(), SIP_PORT)
		&& udp_sip_ping(sipServer.c_str(), sipPort, _pingTimeout)) {
		LOG_DEBUG("can connect directly, no tunnel needed");
		setTunnelNeededConfig(false);
		return true;
	} else {
		return false;
	}
}

bool NetworkDiscovery::tunnelTest(const char * sipServer, int sipPort,
	const char * tunnelServer, 
	const char * proxyAddress, int proxyPort,
	const char * proxyLogin, const char * proxyPassword) {

	if (is_tunnel_conn_allowed(tunnelServer, HTTP_PORT,
		sipServer, sipPort,
		proxyAddress, proxyPort, proxyLogin, proxyPassword,
		NETLIB_FALSE, NETLIB_TRUE, _pingTimeout) == HTTP_OK) {
		//Save config to Settings, using Tunnel on HTTP_PORT without SSL
		LOG_DEBUG(string("SIP can connect through a tunnel on port 80 without SSL and proxy settings: login:")
			+ ((proxyAddress == NULL) ? "" : string(proxyAddress))
			+ ", port: " + String::fromNumber(proxyPort)
			+ ", login: " + ((proxyLogin == NULL) ? "" : string(proxyLogin))
			+ ", password: " + ((proxyPassword == NULL) ? "" : string(proxyPassword)));

		setTunnelConfig(tunnelServer, HTTP_PORT, false);
		setProxyConfig(proxyAddress, proxyPort, proxyLogin, proxyPassword);
		setTunnelNeededConfig(true);
		return true;
	}

	if (is_tunnel_conn_allowed(tunnelServer, HTTPS_PORT, 
		sipServer, sipPort,
		proxyAddress, proxyPort, proxyLogin, proxyPassword,
		NETLIB_FALSE, NETLIB_TRUE, _pingTimeout) == HTTP_OK) {
		//Save config to Settings, using Tunnel on HTTPS_PORT without SSL
		LOG_DEBUG(string("SIP can connect through a tunnel on port 443 without SSL and proxy settings: login:")
			+ ((proxyAddress == NULL) ? string("") : string(proxyAddress))
			+ ", port: " + String::fromNumber(proxyPort)
			+ ", login: " + ((proxyLogin == NULL) ? "" : string(proxyLogin))
			+ ", password: " + ((proxyPassword == NULL) ? "" : string(proxyPassword)));

		setTunnelConfig(tunnelServer, HTTPS_PORT, false);
		setProxyConfig(proxyAddress, proxyPort, proxyLogin, proxyPassword);
		setTunnelNeededConfig(true);
		return true;
	}

	if (is_tunnel_conn_allowed(tunnelServer, HTTPS_PORT, 
		sipServer, sipPort,
		proxyAddress, proxyPort, proxyLogin, proxyPassword,
		NETLIB_TRUE, NETLIB_TRUE, _pingTimeout) == HTTP_OK) {
		//Save config to Settings, using Tunnel on HTTPS_PORT with SSL
		LOG_DEBUG(string("SIP can connect through a tunnel on port 443 with SSL and proxy settings: login:")
			+ ((proxyAddress == NULL) ? "" : string(proxyAddress))
			+ ", port: " + String::fromNumber(proxyPort)
			+ ", login: " + ((proxyLogin == NULL) ? "" : string(proxyLogin))
			+ ", password: " + ((proxyPassword == NULL) ? "" : string(proxyPassword)));

		setTunnelConfig(tunnelServer, HTTPS_PORT, true);
		setProxyConfig(proxyAddress, proxyPort, proxyLogin, proxyPassword);
		setTunnelNeededConfig(true);
		return true;
	}


	return false;
}

bool NetworkDiscovery::discoverProxySettings() {
	LOG_DEBUG("proxy test");
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	string proxyServer = config.get(Config::NETWORK_PROXY_SERVER_KEY, string(""));
	if (proxyServer.empty()) {
		LOG_DEBUG("searching for proxy");
		//TODO: Check if proxy is already set
		char * localProxyUrl = get_local_http_proxy_address();
		int localProxyPort = get_local_http_proxy_port();

		if (localProxyUrl) {
			LOG_DEBUG("proxy found");
			config.set(Config::NETWORK_PROXY_SERVER_KEY, string(localProxyUrl));
			config.set(Config::NETWORK_PROXY_PORT_KEY, localProxyPort);

			if (is_proxy_auth_needed(localProxyUrl, localProxyPort)) {
				LOG_DEBUG("proxy needs a login/password");
				proxySettingsNeededEvent(*this, localProxyUrl, localProxyPort);
				return false;
			}
		} else {
			LOG_DEBUG("no proxy found");
		}
	} else {
		LOG_DEBUG("proxy already found");
	}

	return true;
}

void NetworkDiscovery::setProxyConfig(const char * proxyAddress, int proxyPort,
	const char * proxyLogin, const char * proxyPassword) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (proxyAddress) {
		config.set(Config::NETWORK_PROXY_SERVER_KEY, string(proxyAddress));
		config.set(Config::NETWORK_PROXY_PORT_KEY, proxyPort);
		if (proxyLogin) {
			config.set(Config::NETWORK_PROXY_LOGIN_KEY, string(proxyLogin));
			config.set(Config::NETWORK_PROXY_PASSWORD_KEY, string(proxyPassword));
		} else {
			config.set(Config::NETWORK_PROXY_LOGIN_KEY, string(""));
			config.set(Config::NETWORK_PROXY_PASSWORD_KEY, string(""));
		}
	} else {
		config.set(Config::NETWORK_PROXY_SERVER_KEY, string(""));
		config.set(Config::NETWORK_PROXY_PORT_KEY, string(""));
		config.set(Config::NETWORK_PROXY_LOGIN_KEY, string(""));
		config.set(Config::NETWORK_PROXY_PASSWORD_KEY, string(""));
	}
}

void NetworkDiscovery::setNatConfig(NatType nat) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	string natType;

	switch(nat) {
	case StunTypeUnknown:
	case StunTypeOpen:
	case StunTypeFailure:
		break;
	case StunTypeConeNat:
		natType = "cone";
		break;
	case StunTypeRestrictedNat:
		natType = "restricted";
		break;
	case StunTypePortRestrictedNat:
		natType = "portRestricted";
		break;
	case StunTypeSymNat:
		natType = "sym";
		break;
	case StunTypeSymFirewall:
		natType = "symfirewall";
		break;
	case StunTypeBlocked:
		natType = "blocked";
		break;
	};

	LOG_DEBUG("Nat type: " + natType);
	config.set(Config::NETWORK_NAT_TYPE_KEY, natType);
}

void NetworkDiscovery::setSSOConfig(bool ssoSSL) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NETWORK_SSO_SSL_KEY, ssoSSL);	
}

void NetworkDiscovery::setTunnelConfig(const char * server, int port, bool tunnelSSL) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (server) {
		config.set(Config::NETWORK_TUNNEL_SERVER_KEY, string(server));
		config.set(Config::NETWORK_TUNNEL_PORT_KEY, port);
	} else {
		config.set(Config::NETWORK_TUNNEL_SERVER_KEY, string(""));
		config.set(Config::NETWORK_TUNNEL_PORT_KEY, 0);
	}

	config.set(Config::NETWORK_TUNNEL_SSL_KEY, tunnelSSL);
}

void NetworkDiscovery::setSIPConfig(int port) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NETWORK_SIP_LOCAL_PORT_KEY, port);
}

void NetworkDiscovery::setTunnelNeededConfig(bool needed) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NETWORK_TUNNEL_NEEDED_KEY, needed);
}
