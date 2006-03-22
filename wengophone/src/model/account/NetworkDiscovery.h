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

#ifndef NETWORKDISCOVERY_H
#define NETWORKDISCOVERY_H

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sipwrapper/EnumNatType.h>

#include <util/Event.h>

#include <string>

/**
 * Discover Network configuration.
 *
 * @ingroup model
 * @author Philippe Bernery
 */
class NetworkDiscovery
{
public:

	/**
	 * Emitted when a proxy has been detected and needs a login/password.
	 *
	 * @param proxyAddress the url of the detected proxy
	 * @param proxyPort the port of the detected proxy
	 */
	Event< void(NetworkDiscovery & sender,
		const std::string & proxyAddress, unsigned proxyPort) > proxyNeedsAuthenticationEvent;

	/**
	 * Emitted when given login/password are wrong.
	 *
	 * @param sender this class
	 * @param proxyAddress the url of the detected proxy
	 * @param proxyPort the port of the detected proxy
	 * @param proxyLogin the maybe wrong login
	 * @param proxyPassword the maybe wrong password
	 */
	Event< void(NetworkDiscovery & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword)> wrongProxyAuthenticationEvent;

	NetworkDiscovery();

	~NetworkDiscovery();

	/**
	 * Tests if an url is joinable by Http.
	 *
	 * @param the url to test (e.g: "ws.wengo.fr:443/softphone-sso/sso.php")
	 * @param true if a SSL connection must be tested
	 * @return true if connection ok
	 */
	bool testHTTP(const std::string & url, bool ssl);

	/**
	 * Tests if a UDP connection is possible.
	 *
	 * @param stunServer the STUN server
	 * @return true if ok
	 */
	bool testUDP(const std::string & stunServer);

	/**
	 * Tests if a server respond to a SIP ping.
	 *
	 * @param server the server to test
	 * @param port the server port
	 * @return true if ok
	 */
	bool testSIP(const std::string & server, unsigned port);

	/**
	 * Tests if a HttpTunnel can be create and if SIP can pass through this tunnel.
	 *
	 * @param tunnelServer the tunnel server
	 * @param tunnelPort the tunnel port
 	 * @param ssl true if tunnel must be done with SSL
	 * @param sipServer the SIP server to ping
	 * @param sipServerPort the SIP server port
	 * @return true if ok
	 */
	bool testSIPHTTPTunnel(const std::string & tunnelServer, unsigned tunnelPort, bool ssl,
		const std::string & sipServer, unsigned sipServerPort);

	/**
	 * @return a free local port.
	 */
	unsigned getFreeLocalPort();

	/**
	 * Sets the proxy.
	 *
	 * Unblock discoverProxySettings if it was.
	 */
	void setProxySettings(const std::string & proxyServer, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

	/**
	 * @return the proxy server.
	 */
	const std::string getProxyServer() const {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		return config.getNetworkProxyServer();
	}

	/**
	 * @return the proxy server port.
	 */
	unsigned getProxyServerPort() const {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		return config.getNetworkProxyPort();
	}

	/**
	 * @return the proxy login.
	 */
	const std::string getProxyLogin() const {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		return config.getNetworkProxyLogin();
	}

	/**
	 * @return the proxy password.
	 */
	const std::string getProxyPassword() const {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		return config.getNetworkProxyPassword();
	}

private:

	/**
	 * @return false if proxy is not set (usually when proxy needs authentication), true otherwise
	 */
	void discoverProxySettings();

	void setNatConfig(EnumNatType::NatType natType);

	static const unsigned PING_TIMEOUT = 3;

	static const unsigned HTTP_TIMEOUT = 10;

	bool _proxySettingsSet;
};

#endif /*NETWORKDISCOVERY_H*/
